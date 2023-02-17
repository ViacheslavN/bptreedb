#include "pch.h"
#include <algorithm>
#include "Transaction2PL.h"
#include "../../storage/FileStorage.h"
#include "../TransactionPage.h"
#include "../TransactionFilePager.h"
#include "../../storage/WriteStreamPage.h"
#include "../../storage/ReadStreamPage.h"



namespace bptreedb
{
	namespace transaction
	{
		CTransaction2PL::CTransaction2PL(CommonLib::IAllocPtr ptrAlloc, int32_t storageId, int32_t nCacheSize, storage::IStorageCipherPtr ptrCipher,
			storage::IStoragesHolderPtr ptrStoragesHolder, const util::CUInt128& lsn, const util::CUInt128& prevLsn, uint32_t userId, const char* pszNameUtf8, uint32_t nMinPageSize) :
			m_Lsn(lsn), m_PrevLsn(prevLsn), m_UserId(userId), m_ptrAlloc(ptrAlloc)
		{
			try
			{
				m_ptrLogStorage = std::make_shared<storage::CFileStorage>(ptrAlloc, storageId, nCacheSize, ptrCipher);
				m_ptrLogStorage->Open(pszNameUtf8, true, 0, nMinPageSize);
				StartTransaction();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to create transaction",  excSrc);
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
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to open transaction", excSrc);
				throw;
			}
		}

		void CTransaction2PL::StartTransaction()
		{
			try
			{
				int64_t nAddr = m_ptrLogStorage->GetNewFilePageAddr();
				if (nAddr != 0)
					throw CommonLib::CExcBase("Transaction2PL: transaction file isn't empty");

				storage::IFilePagerPtr ptrTransactionFilePage = std::make_shared<CTransactionFilePager>(m_ptrLogStorage, m_Lsn, m_ptrAlloc, m_UserId);
				storage::TWriteStreamPage stream(ptrTransactionFilePage);
				stream.Open(m_nTranInfoStreamAddr, true);
				stream.Close();		

				m_nUndoStreamAddr = m_ptrLogStorage->GetNewFilePageAddr();

				storage::TWriteStreamPage undoStream(ptrTransactionFilePage);
				undoStream.Open(m_nUndoStreamAddr, true);
				undoStream.Close();

				m_nChangedPageStreamAddr = m_ptrLogStorage->GetNewFilePageAddr();
				storage::TWriteStreamPage changePageStream(ptrTransactionFilePage);
				changePageStream.Open(m_nChangedPageStreamAddr, true);
				changePageStream.Close();

				SaveHeader(eInitState);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to start transaction", excSrc);
				throw;
			}
		}

		util::CUInt128 CTransaction2PL::GetTransactionLSN() const
		{
			return m_Lsn;
		}

		int64_t CTransaction2PL::GetAddressInLog(int32_t nStorageId, int64_t nAddr)
		{

			TMapPageAddr& mapPageAddr = m_PagesAddr[nStorageId];
			TMapPageAddr::iterator it = mapPageAddr.find(nAddr);
			return it != mapPageAddr.end() ? it->second : -1;
		}

		void CTransaction2PL::AddAddressInLog(int32_t nStorageId, int64_t nAddr, int64_t nAddrInLog)
		{
			TMapPageAddr& mapPageAddr = m_PagesAddr[nStorageId];
			mapPageAddr.insert(std::make_pair(nAddr, nAddrInLog));
		}

		uint32_t CTransaction2PL::GetPageSize() const
		{
			return m_ptrLogStorage->GetPageSize();
		}

		storage::IFilePagePtr CTransaction2PL::ReadPage(int32_t nStorageId, int64_t nAddr)
		{
			try
			{
				storage::IFilePagePtr ptrPage = std::make_shared<CTransactionPage>(m_ptrAlloc, m_ptrLogStorage->GetPageSize(), nAddr, nStorageId, m_Lsn, m_UserId);
				ReadPage(ptrPage, nStorageId, nAddr);
				return ptrPage;

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to read page, addr {0}, size {1}, storageId {2}", nAddr, m_ptrLogStorage->GetPageSize(), nStorageId, excSrc);
				throw;
			}
		}

