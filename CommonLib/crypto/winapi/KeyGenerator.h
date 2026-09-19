#pragma once

#include "../crypto.h"
#include "CryptoAlgoritmProvider.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			class CKeyGenerator : public IKeyGenerator
			{
			public:
				CKeyGenerator();
				virtual ~CKeyGenerator();

				virtual void DeriveKeyPBKDF2(const crypto_astr& password, const crypto_vector& salt, uint32_t interations, crypto_vector& keyData, uint32_t keySize);
	
			private:
				CBcryptAlgProviderPtr m_ptrAlgProvider;
			};
		}
	}
}