#include "stdafx.h"
#include "TransactionLog2PL.h"

namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			CTransactionLog::CTransactionLog(CommonLib::IAllocPtr ptrAlloc, bool bCheckCRC, IFileStoragesHolderPtr ptrFileStoragesHolder) : m_ptrAlloc(ptrAlloc),
				m_ptrFileStoragesHolder(ptrFileStoragesHolder)
			{
				m_ptrFileStorage.reset(new CFileStorage(ptrAlloc, bCheckCRC));
			}

			CTransactionLog::~CTransactionLog()
			{

			}

			void CTransactionLog::Open(const astr& fileName, bool bCreate, uint32_t nMinPageSize)
			{
				try
				{
					m_ptrFileStorage->Open(fileName.c_str(), bCreate, nMinPageSize);

				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to open transaction log", excSrc);
					throw;
				}
			}

			void CTransactionLog::Close()
			{
				try
				{
					m_ptrFileStorage->Close();
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to close transaction log", excSrc);
					throw;
				}
			}

			void CTransactionLog::Flush()
			{
				try
				{
					m_ptrFileStorage->Flush();
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to flush transaction log", excSrc);
					throw;
				}
			}

			FilePagePtr CTransactionLog::GetFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, bool decrypt)
			{
				try
				{
					TMapPages::iterator it = m_pages.find(CPageKey(nStorageId, nAddr));
					if (it == m_pages.end())
					{
						CFileStoragePtr ptrFileStorage = m_ptrFileStoragesHolder->GetStorage(nStorageId);
						if (ptrFileStorage.get() == nullptr)
							throw CommonLib::CExcBase("Storage with id: %1 not found", nStorageId);

						FilePagePtr ptrPage = ptrFileStorage->GetFilePage(nAddr, nSize, decrypt);


						SPageInfo pageInfo;
						pageInfo.m_flags = eFP_NULL;
						pageInfo.m_nAddrInLog = -1;
						pageInfo.m_nSize = nSize;
 
						m_pages.insert(std::make_pair(CPageKey(nStorageId, nAddr), pageInfo));
						return ptrPage;

					}
					else
					{
						SPageInfo& pageInfo = it->second;

						if(pageInfo.m_nSize != nSize)
							throw CommonLib::CExcBase("Wrong page size, addr %1, add in log %2, size %3, size in log %4", nAddr, pageInfo.m_nAddrInLog, nSize, pageInfo.m_nSize);


						FilePagePtr ptrPage = m_ptrFileStorage->GetFilePage(pageInfo.m_nAddrInLog, nSize, decrypt);
						ptrPage->SetAddr(nAddr);

						return ptrPage;
					}

				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to GetFilePage from transaction log", excSrc);
					throw;
				}
			}

			void CTransactionLog::DropFilePage(int32_t nStorageId, int64_t nAddr)
			{
				try
				{
					TMapPages::iterator it = m_pages.find(CPageKey(nStorageId, nAddr));
					if (it != m_pages.end())
						throw CommonLib::CExcBase("Page with Storage: %1, Addr %2 not found", nStorageId, nAddr);

					SPageInfo& pageInfo = it->second;
					pageInfo.m_flags |= eFP_REMOVE;

				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to DropFilePage from transaction log", excSrc);
					throw;
				}
			}

			void CTransactionLog::SaveFilePage(int32_t nStorageId, FilePagePtr ptrPage)
			{
				try
				{
					TMapPages::iterator it = m_pages.find(CPageKey(nStorageId, ptrPage->GetPageAddr()));
					if (it == m_pages.end())
						throw CommonLib::CExcBase("Can't find page, StorageId: %1, Addr: %2", nStorageId, ptrPage->GetPageAddr());

					SPageInfo& pageInfo = it->second;

					if (pageInfo.m_nAddrInLog == -1)
						pageInfo.m_nAddrInLog = m_ptrFileStorage->GetNewFilePageAddr(ptrPage->GetFullPageSize());

					int64_t nAddr = ptrPage->GetPageAddr();
					ptrPage->SetAddr(pageInfo.m_nAddrInLog);

					m_ptrFileStorage->SaveFilePage(ptrPage);
					ptrPage->SetAddr(nAddr);
					pageInfo.m_flags |= eFP_CHANGE;
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to save page", excSrc);
					throw;
				}
			}

			FilePagePtr  CTransactionLog::GetNewFilePage(int32_t nStorageId, uint32_t objectID, ObjectPageType objectType, uint32_t parentID, ObjectPageType parentType, uint32_t nSize )
			{
				try
				{
					CFileStoragePtr ptrFileStorage = m_ptrFileStoragesHolder->GetStorage(nStorageId);
					if (ptrFileStorage.get() == nullptr)
						throw CommonLib::CExcBase("Storage with id: %1 not found", nStorageId);

					FilePagePtr ptrPage = ptrFileStorage->GetNewFilePage(objectID, objectType, parentID, parentType, nSize);

					SPageInfo pageInfo;
					pageInfo.m_flags = eFP_NEW;
					pageInfo.m_nAddrInLog = -1;
					pageInfo.m_nSize = nSize;

					m_pages.insert(std::make_pair(CPageKey(nStorageId, ptrPage->GetPageAddr()), pageInfo));

					return ptrPage;

				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to save page", excSrc);
					throw;
				}
			}	

			int64_t CTransactionLog::GetNewFilePageAddr(int32_t nStorageId, uint32_t nSize)
			{
				try
				{
					CFileStoragePtr ptrFileStorage = m_ptrFileStoragesHolder->GetStorage(nStorageId);
					if (ptrFileStorage.get() == nullptr)
						throw CommonLib::CExcBase("Storage with id: %1 not found", nStorageId);

					return ptrFileStorage->GetNewFilePageAddr(nSize);
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to get new file page", excSrc);
					throw;
				}
			}


			FilePagePtr CTransactionLog::GetEmptyFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype)
			{
				try
				{
					CFileStoragePtr ptrFileStorage = m_ptrFileStoragesHolder->GetStorage(nStorageId);
					if (ptrFileStorage.get() == nullptr)
						throw CommonLib::CExcBase("Storage with id: %1 not found", nStorageId);

					return ptrFileStorage->GetEmptyFilePage(nAddr, nSize, objectID, objecttype, parentID, parenttype);
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionLog: Failed to get empty page", excSrc);
					throw;
				}
			}

		}
	}
}
