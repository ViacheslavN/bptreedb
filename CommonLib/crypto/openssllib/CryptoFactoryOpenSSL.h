#pragma once
#include "../crypto.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			class CCryptoFactoryOpenSSL : public ICryptoFactory
			{
			public:
				CCryptoFactoryOpenSSL();
				virtual ~CCryptoFactoryOpenSSL();

				virtual IRandomGeneratorPtr CreateRandomGenerator() const;
				virtual IKeyGeneratorPtr CreateKeyGenerator() const;
				virtual IAESCipherPtr CreateAESCipher(EAESKeySize keySize, bool bPadding, CipherChainMode mode)const ;
			};
		}
	}
}