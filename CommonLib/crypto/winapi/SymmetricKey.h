#pragma once
#include "../crypto.h"
#include "CryptoAlgoritmProvider.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			typedef std::shared_ptr<class CSymmetricKey> CSymmetricKeyPtr;

			class CSymmetricKey  
			{
			public:
				CSymmetricKey(BCRYPT_KEY_HANDLE hKey, PBYTE pbKeyObject, uint32_t keyObjectSize);
				~CSymmetricKey();

				void CryptEncrypt(PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG  cbOutput, ULONG *pcbResult, ULONG  dwFlags);
				void CryptDecrypt(PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG  cbOutput, ULONG *pcbResult, ULONG  dwFlags);
			private:
				BCRYPT_KEY_HANDLE m_hKey{ 0 };
		
				PBYTE m_pbKeyObject;
				uint32_t m_keyObjectSize;
			};
		}
	}
}