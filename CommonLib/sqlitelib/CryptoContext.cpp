#include "stdafx.h"
#include "CryptoContext.h"
 #include "../stream/MemoryStream.h"
#include "../stream/FixedMemoryStream.h"
#include "../utils/CRC.h"

namespace CommonLib
{
	namespace database
	{

		CCryptoContext::CCryptoContext(const crypto::crypto_astr& password, EKeyCryptoType cryptoType, crypto::ICryptoFactoryPtr ptrCryptoFactory) :
			m_password(password), m_cryptoType(cryptoType), m_ptrCryptoFactory(ptrCryptoFactory)
		{

		}

		CCryptoContext::CCryptoContext(const crypto::crypto_astr& password, crypto::ICryptoFactoryPtr ptrCryptoFactory) :
			m_password(password), m_cryptoType(Unknown), m_ptrCryptoFactory(ptrCryptoFactory)
		{

		}

		CCryptoContext::~CCryptoContext()
		{

		}

		crypto::IXTSDataCipherPtr CCryptoContext::GetXTSDataCipher()
		{
			return m_ptrXTSBlockChiper;
		}

		size_t CCryptoContext::GetInitBlockSize() const
		{
			return INIT_BLOCK_SIZE;
		}

		void CCryptoContext::CreateInitBlock(byte_t *pBuf, size_t size)
		{
			try
			{
				CommonLib::crypto::IRandomGeneratorPtr ptrRandomGenerator = m_ptrCryptoFactory->CreateRandomGenerator();
				CommonLib::crypto::IKeyGeneratorPtr ptrKeyGenerator = m_ptrCryptoFactory->CreateKeyGenerator();
				CommonLib::crypto::IAESCipherPtr ptrAesPwdCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::CBC);
				uint32_t blockSize = ptrAesPwdCipher->GetBlockSize();
				uint32_t keySize = ptrAesPwdCipher->GetKeySize();
				CommonLib::crypto::crypto_vector pwdKeyData(keySize, 0);
 
				FillPageWithRandomData(ptrRandomGenerator, ptrKeyGenerator, pBuf, size);

				CommonLib::CFxMemoryWriteStream writeTotalBuf;
				writeTotalBuf.AttachBuffer(pBuf, (uint32_t)size);
		
				CommonLib::crypto::crypto_vector pwdSalt(SALT_SZIE, 0);

				ptrRandomGenerator->GenRandom(pwdSalt.data(), (uint32_t)pwdSalt.size());
				ptrRandomGenerator->GenRandom(ptrAesPwdCipher->GetIVData(), ptrAesPwdCipher->GetIVSize());
 
				ptrKeyGenerator->DeriveKeyPBKDF2(m_password, pwdSalt, PWD_KEY_ROUNDS, pwdKeyData, keySize);
				ptrAesPwdCipher->SetKey(pwdKeyData);
		 

				writeTotalBuf.Write(pwdSalt.data(), pwdSalt.size());
				writeTotalBuf.Write(ptrAesPwdCipher->GetIVData(), ptrAesPwdCipher->GetIVSize());

				size_t checkSumPos = writeTotalBuf.Pos();
				writeTotalBuf.Write(uint32_t(0));


				CommonLib::CFxMemoryWriteStream cryptoBuf;
				cryptoBuf.AttachBuffer(writeTotalBuf.Buffer() + writeTotalBuf.Pos(), CRYPTO_BLOCK_SZIE);

				cryptoBuf.Write(uint32_t(m_cryptoType));

				if (m_cryptoType == MasterKey)
				{				
					CommonLib::crypto::IAESCipherPtr ptrAesDataCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
					CommonLib::crypto::IAESCipherPtr ptrAesTweakCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
					CommonLib::crypto::crypto_vector keyData(keySize, 0);
					CommonLib::crypto::crypto_vector keyTweakData(keySize, 0);					
					ptrRandomGenerator->GenRandom(keyData); 
					ptrRandomGenerator->GenRandom(keyTweakData);

					cryptoBuf.Write(keyData.data(), keyData.size());
					cryptoBuf.Write(keyTweakData.data(), keyTweakData.size());

					ptrAesDataCipher->SetKey(keyData);
					ptrAesTweakCipher->SetKey(keyTweakData);

					m_ptrXTSBlockChiper.reset(new crypto::CXTSBlockChiper(ptrAesDataCipher, ptrAesTweakCipher));				
				}
				else
				{
					CommonLib::crypto::IAESCipherPtr ptrAesDataCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
					CommonLib::crypto::IAESCipherPtr ptrAesTweakCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
					CommonLib::crypto::crypto_vector tweakDataSalt(SALT_SZIE, 0);
					CommonLib::crypto::crypto_vector tweakDataKey(keySize, 0);
					ptrRandomGenerator->GenRandom(tweakDataSalt);

					cryptoBuf.Write(tweakDataSalt.data(), tweakDataSalt.size());

					ptrKeyGenerator->DeriveKeyPBKDF2(m_password, tweakDataSalt, PWD_KEY_ROUNDS, tweakDataKey, keySize);

					ptrAesTweakCipher->SetKey(tweakDataKey);
					ptrAesDataCipher->SetKey(pwdKeyData);

					m_ptrXTSBlockChiper.reset(new crypto::CXTSBlockChiper(ptrAesDataCipher, ptrAesTweakCipher));
				}

				uint32_t crc = util::Crc32(cryptoBuf.Buffer(), CRYPTO_BLOCK_SZIE);				
				ptrAesPwdCipher->Encrypt(cryptoBuf.Buffer(), CRYPTO_BLOCK_SZIE);

				writeTotalBuf.Seek(checkSumPos, soFromBegin);
				writeTotalBuf.Write(crc);
			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExcT("CryptoContext: Failed CreateInitBlock", exc);
				throw;
			}
		}

