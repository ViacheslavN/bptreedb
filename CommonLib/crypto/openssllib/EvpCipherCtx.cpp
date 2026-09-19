#include "stdafx.h"
#include "EvpCipherCtx.h"
#include "OpensslExc.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			CEvpCipherCtx::CEvpCipherCtx(bool padding)
			{
				m_pCipherCtx = EVP_CIPHER_CTX_new();
				if (m_pCipherCtx == nullptr)
					throw COpenSSLExc("EvpCipherCtx: Failed to create");

				EVP_CIPHER_CTX_init(m_pCipherCtx);
			}

			CEvpCipherCtx::~CEvpCipherCtx()
			{
				EVP_CIPHER_CTX_free(m_pCipherCtx);
			}

			void CEvpCipherCtx::EncryptInitEx(const EVP_CIPHER *type, ENGINE *impl, const byte_t *key, const byte_t *iv)
			{
				int ret = EVP_EncryptInit_ex(m_pCipherCtx, type, impl, key, iv);
				if(ret == 0)
					throw COpenSSLExc("CEvpCipherCtx: failed to init");
			}

			void CEvpCipherCtx::DecryptInitEx(const EVP_CIPHER *type, ENGINE *impl, const byte_t *key, const byte_t *iv)
			{
				int ret = EVP_DecryptInit_ex(m_pCipherCtx, type, impl, key, iv);
				if (ret == 0)
					throw COpenSSLExc("CEvpCipherCtx: failed to init");
			}

			void CEvpCipherCtx::EncryptUpdate(byte_t *out, int *outl, const byte_t *in, int inl)
			{
				int ret = EVP_EncryptUpdate(m_pCipherCtx, out, outl, in, inl);
				if (ret == 0)
					throw COpenSSLExc("CEvpCipherCtx: failed to encrypt update");
			}

			void CEvpCipherCtx::DecryptUpdate(byte_t *out, int *outl, const byte_t *in, int inl)
			{
				int ret = EVP_DecryptUpdate(m_pCipherCtx, out, outl, in, inl);
				if (ret == 0)
					throw COpenSSLExc("CEvpCipherCtx: failed to decrypt update");
			}

			void CEvpCipherCtx::EncryptFinalEx(byte_t *out, int *outl)
			{
				int ret = EVP_EncryptFinal_ex(m_pCipherCtx, out, outl);
				if (ret == 0)
					throw COpenSSLExc("CEvpCipherCtx: failed to encrypt final");
			}

			void CEvpCipherCtx::DecryptFinalEx(byte_t *out, int *outl)
			{
				int ret = EVP_DecryptFinal_ex(m_pCipherCtx, out, outl);
				if (ret == 0)
					throw COpenSSLExc("CEvpCipherCtx: failed to decrypt final");
			}

			void CEvpCipherCtx::SetPadding(bool padding)
			{
				int ret = EVP_CIPHER_CTX_set_padding(m_pCipherCtx, padding ? 1 : 0);
				if (ret == 0)
				{
					EVP_CIPHER_CTX_free(m_pCipherCtx);
					throw COpenSSLExc("CEvpCipherCtx: failed to set padding");
				}
			}
		}
	}
}