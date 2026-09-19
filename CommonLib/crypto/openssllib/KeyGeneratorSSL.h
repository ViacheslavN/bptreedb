#pragma once

#include "../crypto.h"
 
namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			class CKeyGenerator : public IKeyGenerator
			{
			public:
				CKeyGenerator();
				virtual ~CKeyGenerator();

				virtual void DeriveKeyPBKDF2(const crypto_astr& password, const crypto_vector& salt, uint32_t interations, crypto_vector& keyData, uint32_t keySize);
 
			};
		}
	}
}