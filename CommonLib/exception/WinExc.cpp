#include "stdafx.h"
#include "WinExc.h"
#include "../str/StringEncoding.h"

namespace CommonLib
{

CWinExc::CWinExc(DWORD error) : m_error(error)
{
	AddMsg(GetErrorMessageA(m_error));
}

CWinExc::CWinExc(const CWinExc& exc) : CExcBase(exc), m_error(exc.m_error)
{
	AddMsg(GetErrorMessageA(m_error));
}

CWinExc::CWinExc(const std::string& msg, DWORD error) : CExcBase(msg), m_error(error)
{
	AddMsg(GetErrorMessageA(m_error));
}	

CWinExc::CWinExc(const std::wstring& msg, DWORD error) : CExcBase(msg), m_error(error)
{
	AddMsg(GetErrorMessageA(m_error));
}

CWinExc::~CWinExc()
{

}

std::shared_ptr<CExcBase>  CWinExc::Clone() const
{
	std::shared_ptr<CExcBase> ptrExc(new CWinExc(*this));
	return ptrExc;
}

DWORD  CWinExc::GetErrCode() const
{
	return m_error;
}

bool CWinExc::GetSysMessage(DWORD errCode, std::string& result)
{
	LPWSTR pBuffer = NULL;
	if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		errCode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPWSTR)&pBuffer, 0, NULL) && pBuffer)
	{
		result = StringEncoding::str_w2utf8_safe(pBuffer);
		LocalFree(pBuffer);
		return true;
	}
	return false;
}

bool CWinExc::GetWinInetMessage(DWORD errCode, std::string& result)
{
	LPWSTR pBuffer = NULL;
	if (errCode >= INTERNET_ERROR_BASE && errCode <= INTERNET_ERROR_LAST)
		if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE, GetModuleHandleW(L"wininet.dll"), errCode, 0, (LPWSTR)&pBuffer, 0, NULL))
			if (pBuffer)
			{
				result = StringEncoding::str_w2utf8_safe(pBuffer);
				LocalFree(pBuffer);
				return true;
			}
	return false;
}

std::string CWinExc::GetErrorMessageA(DWORD errCode)
{
    std::string errorDescr;
	if (!GetSysMessage(errCode, errorDescr))
		GetWinInetMessage(errCode, errorDescr);

	return errorDescr;
}

    std::wstring CWinExc::GetErrorMessageW(DWORD errCode)
{
	return StringEncoding::str_utf82w_safe(GetErrorMessageA(errCode));
}

void CWinExc::AddMsg(DWORD error)
{
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::string& msg, DWORD error)
{
	CExcBase::AddMsg(msg);
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::string& format, const std::string& msg, DWORD error)
{
	CExcBase::AddMsg(format, msg);
	CExcBase::AddMsg(GetErrorMessageA(error));
}
 
void CWinExc::AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2, DWORD error)
{
	CExcBase::AddMsg(format, msg1, msg2);
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2, const std::string& msg3, DWORD error)
{
	CExcBase::AddMsg(format, msg1, msg2, msg3);
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2, const std::string& msg3, const std::string& msg4, DWORD error)
{
	CExcBase::AddMsg(format, msg1, msg2, msg3, msg4);
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::wstring& msg, DWORD error)
{
	CExcBase::AddMsg(msg);
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::wstring& format, const std::wstring& msg, DWORD error)
{
	CExcBase::AddMsg(format, msg);
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2, DWORD error)
{
	CExcBase::AddMsg(format, msg1, msg2);
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2, const std::wstring& msg3, DWORD error)
{
	CExcBase::AddMsg(format, msg1, msg2, msg3);
	CExcBase::AddMsg(GetErrorMessageA(error));
}

void CWinExc::AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2, const std::wstring& msg3, const std::wstring& msg4, DWORD error)
{
	CExcBase::AddMsg(format, msg1, msg2, msg3, msg4);
	CExcBase::AddMsg(GetErrorMessageA(error));
}



}