		void CCryptoContext::FillPageWithRandomData(CommonLib::crypto::IRandomGeneratorPtr ptrRandomGenerator, CommonLib::crypto::IKeyGeneratorPtr ptrKeyGenerator, byte_t *pBuf, size_t size)
		{
			//ptrRandomGenerator->GenRandom(pBuf, (uint32_t)size); //fill data with random
			CommonLib::crypto::IAESCipherPtr ptrAesDataCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::CBC);
			uint32_t blockSize = ptrAesDataCipher->GetBlockSize();
			uint32_t keySize = ptrAesDataCipher->GetKeySize();
			CommonLib::crypto::crypto_vector keyData(keySize, 0); 

			ptrRandomGenerator->GenRandom(ptrAesDataCipher->GetIVData(), ptrAesDataCipher->GetIVSize());
			ptrRandomGenerator->GenRandom(keyData.data(), (uint32_t)keyData.size());
			ptrAesDataCipher->SetKey(keyData);

			ptrAesDataCipher->Encrypt(pBuf, (uint32_t)size);
		}

		bool CCryptoContext::ValidateInitBlock(byte_t *pBuf, size_t size)
		{
			try
			{
				CommonLib::crypto::IKeyGeneratorPtr ptrKeyGenerator = m_ptrCryptoFactory->CreateKeyGenerator();
				CommonLib::crypto::IAESCipherPtr ptrAesPwdCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::CBC);
				uint32_t blockSize = ptrAesPwdCipher->GetBlockSize();
				uint32_t keySize = ptrAesPwdCipher->GetKeySize();
				CommonLib::crypto::crypto_vector pwdKeyData(keySize, 0);

				CommonLib::CReadMemoryStream totalBuf;
				totalBuf.AttachBuffer(pBuf, size);
				CommonLib::crypto::crypto_vector pwdSalt(SALT_SZIE, 0);

				totalBuf.Read(pwdSalt.data(), pwdSalt.size());
				totalBuf.Read(ptrAesPwdCipher->GetIVData(), ptrAesPwdCipher->GetIVSize());

				ptrKeyGenerator->DeriveKeyPBKDF2(m_password, pwdSalt, PWD_KEY_ROUNDS, pwdKeyData, keySize);
				ptrAesPwdCipher->SetKey(pwdKeyData);

				uint32_t crc = totalBuf.ReadIntu32();

				CommonLib::CReadMemoryStream cryptoBuf;
				cryptoBuf.AttachBuffer(totalBuf.Buffer() + totalBuf.Pos(), CRYPTO_BLOCK_SZIE);
				ptrAesPwdCipher->Decrypt(cryptoBuf.Buffer(), CRYPTO_BLOCK_SZIE);

				uint32_t calcCrc = util::Crc32(cryptoBuf.Buffer(), CRYPTO_BLOCK_SZIE);

				if (calcCrc != crc)
					return false;

				m_cryptoType = (EKeyCryptoType)cryptoBuf.ReadIntu32();
				if (m_cryptoType != MasterKey && m_cryptoType != PwdKey)
					return false;

				if (m_cryptoType == MasterKey)
				{
					CommonLib::crypto::IAESCipherPtr ptrAesDataCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
					CommonLib::crypto::IAESCipherPtr ptrAesTweakCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
					CommonLib::crypto::crypto_vector keyData(keySize, 0);
					CommonLib::crypto::crypto_vector keyTweakData(keySize, 0); 

					cryptoBuf.Read(keyData.data(), keyData.size());
					cryptoBuf.Read(keyTweakData.data(), keyTweakData.size());

					ptrAesDataCipher->SetKey(keyData);
					ptrAesTweakCipher->SetKey(keyTweakData);

					m_ptrXTSBlockChiper.reset(new crypto::CXTSBlockChiper(ptrAesDataCipher, ptrAesTweakCipher));
				}
				else
				{
					CommonLib::crypto::IAESCipherPtr ptrAesDataCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
					CommonLib::crypto::IAESCipherPtr ptrAesTweakCipher = m_ptrCryptoFactory->CreateAESCipher(CommonLib::crypto::AES_256, false, CommonLib::crypto::CipherChainMode::ECB);
					CommonLib::crypto::crypto_vector tweakDataSalt(SALT_SZIE, 0);
					CommonLib::crypto::crypto_vector tweakDataKey(keySize, 0);
					cryptoBuf.Read(tweakDataSalt.data(), tweakDataSalt.size());


					ptrKeyGenerator->DeriveKeyPBKDF2(m_password, tweakDataSalt, PWD_KEY_ROUNDS, tweakDataKey, keySize);

					ptrAesDataCipher->SetKey(pwdKeyData);
					ptrAesTweakCipher->SetKey(tweakDataKey);

					m_ptrXTSBlockChiper.reset(new crypto::CXTSBlockChiper(ptrAesDataCipher, ptrAesTweakCipher));
				}

				return true;
			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExcT("CryptoContext: Failed ValidateInitBlock", exc);
				throw;
			}
		}
	}
}