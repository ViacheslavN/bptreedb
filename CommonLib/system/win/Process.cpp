#include "stdafx.h"
#include "Process.h"
#include "../../str/str.h"
#include "../../str/StringEncoding.h"
#include "PipeWriteRead.h"


namespace CommonLib
{
	namespace system
	{
		namespace win
		{
			CProcess::CProcess(const std::string & filePath, const std::string& args, const std::vector<CEnvValue> &envs)
			{
                try
                {
                    m_ptrChildStdIN = WinPipe::Create();;
                    m_ptrChildStdOut = WinPipe::Create();
                    m_ptrStderrPipe = WinPipe::Create();


                    STARTUPINFOW startupInfo;
                    ZeroMemory( &startupInfo, sizeof( startupInfo ) );
                    startupInfo.cb = sizeof( startupInfo );

                    SetPipes(startupInfo);

                    PROCESS_INFORMATION procInfo;
                    memset(&procInfo, 0, sizeof(procInfo));

                    std::wstring envw;
                    for ( size_t i = 0, size = envs.size(); i < size; ++i )
                    {
                        std::string variable = str_format::StrFormatSafe("{0}={1}", envs[i].GetName(), envs[i].GetValue());

                        envw += static_cast<std::wstring>(StringEncoding::str_utf82w_safe(variable)) + ( wchar_t )0;
                    }
                    envw += ( wchar_t )0;

                    void *ptrEnv = envs.empty() ? NULL : (void *)envw.data();

                    if(!::CreateProcessW(StringEncoding::str_utf82w_safe(filePath).c_str(), ( LPWSTR )StringEncoding::str_utf82w_safe(args).c_str(), NULL, NULL, TRUE,
                                     CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW, ptrEnv, NULL, &startupInfo, &procInfo))
                        throw CWinExc();

                    m_hProcess = procInfo.hProcess;
                    m_pid = ::GetProcessId( m_hProcess );
                    if ( 0 == m_pid )
                        throw CWinExc( "Cannot get process Id" );

                    m_ptrChildStdIN->CloseReadEnd();
                    m_ptrChildStdOut->CloseWriteEnd();
                    m_ptrStderrPipe->CloseWriteEnd();


                }
                catch (std::exception& exc)
                {
                    CExcBase::RegenExcT("CProcess: Failed to create process FilePath: {0}, Args: {1}", filePath, args,exc);
                    throw;
                }
			}


            void CProcess::SetPipes(STARTUPINFOW& startupInfo)
            {

                if (!SetHandleInformation(m_ptrChildStdIN->GetWriteFd()->GetHandleExc(), HANDLE_FLAG_INHERIT, 0))
                    throw CExcBase("CProcess: failed to remove inheritance flag");
                if (!SetHandleInformation(m_ptrChildStdOut->GetReadFd()->GetHandleExc(), HANDLE_FLAG_INHERIT, 0))
                    throw CExcBase("CProcess: failed to remove inheritance flag");

                startupInfo.hStdError = m_ptrStderrPipe->GetWriteFd()->GetHandleExc();
                startupInfo.hStdOutput = m_ptrChildStdOut->GetWriteFd()->GetHandleExc();
                startupInfo.hStdInput = m_ptrChildStdIN->GetReadFd()->GetHandleExc();

                startupInfo.dwFlags |= STARTF_USESTDHANDLES;

            }


			CProcess::~CProcess()
			{
				if (IsValid())
				{
					::CloseHandle(m_hProcess);
					m_hProcess = INVALID_HANDLE_VALUE;
				}
			}

			bool CProcess::IsValid() const
			{
				return m_hProcess == INVALID_HANDLE_VALUE;
			}

			void CProcess::IsValidExc() const
			{
				if (!IsValid())
					throw CExcBase("Process handle is invalid");
			}

            std::string  CProcess::GetFullImageName() const
			{
				IsValidExc();
				DWORD size = MAX_PATH;
				WCHAR name[MAX_PATH];
				BOOL success = ::QueryFullProcessImageName(m_hProcess, 0, name, &size);
				if (!success)
					throw CWinExc("Failed to call QueryFullProcessImageName");

				return StringEncoding::str_w2utf8_safe(name);
			}

            int32_t CProcess::GetPid() const
            {
                return m_pid;
            }

            bool CProcess::Wait(int32_t timeoutSec)
            {
                DWORD res = ::WaitForSingleObject(m_hProcess, timeoutSec * 1000);
                if(res == WAIT_FAILED)
                    throw CWinExc("CProcess: Failed to wait");

                return WAIT_TIMEOUT == res;
            }

            int32_t CProcess::GetExitCode()  const
            {
                DWORD exitCode = 0;
                if (!GetExitCodeProcess(m_hProcess, &exitCode))
                    throw CWinExc("CProcess: Failed to get exit code for process (PID: {0})", m_pid);

                if (STILL_ACTIVE == exitCode)
                    throw CExcBase("CProcess: (PID: {0}) is still active", m_pid);

                return (int32_t)exitCode;
            }

            bool CProcess::IsRunning() const
            {
                DWORD exitCode = 0;
                if (!::GetExitCodeProcess(m_hProcess, &exitCode))
                    throw CExcBase("CProcess:  Failed to get response from process (PID: {0})", m_pid);

                return exitCode == STILL_ACTIVE;
            }

            void CProcess::SetPriority(int level)
            {
                if(SetPriorityClass(m_hProcess, level) == FALSE)
                    throw CWinExc("CProcess: failed to set priority (PID: {0}, Priority {1})", m_pid, level);
            }

            void CProcess::Terminate()
            {
                if (!TerminateProcess(m_hProcess, NO_ERROR))
                {
                    if (IsRunning())
                    {
                        throw CWinExc("CProcess: failed to terminate process (PID: {0})", m_pid);
                    }
                }
            }

            io::TWriteReadPtr CProcess::GetStdio() const
            {
                return  std::make_shared<PipeWriteRead>( m_ptrChildStdOut, m_ptrChildStdIN);
            }

            io::TWriteReadPtr CProcess::GetStderr() const
            {
                return m_ptrStderrPipe;
            }

		}


        IProcessPtr IProcess::StartProcess(const std::string & filePath, const std::string& args, const std::vector<CEnvValue> &envs)
        {
            return std::make_shared<win::CProcess>(filePath, args, envs);
        }
	}
}