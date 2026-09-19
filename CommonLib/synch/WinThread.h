#pragma once

#include <functional>
#include "../CommonLib.h"

namespace CommonLib
{
	namespace synch
	{
		class CThreadWinImpl
		{
		public:
			CThreadWinImpl(std::function<void()>&& threadFunk);
			~CThreadWinImpl();

			bool Wait(const int Timeout = 0xFFFFFFFF) const;
			static void SetDescriptionForCurrThread(const  std::string& threadName);
			int32_t GetExitCodeThread();
			void  TerminateThread(int32_t code);


		private:
			HANDLE m_hThread;
			std::function<void()> m_threadFunk;
 		};

		typedef CThreadWinImpl CThreadImpl;
	}
}