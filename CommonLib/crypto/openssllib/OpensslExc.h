#pragma once
#include "../../exception/exc_base.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			class COpenSSLExc : public CExcBase
			{
			public:
				COpenSSLExc(const std::string& err_msg);
				COpenSSLExc(int errorCode);
				COpenSSLExc(const std::string& err_msg, int errorCode);

				template<typename... Types>
				COpenSSLExc(int errorCode, const std::string& format, Types... args) : CExcBase(format, args...), m_nErrorCode(errorCode)
				{
					CExcBase::AddMsg(GetErrorDesc(m_nErrorCode));
				}

				virtual ~COpenSSLExc() noexcept;
				virtual std::shared_ptr<CExcBase> Clone() const;

				static std::string GetErrorDesc(int errorCode);
			private:

			private:
				int m_nErrorCode;
			};
		}
	}
}