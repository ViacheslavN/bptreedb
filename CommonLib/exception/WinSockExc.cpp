#include "stdafx.h"
#include "WinSockExc.h"
#include "../str/StringEncoding.h"


namespace CommonLib
{
	CWinSockExc::CWinSockExc(int err) : m_err(err)
	{
		CExcBase::AddMsg(GetErrorDesc(m_err));
	}

	CWinSockExc::CWinSockExc(const std::string& err_msg, int err) : CExcBase(err_msg), m_err(err)
	{
		CExcBase::AddMsg(GetErrorDesc(m_err));
	}


	CWinSockExc::~CWinSockExc() noexcept
	{

	}

	std::shared_ptr<CExcBase> CWinSockExc::Clone() const
	{
		std::shared_ptr<CWinSockExc> ptrExc(new CWinSockExc(*this));
		return ptrExc;
	}

    std::string CWinSockExc::GetErrorDesc(int errCode)
	{
		LPWSTR pBuffer = NULL;
        std::string result;
		if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errCode, 0, (LPWSTR)&pBuffer, 0, NULL))
		{
			if (pBuffer)
			{
				result = StringEncoding::str_w2utf8_safe(pBuffer);
				LocalFree(pBuffer);

			}
		}

		return result;
	}
}