#include "stdafx.h"
#include "WinThread.h"
#include "../exception/WinExc.h"
#include "../str/StringEncoding.h"

namespace CommonLib
{
	namespace synch
	{

		extern "C" 
		{

			unsigned __stdcall thread_func(void* param)
			{

				std::function<void()>* threadFunc = reinterpret_cast<std::function<void()>*>(param);
				(*threadFunc)();
				return 0;
			}

		}


		CThreadWinImpl::CThreadWinImpl(std::function<void()>&& threadFunk) : m_threadFunk(threadFunk)
		{
			unsigned int  threadId;
			m_hThread = (HANDLE)(_beginthreadex(NULL, NULL, &thread_func, &m_threadFunk, NULL, &threadId));
			if (m_hThread == INVALID_HANDLE_VALUE)
				throw CWinExc("Failed to create thread");
		}

		CThreadWinImpl::~CThreadWinImpl()
		{
			::WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
		}

		bool CThreadWinImpl::Wait(const int Timeout) const
		{
			return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hThread, Timeout);
		}

		using SetThreadDescriptionFunc = HRESULT(WINAPI*)(HANDLE hThread, PCWSTR lpThreadDescription);

		void CThreadWinImpl::SetDescriptionForCurrThread(const std::string& threadName)
		{
			static SetThreadDescriptionFunc setThreadDescription = reinterpret_cast<SetThreadDescriptionFunc>(::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "SetThreadDescription"));
			if (setThreadDescription)
				setThreadDescription(GetCurrentThread(), StringEncoding::str_a2w_safe(threadName).c_str());
		}

		int32_t CThreadWinImpl::GetExitCodeThread()
		{
			DWORD code;
			::GetExitCodeThread(m_hThread, &code);

			return code;
		}

		void CThreadWinImpl::TerminateThread(int32_t code)
		{
			::TerminateThread(m_hThread, code);
		}
	}
}