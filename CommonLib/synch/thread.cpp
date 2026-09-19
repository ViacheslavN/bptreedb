#include "thread.h"
#include "exception/exc_base.h"

namespace CommonLib
{
	namespace synch
	{
		thread_id_t CThread::GetCurThreadId()
		{
#ifdef _WIN32
			return (thread_id_t)GetCurrentThreadId();
#else
			return (thread_id_t)pthread_self();
#endif 
		}

		void  CThread::SetDescriptionForCurrThread(const std::string& threadName)
		{
			CThreadImpl::SetDescriptionForCurrThread(threadName);
		}


		CThread::CThread(std::function<void()>&& threadFunk, std::string&& threadName) : m_threadFunk(threadFunk), m_name(threadName)
		{
			try
			{
			
				m_thread.reset(new CThreadImpl([this]() {
				
					RunThread(m_threadFunk, m_name);
				}				
				));
			}
			catch (std::exception& ex)
			{
				m_exeption = CExcBase::CloneFromExc(ex);
				m_exeption->AddMsgT("Failed to create thread {0}", (uint64_t)GetCurThreadId());
			}
		}

		CThread::~CThread()
		{

		}

		void CThread::RunThread(std::function<void()> threadFunk, std::string threadName)
		{
			try
			{
				SetDescriptionForCurrThread(m_name);
				threadFunk();
			}
			catch (std::exception& ex)
			{
				m_exeption = CExcBase::CloneFromExc(ex);
				m_exeption->AddMsgT("Failed in thread {0}", (uint64_t)GetCurThreadId());
			}
		}

		bool CThread::IsActive() const
		{
			ThrowException();
			
			if (m_thread.get() == nullptr)
				return false;
			
			return m_thread->Wait(0);
		}

		bool CThread::IsHaveException() const
		{
			return m_exeption.get() != nullptr;
		}

        std::string CThread::GetExceptionMsg() const
		{
			if (m_exeption.get() == nullptr)
				return std::string();

			return m_exeption->what();
		
		}
		void CThread::ThrowException() const
		{
			if (m_exeption.get() != nullptr)
				m_exeption->Throw();
		}

		void CThread::Join()
		{
			Join(-1);
		}

		bool CThread::Join(int timeoutms)
		{
			ThrowException();
			if (m_thread.get() == nullptr)
				return false;

			if (timeoutms < 0)
			{
				m_thread->Wait();
				return true;
			}

			return m_thread->Wait(timeoutms);
		}

		void CThread::Sleep(uint32_t msec)
		{
#ifdef _WIN32
			::Sleep(msec);
#else
			struct timespec ts;
			ts.tv_sec = msec / 1000;
			ts.tv_nsec = (msec % 1000) * 1000000;
			nanosleep(&ts, NULL);
#endif
		}

#ifdef _WIN32
		int32_t CThread::GetExitCodeThread()
		{
			return m_thread->GetExitCodeThread();
		}
 
		void CThread::TerminateThread(int32_t code)
		{
			m_thread->TerminateThread(code);
		}
#endif
	}
}