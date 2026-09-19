#include "PosixExc.h"

namespace CommonLib
{
	CPosixExc::CPosixExc(int errcode) : CExcBase(), m_errCode(errcode)
	{
		CExcBase::AddMsg(GetErrorMessageA(m_errCode));
	}

	CPosixExc::CPosixExc(const std::string& msg, int errcode) : CExcBase(msg), m_errCode(errcode)
	{
		CExcBase::AddMsg(GetErrorMessageA(m_errCode));
	}

	CPosixExc::~CPosixExc() noexcept
	{

	}

	std::shared_ptr<CExcBase> CPosixExc::Clone() const
	{
		std::shared_ptr<CPosixExc> ptrExc(new CPosixExc(*this));
		return ptrExc;
	}

	std::string CPosixExc::GetErrorMessageA(int errCode)
	{
		std::vector<char> err_buf(256, 0);
#ifdef _WIN32
		if (strerror_s(&err_buf[0], err_buf.size(), errCode) == 0)
			return std::string(&err_buf[0]);
#else
		char* err_descr = strerror_r(errCode, &err_buf[0], err_buf.size());
		if (err_descr != 0)
			return std::string(err_descr);
#endif

		return str_format::AStrFormatSafeT("POSIX Error: {0}", errCode);

	}

}