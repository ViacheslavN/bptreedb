#include "stdafx.h"
#include <openssl/rand.h>
#include "RandomGeneratorSSL.h"
#include "OpensslExc.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			CRandomGeneratorSSL::CRandomGeneratorSSL()
			{			
			}

			CRandomGeneratorSSL::~CRandomGeneratorSSL()
			{

			}

			void CRandomGeneratorSSL::GenRandom(crypto_vector& random)
			{
				GenRandom(random.data(), (uint32_t)random.size());
			}

			void CRandomGeneratorSSL::GenRandom(byte_t* pData, uint32_t size)
			{
				try
				{
					int rc = RAND_bytes(pData, size);
					if (rc == 0)
						throw COpenSSLExc("Failed to run RAND_bytes");
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("RandomGenerator: Failed to generate random data", exc);
				}
			}
		}
	}
}