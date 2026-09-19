#pragma once

#include "../../crypto.h"
#include "../CryptoAlgoritmProvider.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			class CRandomGenerator : public IRandomGenerator
			{
			public:
				CRandomGenerator();
				virtual ~CRandomGenerator();

				virtual void GenRandom(crypto_vector& random);
				virtual void GenRandom(byte_t* pData, uint32_t size);
			private:
				CBcryptAlgProviderPtr m_ptrBcryptAlgProvider;
			};
		}
	}
}