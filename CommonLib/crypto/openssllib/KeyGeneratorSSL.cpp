#include "stdafx.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "KeyGeneratorSSL.h"
#include "exception/exc_base.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			CKeyGenerator::CKeyGenerator()
			{

			}

			CKeyGenerator::~CKeyGenerator()
			{

			}

			void CKeyGenerator::DeriveKeyPBKDF2(const crypto_astr& password, const crypto_vector& salt, uint32_t interations, crypto_vector& keyData, uint32_t keySize)
			{
				try
				{
					keyData.resize(keySize);
		 			PKCS5_PBKDF2_HMAC(password.c_str(), (int)password.size(), salt.data(), (int)salt.size(), interations, EVP_sha512(), keySize, keyData.data());
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExc("KeyGenerator: Failed to create key", exc);
					throw;
				}
			}
		}
	}
}