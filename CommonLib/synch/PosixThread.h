#pragma once

#include <functional>
#include <pthread.h>
#include <string>
#include "CommonLib.h"

namespace CommonLib
{
	namespace synch
	{
		class CThreadPosixImpl
		{
		public:
			CThreadPosixImpl(std::function<void()>&& threadFunk);
			~CThreadPosixImpl();

			bool Wait(const int Timeout = 0xFFFFFFFF) const;
			static void SetDescriptionForCurrThread(const astr& threadName);
		private:
			pthread_t m_tid;
            std::function<void()> m_threadFunk;
		};

		typedef CThreadPosixImpl CThreadImpl;
	}
}
