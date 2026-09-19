#pragma once
 
#include <openssl/evp.h>

#include "../crypto.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			class CEvpCipherCtx
			{
			public:
				CEvpCipherCtx(bool padding);
				~CEvpCipherCtx();

				void EncryptInitEx(const EVP_CIPHER *type, ENGINE *impl, const byte_t *key, const byte_t *iv);
				void DecryptInitEx(const EVP_CIPHER *type, ENGINE *impl, const byte_t *key, const byte_t *iv);
				void EncryptUpdate(byte_t *out, int *outl, const byte_t *in, int inl);
				void DecryptUpdate(byte_t *out, int *outl, const byte_t *in, int inl);
				void EncryptFinalEx(byte_t *out, int *outl);
				void DecryptFinalEx(byte_t *out, int *outl);
				void SetPadding(bool padding);

			private:
				EVP_CIPHER_CTX* m_pCipherCtx;
			};

			typedef std::shared_ptr<CEvpCipherCtx > CEvpCipherCtxPtr;
		}
	}
}