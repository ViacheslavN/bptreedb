#pragma once
#include "../IProcess.h"
#include "LinPipe.h"


namespace CommonLib {
    namespace system {
        namespace lin {

            class CProcessWaiter
            {
            public:
                CProcessWaiter(pid_t pid);
                int WaitForExit(int32_t timeoutSec);
            private:
                struct WaitingResult
                {
                    int status = 0;
                    pid_t waitpid_result = 0;
                    int errno_result = 0;
                };

                WaitingResult GetResult();
                int ProcessWaitingResult(const WaitingResult& result);

                pid_t m_pid;
                std::shared_ptr<std::mutex> m_ptrMutex;
                std::shared_ptr<WaitingResult> m_ptrResult;
            };
        }
    }
}