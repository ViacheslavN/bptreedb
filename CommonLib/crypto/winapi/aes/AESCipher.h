#pragma once
#include "../CryptoAlgoritmProvider.h"
#include "../SymmetricKey.h"
#include "../../AESBase.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			class CAESCipher : public CAESBase
			{
			public:
				CAESCipher(EAESKeySize keySize, bool bPadding, CipherChainMode mode);
				virtual ~CAESCipher();


				virtual void SetKey(const crypto_vector& keyData);
				virtual uint32_t Encrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
				virtual uint32_t Decrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
				virtual uint32_t Encrypt(byte_t* srcBuf, uint32_t bufSize);
				virtual uint32_t Decrypt(byte_t* srcBuf, uint32_t bufSize);
			private:
				void SetChainMode(CipherChainMode mode);
			private:
				CBcryptAlgProviderPtr m_ptrAlgProvider;
				CSymmetricKeyPtr m_ptrKey;
				uint32_t m_nObjectSize;
			};
		}
	}
}