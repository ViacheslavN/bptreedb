#pragma once

#include "EnvCipher.h"
#include "EvpCipherCtx.h"
#include "../AESBase.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			class CAESCipherSSL : public CAESBase
			{
			public:
				CAESCipherSSL(EAESKeySize keySize, bool bPadding, CipherChainMode mode);
				virtual ~CAESCipherSSL();


				virtual void SetKey(const crypto_vector& keyData);


				virtual uint32_t Encrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
				virtual uint32_t Decrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
				virtual uint32_t Encrypt(byte_t* srcBuf, uint32_t bufSize);
				virtual uint32_t Decrypt(byte_t* srcBuf, uint32_t bufSize);
			private:
				CEvpCipher m_evpCipher;
				CEvpCipherCtxPtr m_enctyptCtx;
				CEvpCipherCtxPtr m_decryptCtx;
				crypto_vector m_keyData;
			};
		}
	}
}