#include "PosixThread.h"
#include <linux/version.h>
#include <sys/prctl.h>
#include <cerrno>
#include "exception/PosixExc.h"
 
namespace CommonLib
{
	namespace synch
	{

		extern "C"
		{

			static void * thread_func(void* param)
			{

				std::function<void()>* threadFunc = reinterpret_cast<std::function<void()>*>(param);
				(*threadFunc)();
				return 0;
			}

		}


		CThreadPosixImpl::CThreadPosixImpl(std::function<void()>&& threadFunk) : m_threadFunk(threadFunk)
		{
			pthread_attr_t threadAttr;
			if (pthread_attr_init(&threadAttr) == -1)
			{
				throw CPosixExc("pthread_attr_init", errno);
			}

			int ret = pthread_create(&m_tid, &threadAttr,	thread_func, &m_threadFunk);
			if (ret != 0) {

				throw CPosixExc("pthread_create", errno);
			}

			if (pthread_attr_destroy(&threadAttr) == -1)
			{
				throw CPosixExc("pthread_attr_destroy", errno);
			}
		}

		CThreadPosixImpl::~CThreadPosixImpl()
		{
			pthread_join(m_tid, NULL);
		}

		bool CThreadPosixImpl::Wait(const int Timeout) const
		{
			return (0 == pthread_join(m_tid, NULL));
		}

		void CThreadPosixImpl::SetDescriptionForCurrThread(const std::string& threadName)
		{
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,9) )
			prctl(PR_SET_NAME, threadName.c_str(), 0, 0, 0);
#endif
		}
	}
}
 