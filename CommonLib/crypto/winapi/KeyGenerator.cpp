#include "stdafx.h"
#include "KeyGenerator.h"


namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			CKeyGenerator::CKeyGenerator()
			{
				try
				{
					m_ptrAlgProvider = CBcryptAlgProvider::Create(BCRYPT_SHA512_ALGORITHM, BCRYPT_ALG_HANDLE_HMAC_FLAG);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("CKeyGenerator: Failed to create", exc);
				}
			}

			CKeyGenerator::~CKeyGenerator()
			{
			}

			void CKeyGenerator::DeriveKeyPBKDF2(const crypto_astr& password, const crypto_vector& salt, uint32_t interations, crypto_vector& keyData, uint32_t keySize)
			{
				try
				{
					m_ptrAlgProvider->CryptDeriveKeyPBKDF2(password, salt, interations, keySize, keyData);
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