		void CTransaction2PL::ReadPage(storage::IFilePagePtr ptrPage, int32_t nStorageId, int64_t nAddr)
		{
			try
			{
				if (ptrPage->GetSize() != m_ptrLogStorage->GetPageSize())
					throw CommonLib::CExcBase("Transaction2PL wrong page size {0}", ptrPage->GetSize());

				int64_t nAddrInLog = GetAddressInLog(nStorageId, nAddr);

				if (nAddrInLog == -1)
				{
					storage::IStoragePtr ptrStorage = m_ptrStoragesHolder->GetStorageThrowIfNull(nStorageId);
					ptrPage->Read(ptrStorage, nAddr);
				}
				else
				{
					ptrPage->Read(m_ptrLogStorage, nAddrInLog);
					ptrPage->SetAddr(nAddr);
				}

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to read page, addr {0},  storageId {1}", nAddr, nStorageId, excSrc);
				throw;
			}
		}

		void CTransaction2PL::SavePage(storage::IFilePagePtr ptrPage, int32_t nStorageId)
		{
			try
			{
				if (ptrPage->GetSize() != m_ptrLogStorage->GetPageSize())
					throw CommonLib::CExcBase("Transaction2PL wrong page size {0}", ptrPage->GetSize());

				int64_t nAddrInLog = GetAddressInLog(nStorageId, ptrPage->GetAddr());
				if (nAddrInLog == -1)
				{
					storage::IStoragePtr ptrStorage = m_ptrStoragesHolder->GetStorageThrowIfNull(nStorageId);
					storage::IFilePagePtr ptrUndoPage = std::make_shared<CTransactionPage>(m_ptrAlloc, ptrPage->GetSize(), ptrPage->GetAddr(), nStorageId, m_Lsn, m_UserId);
					ptrUndoPage->Read(ptrStorage, ptrPage->GetAddr());
					
					SavePageForUndo(ptrUndoPage);

					nAddrInLog = m_ptrLogStorage->GetNewFilePageAddr(ptrPage->GetSize());
					AddAddressInLog(nStorageId, ptrPage->GetAddr(), nAddrInLog);
				}

				ptrPage->Save(m_ptrLogStorage, nAddrInLog);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Tran2PLStorag failed  to save page addr: {1}, storageId {2}", ptrPage->GetAddr(), nStorageId, excSrc);
				throw;
			}
		}

		void CTransaction2PL::DropPage(storage::IFilePagePtr ptrPage, int32_t nStorageId)
		{
			try
			{


			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to drop page addr: {1}, storageId {2}", ptrPage->GetAddr(), nStorageId, excSrc);
				throw;
			}
		}

		int64_t CTransaction2PL::GetNewFilePageAddr(int32_t nStorageId)
		{
			try
			{
				storage::IStoragePtr ptrStorage = m_ptrStoragesHolder->GetStorageThrowIfNull(nStorageId);
				return ptrStorage->GetNewFilePageAddr();

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to get new file addr", excSrc);
				throw;
			}
		}

		storage::IFilePagePtr CTransaction2PL::CreateNewPage(int32_t nStorageId, int64_t nAddr)
		{
			try
			{
				storage::IFilePagePtr ptrPage = std::make_shared<CTransactionPage>(m_ptrAlloc, m_ptrLogStorage->GetPageSize(), nAddr, nStorageId, m_Lsn, m_UserId);
				return ptrPage;

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to create new file addr, addr: {1}, size: {2}, storage {3}", nAddr, m_ptrLogStorage->GetPageSize(), nStorageId, excSrc);
				throw;
			}
		}

		void CTransaction2PL::SavePageForUndo(storage::IFilePagePtr ptrPage)
		{
			try
			{

				if (ptrPage->GetSize() != m_ptrLogStorage->GetPageSize())
					throw CommonLib::CExcBase("Transaction2PL wrong page size {0}", ptrPage->GetSize());

				int64_t nAddr = m_ptrLogStorage->GetNewFilePageAddr();
				m_PagesForUndo.push_back(nAddr);
				ptrPage->Save(m_ptrLogStorage, nAddr);

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to save page for undo addr: {1}", ptrPage->GetAddr(), excSrc);
				throw;
			}
		}

