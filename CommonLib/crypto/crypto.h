#pragma once
#include <vector>
#include "CryptoData.h"
#include "IDataCipher.h"

namespace CommonLib
{
	namespace crypto
	{

		enum EKeyType
		{
			SymmetricKey,
			AsymmetricKey
		};

		enum EAESKeySize
		{
			AES_128,
			AES_192,
			AES_256
		};

		typedef std::shared_ptr<class IKey> IKeyPtr;
		typedef std::shared_ptr<class IRandomGenerator> IRandomGeneratorPtr;
		typedef std::shared_ptr<class IKeyGenerator> IKeyGeneratorPtr; 
		typedef std::shared_ptr<class IAESCipher> IAESCipherPtr;
		typedef std::shared_ptr<class ISymmetricKey> ISymmetricKeyPtr;
		typedef std::shared_ptr<class ICryptoFactory> ICryptoFactoryPtr;

		enum class CipherChainMode
		{
			ECB,
			CBC,
			CCM,
			GCM
		};

		class ICryptoFactory
		{
		public:
			ICryptoFactory() {}
			virtual ~ICryptoFactory() {}

			virtual IRandomGeneratorPtr CreateRandomGenerator() const = 0;
			virtual IKeyGeneratorPtr CreateKeyGenerator() const = 0;
			virtual IAESCipherPtr CreateAESCipher(EAESKeySize keySize, bool bPadding, CipherChainMode mode) const = 0;
		};
	 

		class IRandomGenerator
		{
		public:
			IRandomGenerator(){}
			virtual ~IRandomGenerator(){}

			virtual void GenRandom(crypto_vector& random) = 0;
			virtual void GenRandom(byte_t* pData, uint32_t size) = 0;
		};

		class IKeyGenerator
		{
		public:
			virtual void DeriveKeyPBKDF2(const crypto_astr& password, const crypto_vector& salt, uint32_t interations, crypto_vector& keyData, uint32_t keySize) = 0;

		};

		

		class IAESCipher : public IDataCipher
		{
		public:
			IAESCipher(){}
			virtual ~IAESCipher(){}

			virtual uint32_t GetKeySize() const = 0;
			virtual uint32_t GetBlockSize() const = 0;
			virtual uint32_t GetIVSize() const = 0;
			virtual byte_t* GetIVData() = 0;
			virtual const byte_t* GetIVData() const = 0;
			virtual void SetKey(const crypto_vector& keyData) = 0;
 
		};
	}
}