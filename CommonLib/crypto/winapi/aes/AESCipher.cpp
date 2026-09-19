#include "stdafx.h"
#include "AESCipher.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			CAESCipher::CAESCipher(EAESKeySize keySize, bool bPadding, CipherChainMode mode) : CAESBase(keySize, bPadding, mode)
			{
				try
				{
					m_ptrAlgProvider = CBcryptAlgProvider::Create(BCRYPT_AES_ALGORITHM, 0);
					ULONG resultLength = 0;
					//m_ptrAlgProvider->CryptGetProperty(BCRYPT_BLOCK_LENGTH, (PBYTE)&m_nBlockSize, sizeof(m_nBlockSize),  &resultLength, 0);
					//if (m_nBlockSize == 0)
					//	throw CExcBase("Block size is null");

					m_ptrAlgProvider->CryptGetProperty(BCRYPT_OBJECT_LENGTH, (PBYTE)&m_nObjectSize, sizeof(m_nObjectSize), &resultLength, 0);
					SetChainMode(m_chainMode);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("AESCipher: Failed to create provider ", exc);
					throw;
				}
			}

			void CAESCipher::SetChainMode(CipherChainMode mode)
			{
				switch (mode)
				{
				case CommonLib::crypto::CipherChainMode::ECB:
					m_ptrAlgProvider->CryptSetProperty(BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_ECB, sizeof(BCRYPT_CHAIN_MODE_ECB), 0);
					break;
				case CommonLib::crypto::CipherChainMode::CBC:
					m_ptrAlgProvider->CryptSetProperty(BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
					break;
				case CommonLib::crypto::CipherChainMode::CCM:
					m_ptrAlgProvider->CryptSetProperty(BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CCM, sizeof(BCRYPT_CHAIN_MODE_CCM), 0);
					break;
				case CommonLib::crypto::CipherChainMode::GCM:
					m_ptrAlgProvider->CryptSetProperty(BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_GCM, sizeof(BCRYPT_CHAIN_MODE_GCM), 0);
					break;
				default:
					break;
				}
			}

			CAESCipher::~CAESCipher()
			{

			}

		
			void CAESCipher::SetKey(const crypto_vector& keyData)
			{
				try
				{
					if (keyData.size() != GetKeySize())
						throw CExcBase("AESCipher: wrong size {0}, expected ", keyData.size(), GetKeySize());

					PBYTE pbKeyObject = (PBYTE)::HeapAlloc(::GetProcessHeap(), 0, m_nObjectSize);
				
					BCRYPT_KEY_HANDLE keyHandle = m_ptrAlgProvider->CryptGenerateSymmetricKey(pbKeyObject, m_nObjectSize, (PUCHAR)keyData.data(), (ULONG)keyData.size(), 0);
					//BCryptGenerateSymmetricKey

					return m_ptrKey.reset(new CSymmetricKey(keyHandle, pbKeyObject, m_nObjectSize));
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("AESCipher: Failed to create key ", exc);
					throw;
				}
			}
			
			uint32_t CAESCipher::Encrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
			{
				try
				{
					if (m_ptrKey.get() == nullptr)
						throw CExcBase("AESCipher: key is null");

					ULONG nResult = 0;
					if(m_chainMode == CipherChainMode::ECB)
							m_ptrKey->CryptEncrypt((PUCHAR)srcBuf, bufSize, nullptr, nullptr, 0, dstBuf, dstSize, &nResult,	m_bPadding ? BCRYPT_BLOCK_PADDING : 0);
					else
						m_ptrKey->CryptEncrypt((PUCHAR)srcBuf, bufSize, nullptr, (PUCHAR)m_IvData.data(), (ULONG)m_IvData.size(), dstBuf, dstSize, &nResult, m_bPadding ? BCRYPT_BLOCK_PADDING : 0);
					
					return nResult;
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("AESCipher: Failed to encrypt", exc);
					throw;
				}
			}

			uint32_t CAESCipher::Decrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
			{
				try
				{
					if (m_ptrKey.get() == nullptr)
						throw CExcBase("AESCipher: key is null");

					ULONG nResult = 0;
					if (m_chainMode == CipherChainMode::ECB)
						m_ptrKey->CryptDecrypt((PUCHAR)srcBuf, bufSize, nullptr, nullptr, 0, dstBuf, dstSize, &nResult,	m_bPadding ? BCRYPT_BLOCK_PADDING : 0);
					else
						m_ptrKey->CryptDecrypt((PUCHAR)srcBuf, bufSize, nullptr, (PUCHAR)m_IvData.data(), (ULONG)m_IvData.size(),  dstBuf, dstSize, &nResult, m_bPadding ? BCRYPT_BLOCK_PADDING : 0);

					return nResult;
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("AESCipher: Failed to decrypt", exc);
					throw;
				}
			}

			uint32_t CAESCipher::Encrypt(byte_t* srcBuf, uint32_t bufSize)
			{
				return Encrypt(srcBuf, bufSize, srcBuf, bufSize);
			}

			uint32_t CAESCipher::Decrypt(byte_t* srcBuf, uint32_t bufSize)
			{
				return Decrypt(srcBuf, bufSize, srcBuf, bufSize);
			}

		}
	}
}