		void CTransaction2PL::SaveHeader(TransactionState state)
		{
			try
			{
				storage::IFilePagerPtr ptrTransactionFilePage = std::make_shared<CTransactionFilePager>(m_ptrLogStorage, m_Lsn, m_ptrAlloc, m_UserId);
				storage::TWriteStreamPage stream(ptrTransactionFilePage);
				stream.Open(m_nTranInfoStreamAddr, false);
				stream.Write((uint32_t)state);
				stream.Write(m_nUndoStreamAddr);
				stream.Write(m_nChangedPageStreamAddr);
				stream.Close();

				m_ptrLogStorage->Flush();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to save ", excSrc);
				throw;
			}
		}

		void CTransaction2PL::SaveUndoPageAddrs()
		{
			try
			{
				storage::IFilePagerPtr ptrTransactionFilePage = std::make_shared<CTransactionFilePager>(m_ptrLogStorage, m_Lsn, m_ptrAlloc, m_UserId);
				storage::TWriteStreamPage stream(ptrTransactionFilePage);
				stream.Open(m_nUndoStreamAddr, false);
				stream.Write(uint32_t(m_PagesForUndo.size()));

				for (size_t i = 0; i < m_PagesForUndo.size(); ++i)
				{
					stream.Write(m_PagesForUndo[i]);
				}

				stream.Close();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to save  undo page addrs", excSrc);
				throw;
			}
		}

		void CTransaction2PL::SaveChangedPageAddrs()
		{
			try
			{
				storage::IFilePagerPtr ptrTransactionFilePage = std::make_shared<CTransactionFilePager>(m_ptrLogStorage, m_Lsn, m_ptrAlloc, m_UserId);
				storage::TWriteStreamPage stream(ptrTransactionFilePage);
				stream.Open(m_nUndoStreamAddr, false);
				stream.Write((uint32_t)m_PagesAddr.size());

				for (TMapPageAddrbyStorage::iterator it = m_PagesAddr.begin(); it != m_PagesAddr.end(); ++it)
				{
					stream.Write(it->first); //Storage Id
					TMapPageAddr& mapPageAddr = it->second;

					stream.Write((uint32_t)mapPageAddr.size());

					for (TMapPageAddr::iterator addr_it = mapPageAddr.begin(); addr_it != mapPageAddr.end(); ++addr_it)
					{
						stream.Write(addr_it->first); //Page Addr 
						stream.Write(addr_it->second); //Add in log

					}
				}

				stream.Close();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to save  undo page addrs", excSrc);
				throw;
			}
		}


		void CTransaction2PL::StartCommit()
		{	
			try
			{

				SaveUndoPageAddrs();
				SaveChangedPageAddrs();

				SaveHeader(eSavePages);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to start commit ", excSrc);
				throw;
			}
		}


		void CTransaction2PL::Commit()
		{
			try
			{
				//Saving all pages
				StartCommit();
				std::vector<storage::IFilePagePtr> cachePages;
				for (TMapPageAddrbyStorage::iterator it = m_PagesAddr.begin(); it != m_PagesAddr.end(); ++it)
				{
					TMapPageAddr& mapPageAddr = it->second;
					std::vector<int64_t> vecAddrsInLog;
					for (TMapPageAddr::iterator addr_it = mapPageAddr.begin(); addr_it != mapPageAddr.end(); ++addr_it)
					{
						vecAddrsInLog.push_back(addr_it->second);
					}

					CommitToStorage(vecAddrsInLog, it->first, cachePages);
				}
	
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to commit", excSrc);
				throw;
			}
		}

		void CTransaction2PL::CommitToStorage(std::vector<int64_t>& addrs, int64_t nStorageId, std::vector<storage::IFilePagePtr>& cachePages)
		{
			try
			{
				const uint32_t _pageCacheSize = 1000;

				storage::IStoragePtr ptrStorage = m_ptrStoragesHolder->GetStorageThrowIfNull(nStorageId);





			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to commit in storage, storage id: {1}", nStorageId, excSrc);
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
				CommonLib::CExcBase::RegenExcT("Transaction2PL failed to rollback", excSrc);
				throw;
			}
		}

	}
}