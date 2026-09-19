#include "stdafx.h"
#include "SymmetricKey.h"
#include "../../exception/WinNtExc.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace winapi
		{
			CSymmetricKey::CSymmetricKey(BCRYPT_KEY_HANDLE hKey, PBYTE pbKeyObject, uint32_t keyObjectSize) : m_hKey(hKey),
				m_pbKeyObject(pbKeyObject), m_keyObjectSize(keyObjectSize)
			{
		
			}

			CSymmetricKey::~CSymmetricKey()
			{
				if (m_hKey)
				{
					::BCryptDestroyKey(m_hKey);
					m_hKey = 0;
				}
				if (m_pbKeyObject)
				{
					memset(m_pbKeyObject, 0, m_keyObjectSize);
					::HeapFree(::GetProcessHeap(), 0, m_pbKeyObject);
					m_pbKeyObject = 0;
				}
			}

			void CSymmetricKey::CryptEncrypt(PUCHAR pbInput, ULONG cbInput, VOID  *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG  cbOutput, ULONG *pcbResult, ULONG  dwFlags)
			{
				try
				{
					NTSTATUS ntStatus = ::BCryptEncrypt(m_hKey, pbInput, cbInput, pPaddingInfo, pbIV, cbIV, pbOutput, cbOutput, pcbResult, dwFlags);
					if (!BCRYPT_SUCCESS(ntStatus))
						throw CWinNtExc(ntStatus);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("SymmetricKey: Failed to encrypt", exc);
				}
			}

			void CSymmetricKey::CryptDecrypt(PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG  cbOutput, ULONG *pcbResult, ULONG  dwFlags)
			{
				try
				{
					NTSTATUS ntStatus = ::BCryptDecrypt(m_hKey, pbInput, cbInput, pPaddingInfo, pbIV, cbIV, pbOutput, cbOutput, pcbResult, dwFlags);
					if (!BCRYPT_SUCCESS(ntStatus))
						throw CWinNtExc(ntStatus);
				}
				catch (std::exception& exc)
				{
					CExcBase::RegenExcT("SymmetricKey: Failed to encrypt", exc);
				}
			}
		}
	}
}