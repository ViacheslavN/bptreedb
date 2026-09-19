#include "stdafx.h"
#include "RandomGenerator.h"
#include "../../../exception/exc_base.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			CRandomGenerator::CRandomGenerator()
			{
				try
				{
					m_ptrBcryptAlgProvider = CBcryptAlgProvider::Create(BCRYPT_RNG_ALGORITHM, 0);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("RandomGenerator: Failed to create", exc);
				}
			}

			CRandomGenerator::~CRandomGenerator()
			{

			}

			void CRandomGenerator::GenRandom(crypto_vector& random)
			{
				try
				{
					GenRandom(random.data(), (uint32_t)random.size());
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("RandomGenerator: Failed to generate random data", exc);
				}
			}
			
			void CRandomGenerator::GenRandom(byte_t* pData, uint32_t size)
			{
				try
				{
					m_ptrBcryptAlgProvider->CryptGenRandom(pData, size, BCRYPT_RNG_USE_ENTROPY_IN_BUFFER);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("RandomGenerator: Failed to generate random data", exc);
				}
			}
		}
	}
}