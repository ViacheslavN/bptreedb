#include "stdafx.h"
#include "CryptoFactoryOpenSSL.h"
#include "KeyGeneratorSSL.h"
#include "OpenSSlLib.h"
#include "AESCipherSSL.h"
#include "RandomGeneratorSSL.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			CCryptoFactoryOpenSSL::CCryptoFactoryOpenSSL()
			{
			}
			
			CCryptoFactoryOpenSSL::~CCryptoFactoryOpenSSL()
			{

			}

			IRandomGeneratorPtr CCryptoFactoryOpenSSL::CreateRandomGenerator() const
			{
				return IRandomGeneratorPtr(new CRandomGeneratorSSL());
			}

			IKeyGeneratorPtr CCryptoFactoryOpenSSL::CreateKeyGenerator() const
			{
				return IKeyGeneratorPtr(new CKeyGenerator());
			}
			 
			IAESCipherPtr CCryptoFactoryOpenSSL::CreateAESCipher(EAESKeySize keySize, bool bPadding, CipherChainMode mode) const
			{
				return IAESCipherPtr(new CAESCipherSSL(keySize, bPadding, mode));
			}

		}
	}
}