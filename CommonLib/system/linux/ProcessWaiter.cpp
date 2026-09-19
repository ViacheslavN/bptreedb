#include "stdafx.h"
#include "ProcessWaiter.h"
#include "../../synch/thread.h"
#include <sys/types.h>
#include <sys/wait.h>

namespace CommonLib {
    namespace system {
        namespace lin {

            CProcessWaiter::CProcessWaiter(pid_t pid) :
                    m_pid(pid),
                    m_ptrMutex(new std::mutex()),
                    m_ptrResult(new WaitingResult)
            {

            }


            CProcessWaiter::WaitingResult CProcessWaiter::GetResult()
            {
                return *m_ptrResult;
            }

            int CProcessWaiter::ProcessWaitingResult(const WaitingResult& result)
            {
                if (result.waitpid_result == m_pid)
                {
                    // now thread is not running exactly, but process maybe
                    if (WIFEXITED(result.status))
                    {
                        //now process is not running exactly
                        return WEXITSTATUS(result.status);
                    }
                    else if (WIFSIGNALED(result.status))
                    {
                        const int signalNumber = WTERMSIG(result.status);
                        throw CExcBase("Process {0} was terminated by signal {1}", m_pid, signalNumber);
                    }
                    else
                        throw CExcBase("Timed out waiting for the process. Waitpid status: {0}", result.status);
                 }
                else
                     throw CExcBase("Timed out waiting for the process, error {0}", result.errno_result);
            }

            int CProcessWaiter::WaitForExit(int32_t timeoutSec)
            {

                synch::CThread waitThread([pid = this->m_pid,
                                                      ptrResult = this->m_ptrResult, ptrMutex = this->m_ptrMutex]()
                                              {
                                                  pid_t result = waitpid(pid, &(ptrResult->status), 0);
                                                  std::lock_guard<std::mutex> lock_guard(*ptrMutex);
                                                  ptrResult->waitpid_result = result;
                                                  ptrResult->errno_result = errno;
                                              },
                                              "Waiting thread"
                                              );

                bool waitSuccesfull = waitThread.Join(timeoutSec);

                if (waitSuccesfull)
                {
                    try
                    {
                        return ProcessWaitingResult(GetResult());
                    }
                    catch (std::exception& ex)
                    {
                        CExcBase::RegenExcT("Failed to get timeout result for process {0}", m_pid, ex);
                        throw;
                    }
                }
                else
                   throw CExcBase("Timed out waiting for the process to finish");

            }


        }
    }
}