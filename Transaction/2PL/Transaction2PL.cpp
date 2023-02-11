#include "pch.h"
#include "Transaction2PL.h"
#include "../../storage/FileStorage.h"
#include "../TransactionPage.h"

namespace bptreedb
{
	namespace transaction
	{
		CTransaction2PL::CTransaction2PL(CommonLib::IAllocPtr ptrAlloc, int32_t storageId, int32_t nCacheSize, storage::IStorageCipherPtr ptrCipher,
			storage::IStoragesHolderPtr ptrStoragesHolder, const util::CUInt128& lsn, uint32_t userId, const char* pszNameUtf8, uint32_t nMinPageSize) :
			m_Lsn(lsn), m_UserId(userId), m_ptrAlloc(ptrAlloc)
		{
			try
			{
				m_ptrLogStorage = std::make_shared<storage::CFileStorage>(ptrAlloc, storageId, nCacheSize, ptrCipher);
				m_ptrLogStorage->Open(pszNameUtf8, true, 0, nMinPageSize);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to create transaction",  excSrc);
				throw;
			}
		}

		CTransaction2PL::CTransaction2PL(CommonLib::IAllocPtr ptrAlloc, int32_t storageId, int32_t nCacheSize, storage::IStorageCipherPtr ptrCipher,
			storage::IStoragesHolderPtr ptrStoragesHolder, const char* pszNameUtf8, uint32_t nMinPageSize)
		{
			try
			{
				m_ptrLogStorage = std::make_shared<storage::CFileStorage>(ptrAlloc, storageId, nCacheSize, ptrCipher);
				m_ptrLogStorage->Open(pszNameUtf8, false, 0, nMinPageSize);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to open transaction", excSrc);
				throw;
			}
		}

		util::CUInt128 CTransaction2PL::GetTransactionLSN() const
		{
			return m_Lsn;
		}

		int64_t CTransaction2PL::GetAddressInLog(uint32_t nStorageId, int64_t nAddr)
		{
			TMapPageAddrToLog::iterator it = m_PagesAddr.find(SPageKey(nStorageId, nAddr));
			return it != m_PagesAddr.end() ? it->second : -1;
		}

		ITransactionPagePtr CTransaction2PL::ReadPage(uint32_t nStorageId, int64_t nAddr, uint32_t nSize)
		{

			try
			{
				ITransactionPagePtr ptrPage = std::make_shared<CTransactionPage>(m_ptrAlloc, nSize, nAddr, nStorageId, m_Lsn, m_UserId);
				ReadPage(ptrPage, nStorageId, nAddr);
				return ptrPage;

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to read page, addr %1, size %2, storageId %3", nAddr, nSize, nStorageId, excSrc);
				throw;
			}
		}

		void CTransaction2PL::ReadPage(ITransactionPagePtr ptrPage, uint32_t nStorageId, int64_t nAddr)
		{
			try
			{
				int64_t nAddrInLog = GetAddressInLog(nStorageId, nAddr);

				if (nAddrInLog == -1)
				{
					storage::IStoragePtr ptrStorage = m_ptrStoragesHolder->GetStorageThrowIfNull(nStorageId);
					ptrPage->Read(ptrStorage, nAddr);
				}

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to read page, addr %1,  storageId %2", nAddr, nStorageId, excSrc);
				throw;
			}
		}

		void CTransaction2PL::SavePage(ITransactionPagePtr ptrPage)
		{
			try
			{
				int64_t nAddrInLog = GetAddressInLog(ptrPage->GetStorageId(), ptrPage->GetAddr());
				if (nAddrInLog == -1)
				{
					nAddrInLog = m_ptrLogStorage->GetNewFilePageAddr(ptrPage->GetSize());
					m_PagesAddr.insert(std::make_pair(SPageKey(ptrPage->GetStorageId(), ptrPage->GetAddr()), nAddrInLog));
				}

				ptrPage->Save(m_ptrLogStorage, nAddrInLog);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Tran2PLStorag filed to save page addr: %1, storageId %2", ptrPage->GetAddr(), ptrPage->GetStorageId(), excSrc);
				throw;
			}
		}

		void CTransaction2PL::DropPage(ITransactionPagePtr ptrPage)
		{
			try
			{


			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to drop page addr: %1, storageId %2", ptrPage->GetAddr(), ptrPage->GetStorageId(), excSrc);
				throw;
			}
		}

		int64_t CTransaction2PL::GetNewFilePageAddr(uint32_t nStorageId, uint32_t nSize)
		{
			try
			{
				storage::IStoragePtr ptrStorage = m_ptrStoragesHolder->GetStorageThrowIfNull(nStorageId);
				return ptrStorage->GetNewFilePageAddr(nStorageId);

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to get new file addr, size: %1", nSize, excSrc);
				throw;
			}
		}

		ITransactionPagePtr CTransaction2PL::CreateNewPage(uint32_t nStorageId, int64_t nAddr, uint32_t nSize)
		{
			try
			{
				ITransactionPagePtr ptrPage = std::make_shared<CTransactionPage>(m_ptrAlloc, nSize, nAddr, nStorageId, m_Lsn, m_UserId);
				return ptrPage;

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to create new file addr, addr: %1, size: %2, storage %3", nAddr, nSize, nStorageId, excSrc);
				throw;
			}
		}

		void CTransaction2PL::Commit()
		{
			try
			{
			 

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to commit", excSrc);
				throw;
			}
		}

		void CTransaction2PL::Rollback()
		{
			try
			{
				 

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL filed to rollback", excSrc);
				throw;
			}
		}

	}
}