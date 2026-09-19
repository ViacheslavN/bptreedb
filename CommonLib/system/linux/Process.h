#pragma once
#include "../IProcess.h"
#include "LinPipe.h"


namespace CommonLib {
    namespace system {
        namespace lin {

            typedef std::shared_ptr<class CProcess> CProcessPtr;

            class CProcess : public IProcess
            {
            public:
                CProcess(const std::string & filePath, const std::string& args, const std::vector<CEnvValue> &envs);
                ~CProcess();

                bool IsValid() const;

                virtual int32_t GetPid() const;
                virtual std::string GetFullImageName() const;
                virtual bool Wait(int32_t timeoutSec);
                virtual int32_t GetExitCode()  const;
                virtual bool IsRunning() const;
                virtual  void SetPriority(int level);
                virtual void Terminate();

                virtual io::TWriteReadPtr GetStdio() const;
                virtual io::TWriteReadPtr GetStderr() const;
            private:
                void CloseAllFds();
                void PepareArguments(const std::string & fileName, const std::vector<std::string> & argsVec,
                                                          const std::vector<CEnvValue> &env, std::vector<char*>& argsPtrsVec, std::vector<char*>& envPtrs);

                bool IsRunningImpl() const;
                void TerminateImpl();


            private:
                pid_t m_pid;

                LinPipePtr m_ptrChildStdIN;
                LinPipePtr m_ptrChildStdOut;
                LinPipePtr m_ptrStderrPipe;
                std::string m_filePath;
            };
        }
    }
}

