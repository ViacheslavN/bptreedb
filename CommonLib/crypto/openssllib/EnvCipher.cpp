#include "stdafx.h"
#include "EnvCipher.h"
#include "OpensslExc.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{

			CEvpCipher::CEvpCipher(EAESKeySize keySize, CipherChainMode mode)
			{
				switch (mode)
				{
				case CommonLib::crypto::CipherChainMode::ECB:
					m_pCipher = GetEVPcipherECB(keySize);
					break;
				case CommonLib::crypto::CipherChainMode::CBC:
					m_pCipher = GetEVPcipherCBC(keySize);
					break;
				case CommonLib::crypto::CipherChainMode::CCM:
					m_pCipher = GetEVPcipherCCM(keySize);
					break;
				case CommonLib::crypto::CipherChainMode::GCM:
					m_pCipher = GetEVPcipherGCM(keySize);
					break;
				default:
					throw CExcBase("EvpCipher unknown chain mode: {0}", (int)mode);

				}

				if (m_pCipher == nullptr)
					throw COpenSSLExc("EvpCipher: Failed to create");

			}

			CEvpCipher::~CEvpCipher()
			{

			}

 
			const EVP_CIPHER* CEvpCipher::GetCipher() const
			{
				return m_pCipher;
			}

			const EVP_CIPHER* CEvpCipher::GetEVPcipherECB(EAESKeySize keySize) const
			{
				switch (keySize)
				{
				case CommonLib::crypto::AES_128:
					return EVP_aes_128_ecb();
				case CommonLib::crypto::AES_192:
					return EVP_aes_192_ecb();
				case CommonLib::crypto::AES_256:
					return EVP_aes_256_ecb();
				default:
					throw CExcBase("EvpCipher, unknown key size {0}", keySize);
				}
			}

			const EVP_CIPHER* CEvpCipher::GetEVPcipherCBC(EAESKeySize keySize) const
			{
				switch (keySize)
				{
				case CommonLib::crypto::AES_128:
					return EVP_aes_128_cbc();
				case CommonLib::crypto::AES_192:
					return EVP_aes_192_cbc();
				case CommonLib::crypto::AES_256:
					return EVP_aes_256_cbc();
				default:
					throw CExcBase("EvpCipher, unknown key size {0}", keySize);
				}
			}

			const EVP_CIPHER* CEvpCipher::GetEVPcipherCCM(EAESKeySize keySize) const
			{
				switch (keySize)
				{
				case CommonLib::crypto::AES_128:
					return EVP_aes_128_ccm();
				case CommonLib::crypto::AES_192:
					return EVP_aes_192_ccm();
				case CommonLib::crypto::AES_256:
					return EVP_aes_256_ccm();
				default:
					throw CExcBase("EvpCipher, unknown key size {0}", keySize);
				}
			}

			const EVP_CIPHER* CEvpCipher::GetEVPcipherGCM(EAESKeySize keySize) const
			{
				switch (keySize)
				{
				case CommonLib::crypto::AES_128:
					return EVP_aes_128_gcm();
				case CommonLib::crypto::AES_192:
					return EVP_aes_192_gcm();
				case CommonLib::crypto::AES_256:
					return EVP_aes_256_gcm();
				default:
					throw CExcBase("EvpCipher, unknown key size {0}", keySize);
				}
			}
		}
	}
}