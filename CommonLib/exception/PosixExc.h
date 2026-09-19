#pragma once

#include "exc_base.h"

namespace CommonLib
{
	class CPosixExc : public CExcBase
	{
	public:
		CPosixExc(int errcode);
		CPosixExc(const astr& err_msg, int err_code);


		template<typename... Types>
		CPosixExc(int err_code, const std::string& format, Types&&... args) : CExcBase(format, std::forward<Types>(args)...), m_errCode(err_code)
		{
			CExcBase::AddMsg(GetErrorMessageA(m_errCode));
		}

		~CPosixExc() noexcept;
		virtual std::shared_ptr<CExcBase> Clone() const;

		static astr GetErrorMessageA(int errCode);
	private:
	
	private:
		int m_errCode;
		
	};
}