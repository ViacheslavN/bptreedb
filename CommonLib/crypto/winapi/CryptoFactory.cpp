#include "stdafx.h"
#include "CryptoFactory.h"
#include "random/RandomGenerator.h"
#include "KeyGenerator.h"
#include "aes/AESCipher.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			CCryptoFactory::CCryptoFactory() 
			{

			}
			
			CCryptoFactory::~CCryptoFactory()
			{

			}

			IRandomGeneratorPtr CCryptoFactory::CreateRandomGenerator() const
			{
				return IRandomGeneratorPtr(new CRandomGenerator());
			}

			IKeyGeneratorPtr CCryptoFactory::CreateKeyGenerator() const
			{
				return IKeyGeneratorPtr(new CKeyGenerator());
			}

			IAESCipherPtr CCryptoFactory::CreateAESCipher(EAESKeySize keySize, bool bPadding, CipherChainMode mode) const
			{ 
				return IAESCipherPtr(new CAESCipher(keySize, bPadding, mode));
			}
		}
	}
}