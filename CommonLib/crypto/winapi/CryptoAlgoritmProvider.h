#pragma once
#include "../CryptoData.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			typedef std::shared_ptr<class CBcryptAlgProvider> CBcryptAlgProviderPtr;

			class CBcryptAlgProvider
			{
			public:
				static	CBcryptAlgProviderPtr Create(LPCWSTR pszAlgId, ULONG dwFlags);
				~CBcryptAlgProvider();

				void CryptDeriveKeyPBKDF2(const crypto_astr& password, const crypto_vector& salt, uint32_t interations, uint32_t keySize, crypto_vector& derivedKey);
				void CryptGenRandom(byte* pBuf, uint32_t size, ULONG dwFlags);
				BCRYPT_KEY_HANDLE CryptGenerateSymmetricKey(PUCHAR pbKeyObject, ULONG cbKeyObject, PUCHAR  pbSecret, ULONG  cbSecret, ULONG dwFlags);
				void CryptGetProperty(LPCWSTR pszProperty, PUCHAR  pbOutput, ULONG  cbOutput, ULONG  *pcbResult, ULONG dwFlags);
				void CryptSetProperty(LPCWSTR pszProperty, PUCHAR  pbInput, ULONG  cbInput,  ULONG dwFlags);

			private:
				CBcryptAlgProvider(BCRYPT_ALG_HANDLE phAlgorithm);
			private:
				BCRYPT_ALG_HANDLE m_hAlgorithm{ 0 };
			};
		}
	}
}