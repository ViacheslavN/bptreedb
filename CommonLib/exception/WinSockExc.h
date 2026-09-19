#pragma once

#include "exc_base.h"
namespace CommonLib
{
	class CWinSockExc : public CExcBase
	{
	public:
		CWinSockExc(int err = WSAGetLastError());
 		CWinSockExc(const std::string& err_msg, int err = WSAGetLastError());

		template<typename... Types>
		CWinSockExc(HRESULT err, const std::string& format, Types&&... args) : CExcBase(format, std::forward<Types>(args)...), m_err(err)
		{
			CExcBase::AddMsg(GetErrorDesc(err));
		}

		~CWinSockExc() noexcept;
		virtual std::shared_ptr<CExcBase> Clone() const;

		static std::string GetErrorDesc(int err);
	private:

	private:
		int m_err;

	};
}