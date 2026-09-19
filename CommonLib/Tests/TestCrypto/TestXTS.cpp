#include <iostream>
#include "../../CommonLib.h"
#include "../../crypto/winapi/CryptoFactory.h"
#include "../../crypto/openssllib/CryptoFactoryOpenSSL.h"
#include "../../str/str.h"
#include "../../crypto/XTSBlockChiper.h"

void TestXTS()
{
	CommonLib::crypto::ICryptoFactoryPtr ptrCryptoFactory(new CommonLib::crypto::winapi::CCryptoFactory());
	CommonLib::crypto::ICryptoFactoryPtr ptrCryptoFactorySSL(new CommonLib::crypto::openssllib::CCryptoFactoryOpenSSL());

	CommonLib::crypto::IRandomGeneratorPtr ptrRandomGenerator = ptrCryptoFactory->CreateRandomGenerator();
	CommonLib::crypto::IKeyGeneratorPtr ptrKeyGenerator = ptrCryptoFactory->CreateKeyGenerator();
	CommonLib::crypto::IAESCipherPtr ptrAesDataCipher = ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
	CommonLib::crypto::IAESCipherPtr ptrAesTweakCipher = ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
	
	CommonLib::crypto::IAESCipherPtr ptrAesDataCipherSSL = ptrCryptoFactorySSL->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
	CommonLib::crypto::IAESCipherPtr ptrAesTweakCipherSSL = ptrCryptoFactorySSL->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
	
	uint32_t blockSize = ptrAesDataCipher->GetBlockSize();
	uint32_t keySize = ptrAesDataCipher->GetKeySize();

	CommonLib::crypto::crypto_astr pwd = "123456789";
	CommonLib::crypto::crypto_vector pwdSalt(64, 0);
	CommonLib::crypto::crypto_vector tweakSalt(64, 0);
	CommonLib::crypto::crypto_vector keyData;
	CommonLib::crypto::crypto_vector keyTweakData;

	ptrRandomGenerator->GenRandom(pwdSalt);
	ptrRandomGenerator->GenRandom(tweakSalt);

	ptrKeyGenerator->DeriveKeyPBKDF2(pwd, pwdSalt, 2048, keyData, keySize);
	ptrKeyGenerator->DeriveKeyPBKDF2(pwd, tweakSalt, 2048, keyTweakData, keySize);

	ptrAesDataCipher->SetKey(keyData);
	ptrAesTweakCipher->SetKey(keyTweakData);

	ptrAesDataCipherSSL->SetKey(keyData);
	ptrAesTweakCipherSSL->SetKey(keyTweakData);

	CommonLib::crypto::CXTSBlockChiperPtr ptrXTSBlockChipher(new CommonLib::crypto::CXTSBlockChiper(ptrAesDataCipher, ptrAesTweakCipher));
	CommonLib::crypto::CXTSBlockChiperPtr ptrXTSBlockChipherSSL(new CommonLib::crypto::CXTSBlockChiper(ptrAesDataCipherSSL, ptrAesTweakCipherSSL));

	for (uint32_t i = 0; i < 100; ++i)
	{
		std::vector<byte_t> page(8192, '0' + i % 10);
		std::vector<byte_t> copy_page = page;

		if ((i % 2) == 0)
		{
			ptrXTSBlockChipher->Encrypt(i, page.data(), page.size());
			ptrXTSBlockChipherSSL->Decrypt(i, page.data(), page.size());
		}
		else
		{
			ptrXTSBlockChipherSSL->Encrypt(i, page.data(), page.size());
			ptrXTSBlockChipher->Decrypt(i, page.data(), page.size());
		}


		for (size_t j = 0; j < copy_page.size(); ++j)
		{
			if (copy_page[j] != page[j])
			{
				std::cout << "Wrong data \n";
				break;
			}
		}

	}







}