#include "stdafx.h"
#include "CryptoAlgoritmProvider.h"
#include "../../exception/WinNtExc.h"
namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			CBcryptAlgProviderPtr CBcryptAlgProvider::Create(LPCWSTR pszAlgId, ULONG dwFlags)
			{
				try
				{
					BCRYPT_ALG_HANDLE handle;
					NTSTATUS ntStatus = BCryptOpenAlgorithmProvider(&handle, pszAlgId, NULL, dwFlags);
					if (!BCRYPT_SUCCESS(ntStatus))
						throw CWinNtExc(ntStatus);

					return CBcryptAlgProviderPtr(new CBcryptAlgProvider(handle));
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("CNG: Failed to create provider AlgId: {0}, Flags: {1}", pszAlgId, (uint32_t)dwFlags, exc);
					throw;
				}
			}

			CBcryptAlgProvider::CBcryptAlgProvider(BCRYPT_ALG_HANDLE phAlgorithm) : m_hAlgorithm(phAlgorithm)
			{

			}

			CBcryptAlgProvider::~CBcryptAlgProvider()
			{
				if (m_hAlgorithm)
					::BCryptCloseAlgorithmProvider(&m_hAlgorithm, 0);
			}


			void CBcryptAlgProvider::CryptDeriveKeyPBKDF2(const crypto_astr& password, const crypto_vector& salt, uint32_t interations, uint32_t keySize, crypto_vector& derivedKey)
			{
				try
				{
					derivedKey.resize(keySize);
					NTSTATUS ntStatus = ::BCryptDeriveKeyPBKDF2(m_hAlgorithm, (PUCHAR)password.c_str(), (ULONG)password.size(),
						(PUCHAR)salt.data(), (ULONG)salt.size(), interations, (PUCHAR)derivedKey.data(), (ULONG)derivedKey.size(), 0);

					if (!BCRYPT_SUCCESS(ntStatus))
						throw CWinNtExc(ntStatus);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExc("CNG: Failed to derive key", exc);
					throw;
				}
			}

			void CBcryptAlgProvider::CryptGenRandom(byte* pBuf, uint32_t size, ULONG dwFlags)
			{
				try
				{
	
					NTSTATUS ntStatus = ::BCryptGenRandom(m_hAlgorithm, (PUCHAR)pBuf, (ULONG)size, dwFlags);

					if (!BCRYPT_SUCCESS(ntStatus))
						throw CWinNtExc(ntStatus);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExc("CNG: Failed get random", exc);
					throw;
				}
			}

			BCRYPT_KEY_HANDLE CBcryptAlgProvider::CryptGenerateSymmetricKey(PUCHAR pbKeyObject, ULONG cbKeyObject, PUCHAR  pbSecret, ULONG  cbSecret, ULONG dwFlags)
			{
				try
				{
					BCRYPT_KEY_HANDLE hKey = 0;
					NTSTATUS ntStatus = ::BCryptGenerateSymmetricKey(m_hAlgorithm, &hKey, pbKeyObject, cbKeyObject, pbSecret, cbSecret, dwFlags);

					if (!BCRYPT_SUCCESS(ntStatus))
						throw CWinNtExc(ntStatus);

					return hKey;
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExc("CNG: Failed to generate symmetric key", exc);
					throw;
				}
			}

			void CBcryptAlgProvider::CryptGetProperty(LPCWSTR pszProperty, PUCHAR  pbOutput, ULONG  cbOutput, ULONG  *pcbResult, ULONG dwFlags)
			{
				try
				{				 
					NTSTATUS ntStatus = ::BCryptGetProperty(m_hAlgorithm, pszProperty, pbOutput, cbOutput, pcbResult, dwFlags);

					if (!BCRYPT_SUCCESS(ntStatus))
						throw CWinNtExc(ntStatus);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("CNG: Failed get property", pszProperty, exc);
					throw;
				}
			}

			void CBcryptAlgProvider::CryptSetProperty(LPCWSTR pszProperty, PUCHAR  pbInput, ULONG  cbInput, ULONG dwFlags)
			{
				try
				{
					NTSTATUS ntStatus = ::BCryptSetProperty(m_hAlgorithm, pszProperty, pbInput, cbInput, dwFlags);
					if (!BCRYPT_SUCCESS(ntStatus))
						throw CWinNtExc(ntStatus);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("CNG: Failed set property", pszProperty, exc);
					throw;
				}
			}
		}
	}
}
