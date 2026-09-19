#include "stdafx.h"
#include "../../exception/PosixExc.h"
#include "Process.h"
#include "../../str/str.h"
#include "../../str/StringEncoding.h"
#include "../../filesystem/PathUtils.h"
#include "../../str/StrUtils.h"
#include "SignalHandler.h"
#include "PipeWriteRead.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include "ProcessWaiter.h"
#include <iostream>

namespace CommonLib {
    namespace system {
        namespace lin {

            CProcess::CProcess(const std::string & filePath, const std::string& args, const std::vector<CEnvValue> &envs) : m_filePath(filePath)
            {
                try
                {
                    m_ptrChildStdIN = CLinPipe::Create();;
                    m_ptrChildStdOut = CLinPipe::Create();
                    m_ptrStderrPipe = CLinPipe::Create();


                    pid_t childPid = fork();
                    if (childPid == -1)
                        throw CPosixExc(errno, "Failed to start process: {0}", filePath);


                    if (childPid == 0)
                    {
                        m_ptrChildStdOut->AttachToStdout();
                        m_ptrStderrPipe->AttachToStderr();
                        m_ptrChildStdIN->AttachToStdin();

                        CloseAllFds();


                        std::string fileName = CPathUtils::FindFileName(filePath);
                        if (fileName.empty())
                            fileName = filePath;

                        std::vector<char *> argsPtrsVec, envPtrs;
                        std::vector<std::string> argsVec = StringUtils::ArgsStringToArgsVector(args);

                        PepareArguments(fileName, argsVec, envs, argsPtrsVec, envPtrs);

                        CSignalHandler sigChild(SIGCHLD, SIG_DFL);

                        if (envs.empty()) {
                            execvp(filePath.c_str(), &argsPtrsVec[0]);
                        } else {
                            execve(filePath.c_str(), &argsPtrsVec[0], &envPtrs[0]);
                        }

                        const int errorCode = errno;

                        std::cerr << str_format::AStrFormatSafeT(
                                "Failed to execute {0} in child process (errno: {1}): {2}.", filePath, errorCode,
                                CPosixExc::GetErrorMessageA(errorCode).c_str());
                        std::cerr.flush();

                        _exit(errorCode);
                    }

                    m_ptrChildStdIN->CloseReadEnd();
                    m_ptrStderrPipe->CloseWriteEnd();
                    m_ptrChildStdOut->CloseWriteEnd();
                    m_pid = childPid;
                }
                catch (std::exception& ex)
                {
                    std::cerr << ex.what() << std::endl;
                    std::cerr.flush();

                    ::_exit(EXIT_FAILURE);
                }
            }

            CProcess::~CProcess()
            {

            }

            void CProcess::PepareArguments(const std::string & fileName, const std::vector<std::string> & argsVec,
                                 const std::vector<CEnvValue> &env, std::vector<char*>& argsPtrsVec, std::vector<char*>& envPtrs)
            {
                argsPtrsVec.push_back((char *)fileName.c_str());

                for (size_t i = 0; i < argsVec.size(); ++i)
                    argsPtrsVec.push_back((char *)argsVec[i].c_str());

                argsPtrsVec.push_back((char *)0);

                for (size_t i = 0; i < env.size(); ++i)
                {
                    char* ptrEnv = const_cast<char*>((const char *)(env[i]));
                    envPtrs.push_back(ptrEnv);
                }
                envPtrs.push_back((char *)0);
            }

            void CProcess::CloseAllFds()
            {
                int32_t maxfd = sysconf(_SC_OPEN_MAX);
                const int32_t MAX_FD_TO_CLOSE = 8192;
                const int32_t LOW_FD = 3; //0-stdin, 1-stdout, 2-stderr
                if (maxfd == -1)
                    maxfd = MAX_FD_TO_CLOSE;

                for (int i = LOW_FD; i < maxfd; ++i)
                {
                    close(i);
                }
            }

            int32_t CProcess::GetPid() const
            {
                return m_pid;
            }

            std::string CProcess::GetFullImageName() const
            {
                return m_filePath;
            }

            bool CProcess::Wait(int32_t timeoutSec)
            {
                struct sigaction sigChildAction;
                sigaction(SIGCHLD, NULL, &sigChildAction);
                if (sigChildAction.sa_handler != SIG_DFL)
                    throw CExcBase("Invalid SIGCHLD handler");

                CProcessWaiter processWaiter(m_pid);
                try
                {
                    return processWaiter.WaitForExit(timeoutSec);
                }
                catch (std::exception& exc)
                {
                    try
                    {
                        TerminateImpl();
                    }
                    catch (std::exception& ex)
                    {
                        //TODO log
                    }
                    CExcBase::RegenExc("Timed out waiting for the process", exc);
                    throw;
                }
            }

            int32_t CProcess::GetExitCode()  const
            {
                int status = 0;
                pid_t result = waitpid(m_pid, &status, 0);
                if (result != m_pid)
                {
                    throw CPosixExc("Failed to get status of process with PID", errno);
                }

                if (WIFEXITED(status))
                {
                    return WEXITSTATUS(status);
                }
                else
                {
                    throw CExcBase("Unexpected status: {0}", status);
                }
            }

            bool CProcess::IsRunning() const
            {
                return IsRunningImpl();
            }

            void CProcess::SetPriority(int level)
            {

            }

            void CProcess::Terminate()
            {
                TerminateImpl();
            }

            io::TWriteReadPtr CProcess::GetStdio() const
            {
                return  std::make_shared<PipeWriteRead>( m_ptrChildStdOut, m_ptrChildStdIN);
            }

            io::TWriteReadPtr CProcess::GetStderr() const
            {
                return m_ptrStderrPipe;
            }

            bool CProcess::IsRunningImpl() const
            {
                int status = 0;
                return (waitpid(m_pid, &status, WNOHANG) == 0) && (m_pid > 0);
            }

            void CProcess::TerminateImpl()
            {
                if (IsRunningImpl())
                {
                    if ((::kill(m_pid, SIGTERM) != 0) && (errno != ESRCH))
                        throw CPosixExc("Failed to terminate", errno);
                }
            }

        }



        IProcessPtr IProcess::StartProcess(const std::string & filePath, const std::string& args, const std::vector<CEnvValue> &envs)
        {
            return std::make_shared<lin::CProcess>(filePath, args, envs);
        }
    }
}