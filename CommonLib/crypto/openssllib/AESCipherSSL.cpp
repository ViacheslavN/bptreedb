#include "stdafx.h"
#include "AESCipherSSL.h"
#include "exception/exc_base.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			CAESCipherSSL::CAESCipherSSL(EAESKeySize keySize, bool bPadding, CipherChainMode mode) :
				CAESBase(keySize, bPadding, mode), m_evpCipher(keySize, mode)
			{

			}

			CAESCipherSSL::~CAESCipherSSL()
			{

			}

		
			void CAESCipherSSL::SetKey(const crypto_vector& keyData)
			{
				if (GetKeySize() != keyData.size())
					throw CExcBase("CAESCipherSSL: wrong key size, input {0}, expected: {1} ", keyData.size(), GetKeySize());

				m_keyData = keyData;
			}


			uint32_t CAESCipherSSL::Encrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
			{
				try
				{
					if (m_keyData.empty())
						throw CExcBase("key is null");

					EncryptThrowIfWrongtSize(bufSize, dstSize);

					if (m_enctyptCtx.get() == nullptr)
					{
						m_enctyptCtx.reset(new CEvpCipherCtx(m_bPadding));
					}
														   
					int encryptSize = 0;
					if (m_chainMode == CipherChainMode::ECB)
						m_enctyptCtx->EncryptInitEx(m_evpCipher.GetCipher(), nullptr, m_keyData.data(), nullptr);
					else
						m_enctyptCtx->EncryptInitEx(m_evpCipher.GetCipher(), nullptr, m_keyData.data(), m_IvData.data());

					m_enctyptCtx->SetPadding(m_bPadding);
					m_enctyptCtx->EncryptUpdate(dstBuf, &encryptSize, srcBuf, bufSize);

					int nResult = encryptSize;

					if (m_bPadding)
					{
						m_enctyptCtx->EncryptFinalEx(dstBuf + nResult, &encryptSize);
						nResult += encryptSize;
					}

					return nResult;
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("CAESCipherSSL: Failed to encrypt", exc);
					throw;
				}
			}

			uint32_t CAESCipherSSL::Decrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
			{
				try
				{
					if (m_keyData.empty())
						throw CExcBase("key is null");

					if (m_decryptCtx.get() == nullptr)
					{
						m_decryptCtx.reset(new CEvpCipherCtx(m_bPadding));
					}

					int decryptSize = 0;
					if (m_chainMode == CipherChainMode::ECB)
						m_decryptCtx->DecryptInitEx(m_evpCipher.GetCipher(), nullptr, m_keyData.data(), nullptr);
					else
						m_decryptCtx->DecryptInitEx(m_evpCipher.GetCipher(), nullptr, m_keyData.data(), m_IvData.data());

					m_decryptCtx->SetPadding(m_bPadding);
					m_decryptCtx->DecryptUpdate(dstBuf, &decryptSize, srcBuf, bufSize);

					uint32_t nResult = decryptSize;

					if (m_bPadding)
					{
						m_enctyptCtx->DecryptFinalEx(dstBuf + nResult, &decryptSize);
						nResult += decryptSize;
					}

					return nResult;
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("AESCipher: Failed to decrypt", exc);
					throw;
				}
			}

			uint32_t CAESCipherSSL::Encrypt(byte_t* srcBuf, uint32_t bufSize)
			{
				return Encrypt(srcBuf, bufSize, srcBuf, bufSize);
			}

			uint32_t CAESCipherSSL::Decrypt(byte_t* srcBuf, uint32_t bufSize)
			{
				return Decrypt(srcBuf, bufSize, srcBuf, bufSize);
			}


		}
	}
}