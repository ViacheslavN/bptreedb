// TestCrypto.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../../CommonLib.h"
#include "../../crypto/winapi/CryptoFactory.h"
#include "../../crypto/openssllib/CryptoFactoryOpenSSL.h"
#include "../../str/str.h"


void TestXTS();

int main()
{ 
	try
	{

		TestXTS();

		return 0;

		CommonLib::crypto::ICryptoFactoryPtr ptrCryptoFactory(new CommonLib::crypto::winapi::CCryptoFactory());
		CommonLib::crypto::ICryptoFactoryPtr ptrCryptoFactorySSL(new CommonLib::crypto::openssllib::CCryptoFactoryOpenSSL());

		CommonLib::crypto::IRandomGeneratorPtr ptrRandomGenerator = ptrCryptoFactory->CreateRandomGenerator();
		CommonLib::crypto::IKeyGeneratorPtr ptrKeyGenerator = ptrCryptoFactory->CreateKeyGenerator();
		CommonLib::crypto::IKeyGeneratorPtr ptrKeyGeneratorSSL = ptrCryptoFactorySSL->CreateKeyGenerator();

		CommonLib::crypto::IAESCipherPtr ptrAesCipher= ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
		CommonLib::crypto::IAESCipherPtr ptrAesCipherWithPadding = ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, true, CommonLib::crypto::CipherChainMode::ECB);
	
		CommonLib::crypto::IAESCipherPtr ptrAesCipherSSL = ptrCryptoFactorySSL->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
		CommonLib::crypto::IAESCipherPtr ptrAesCipherWithPaddingSSL = ptrCryptoFactorySSL->CreateAESCipher(CommonLib::crypto::AES_256, true, CommonLib::crypto::CipherChainMode::ECB);



		uint32_t blockSize = ptrAesCipher->GetBlockSize();
		uint32_t keySize = ptrAesCipher->GetKeySize();

		CommonLib::crypto::crypto_astr pwd = "123456789";
		CommonLib::crypto::crypto_vector pwdSalt(64, 0);
		CommonLib::crypto::crypto_vector keyData;
		CommonLib::crypto::crypto_vector keyData1;

		ptrRandomGenerator->GenRandom(pwdSalt);

		ptrRandomGenerator->GenRandom(ptrAesCipher->GetIVData(), ptrAesCipher->GetIVSize());

		memcpy(ptrAesCipherSSL->GetIVData(), ptrAesCipher->GetIVData(), ptrAesCipher->GetIVSize());
		

		ptrKeyGenerator->DeriveKeyPBKDF2(pwd, pwdSalt,  2048, keyData, keySize);
		ptrKeyGeneratorSSL->DeriveKeyPBKDF2(pwd, pwdSalt, 2048, keyData1, keySize);

		for (uint32_t i = 0; i < keySize; ++i)
		{
			if (keyData[i] != keyData1[i])
			{
				std::cout << "wrong PBKDF2\n";
				break;
			}
		}

 
		ptrAesCipher->SetKey(keyData);
		ptrAesCipherSSL->SetKey(keyData);

		std::string plainText;
		plainText.resize(16, 'a');
		std::string plainTextSSL;
		plainTextSSL.resize(16, 'a');

		int32_t size =  ptrAesCipher->GetBufferSize(plainText.length());
		std::vector<byte_t> butChiperText(size, 0);
		ptrAesCipher->Encrypt((const byte_t*)plainText.c_str(), plainText.size(), (byte_t*)plainText.data(), plainText.size());
		ptrAesCipherSSL->Encrypt((const byte_t*)plainTextSSL.c_str(), plainTextSSL.size(), (byte_t*)plainTextSSL.data(), plainTextSSL.size());

		int32_t size1 = ptrAesCipher->GetBufferSize(butChiperText.size());
		std::vector<byte_t> plainText1;
		std::vector<byte_t> plainText2;
		plainText1.resize(size1);
		plainText2.resize(size1);
		ptrAesCipher->Decrypt((const byte_t*)plainTextSSL.data(), plainTextSSL.size(), (byte_t*)plainTextSSL.data(), plainTextSSL.size());
		ptrAesCipherSSL->Decrypt((const byte_t*)plainText.data(), plainText.size(), (byte_t*)plainText.data(), plainText.size());
	
	}
	catch (std::exception& exc)
	{
		std::cout << "Error: " << exc.what();
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
