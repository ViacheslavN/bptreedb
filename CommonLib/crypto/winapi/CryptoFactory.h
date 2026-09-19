#pragma once

#include "../crypto.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			class CCryptoFactory : public ICryptoFactory
			{
			public:
				CCryptoFactory();
				virtual ~CCryptoFactory();

				virtual IRandomGeneratorPtr CreateRandomGenerator() const;
				virtual IKeyGeneratorPtr CreateKeyGenerator() const;
				virtual IAESCipherPtr CreateAESCipher(EAESKeySize keySize, bool bPadding, CipherChainMode mode) const;
			};
		}
	}
}