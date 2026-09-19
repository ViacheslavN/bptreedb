#pragma once
#include "../IProcess.h"
#include "WinPipe.h"

namespace CommonLib
{
	namespace system
	{
		namespace win
		{
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
				void IsValidExc() const;
                void SetPipes(STARTUPINFOW& startupInfo);
			private:
				HANDLE m_hProcess;
                int32_t  m_pid;
                WinPipePtr m_ptrChildStdIN;
                WinPipePtr m_ptrChildStdOut;
                WinPipePtr m_ptrStderrPipe;
    		};
		}
	}
}
