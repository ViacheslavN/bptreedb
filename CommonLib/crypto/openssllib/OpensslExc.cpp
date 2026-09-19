#include "stdafx.h"
#include <openssl/err.h>
#include "OpensslExc.h"
#include "../../str/str.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			COpenSSLExc::COpenSSLExc(const std::string& err_msg) : CExcBase(err_msg)
			{
				m_nErrorCode = ERR_get_error();
				AddMsg(GetErrorDesc(m_nErrorCode));
			}

			COpenSSLExc::COpenSSLExc(int errorCode): m_nErrorCode(errorCode)
			{
				AddMsg(GetErrorDesc(m_nErrorCode));
			}

			COpenSSLExc::COpenSSLExc(const std::string& err_msg, int errorCode) : CExcBase(err_msg), m_nErrorCode(errorCode)
			{
				AddMsg(GetErrorDesc(m_nErrorCode));
			}

			COpenSSLExc::~COpenSSLExc() noexcept
			{

			}

			std::shared_ptr<CExcBase> COpenSSLExc::Clone() const
			{
				std::shared_ptr<CExcBase> ptrExc(new COpenSSLExc(*this));
				return ptrExc;
			}

            std::string COpenSSLExc::GetErrorDesc(int errorCode)
			{
				const char*  pszErrdescr = ERR_error_string(errorCode, 0);
				return  str_format::AStrFormatSafeT("OpenSSL error code: {0}, desc: {1}", errorCode, pszErrdescr ? pszErrdescr : "Unknown error");
			}
		}
	}
}