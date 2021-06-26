#include "stdafx.h"
#include "TransactionLog2PL.h"

namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			CTransactionLog::CTransactionLog(CommonLib::IAllocPtr ptrAlloc, bool bCheckCRC) : m_ptrAlloc(ptrAlloc)
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
					CommonLib::CExcBase::RegenExcT("Failed to open transaction log", excSrc);
					throw;
				}
			}

			void CTransactionLog::Close()
			{
				try
				{
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("Failed to close transaction log", excSrc);
					throw;
				}
			}

			void CTransactionLog::Flush()
			{
				try
				{
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("Failed to flush transaction log", excSrc);
					throw;
				}
			}

			FilePagePtr CTransactionLog::GetFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, bool decrypt)
			{
				try
				{
					SPageInfo pageInfo;
					TMapPages::iterator it = m_pages.find(CPageKey(nStorageId, nAddr));
					if (it == m_pages.end())
					{

					}

				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("Failed to getfilepage from transaction log", excSrc);
					throw;
				}
			}
		}
	}
}
