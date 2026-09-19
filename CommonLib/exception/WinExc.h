#pragma once

#include "exc_base.h"

namespace CommonLib
{
	class CWinExc : public CExcBase
	{
		public:
			CWinExc(DWORD error = ::GetLastError());
			CWinExc(const CWinExc& exc);
			CWinExc(const std::string& msg, DWORD error = ::GetLastError());
			CWinExc(const std::wstring& msg, DWORD error = ::GetLastError());

			template<class TArg>
			CWinExc(const std::string& format, const TArg& arg, DWORD error = ::GetLastError()) : CExcBase(format, arg), m_error(error)
			{
				AddMsg(GetErrorMessageA(m_error));
			}

			template<class TArg1, class TArg2>
			CWinExc(const std::string& format, const TArg1& arg1, const TArg2& arg2, DWORD error = ::GetLastError()) : CExcBase(format, arg1, arg2), m_error(error)
			{
				AddMsg(GetErrorMessageA(m_error));
			}

			template<class TArg1, class TArg2, class TArg3>
			CWinExc(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, DWORD error = ::GetLastError()) : CExcBase(format, arg1, arg2, arg3), m_error(error)
			{
				AddMsg(GetErrorMessageA(m_error));
			}

			template<class TArg1, class TArg2, class TArg3, class TArg4>
			CWinExc(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4, DWORD error = ::GetLastError()) : CExcBase(format, arg1, arg2, arg3, arg4), m_error(error)
			{
				AddMsg(GetErrorMessageA(m_error));
			}


			template<class TArg>
			CWinExc(const std::wstring& format, const TArg& arg, DWORD error = ::GetLastError()) : CExcBase(format, arg), m_error(error)
			{
				AddMsg(GetErrorMessageA(m_error));
			}
					

			template<class TArg1, class TArg2>
			CWinExc(const std::wstring& format, const TArg1& arg1, const TArg2& arg2, DWORD error = ::GetLastError()) : CExcBase(format, arg1, arg2), m_error(error)
			{
				AddMsg(GetErrorMessageA(m_error));
			}

					   			 
			virtual ~CWinExc();

			virtual std::shared_ptr<CExcBase> Clone() const;
			DWORD  GetErrCode() const;

			static bool GetSysMessage(DWORD errCode, std::string& result);
			static bool GetWinInetMessage(DWORD errCode, std::string& result);

			static std::string GetErrorMessageA(DWORD errCode);
			static std::wstring GetErrorMessageW(DWORD errCode);

			virtual void AddMsg(DWORD error = ::GetLastError());
			virtual void AddMsg(const std::string& msg, DWORD error = ::GetLastError());
			virtual void AddMsg(const std::string& format, const std::string& msg, DWORD error = ::GetLastError());
			virtual void AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2, DWORD error = ::GetLastError());
			virtual void AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2, const std::string& msg3, DWORD error = ::GetLastError());
			virtual void AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2, const std::string& msg3, const std::string& msg4, DWORD error = ::GetLastError());

			virtual void AddMsg(const std::wstring& msg, DWORD error = ::GetLastError());
			virtual void AddMsg(const std::wstring& format, const std::wstring& msg, DWORD error = ::GetLastError());
			virtual void AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2, DWORD error = ::GetLastError());
			virtual void AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2, const std::wstring& msg3, DWORD error = ::GetLastError());
			virtual void AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2, const std::wstring& msg3, const std::wstring& msg4, DWORD error = ::GetLastError());

		protected:
			DWORD m_error;
	};
}