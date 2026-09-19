#include "stdafx.h"
#include "WinNtExc.h"
#include "../str/StringEncoding.h"

namespace CommonLib
{
	CWinNtExc::CWinNtExc(NTSTATUS ntStatus) : m_ntStatus(ntStatus)
	{
		AddMsg(GetErrorDesc(m_ntStatus));
	}

	CWinNtExc::CWinNtExc(const std::string& err_msg, NTSTATUS ntStatus) : CExcBase(err_msg), m_ntStatus(ntStatus)
	{
		AddMsg(GetErrorDesc(m_ntStatus));
	}
	
	CWinNtExc::~CWinNtExc() noexcept
	{

	}

	std::shared_ptr<CExcBase> CWinNtExc::Clone() const
	{
		std::shared_ptr<CExcBase> ptrExc(new CWinNtExc(*this));
		return ptrExc;
	}

    std::string CWinNtExc::GetErrorDesc(NTSTATUS ntStatus)
	{
		LPWSTR pBuffer;
        std::string result;
		if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_FROM_HMODULE,
			GetModuleHandleW(L"ntdll.dll"),
			ntStatus,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&pBuffer,
			0,
			NULL))
		{
			result = StringEncoding::str_w2utf8_safe(pBuffer);
			LocalFree(pBuffer);
			return result;
		}

		return "Unknown error";
	}
}