#include "pch.h"
#include "TransactionFilePager.h"
#include "TransactionPage.h"

namespace bptreedb
{
	namespace transaction
	{
		CTransactionFilePager::CTransactionFilePager(storage::IStoragePtr ptrStorage, util::CUInt128 lsn, CommonLib::IAllocPtr ptrAlloc, uint32_t userId) : 
			m_ptrStorage(ptrStorage), m_Lsn(lsn), m_ptrAlloc(ptrAlloc), m_UserId(userId)
		{}

		CTransactionFilePager::~CTransactionFilePager()
		{}

		int64_t CTransactionFilePager::GetNewFilePageAddr()
		{
			try
			{ 
				return m_ptrStorage->GetNewFilePageAddr();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("TransactionFilePager failed to get new addr", excSrc);
				throw;
			}
		}

		storage::IFilePagePtr CTransactionFilePager::GetNewPage(int64_t nAddr)
		{
			try
			{
				storage::IFilePagePtr ptrPage = std::make_shared<CTransactionPage>(m_ptrAlloc, m_ptrStorage->GetPageSize(), nAddr, m_ptrStorage->GetStorageId(), m_Lsn, m_UserId);
				return ptrPage;
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("TransactionFilePager failed to get new page", excSrc);
				throw;
			}
		}

		storage::IFilePagePtr CTransactionFilePager::ReadPage(int64_t nAddr)
		{
			try
			{
				storage::IFilePagePtr ptrPage = std::make_shared<CTransactionPage>(m_ptrAlloc, m_ptrStorage->GetPageSize(), nAddr, m_ptrStorage->GetStorageId(), m_Lsn, m_UserId);
				ptrPage->Read(m_ptrStorage, nAddr);

				return ptrPage;
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("TransactionFilePager failed read page, addr: {1}", nAddr, excSrc);
				throw;
			}
		}

		void CTransactionFilePager::ReadPage(storage::IFilePagePtr ptrPage, int64_t nAddr)
		{
			try
			{			 
				ptrPage->Read(m_ptrStorage, nAddr); 
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("TransactionFilePager failed read page, addr: {1}", nAddr, excSrc);
				throw;
			}
		}

		void CTransactionFilePager::SavePage(storage::IFilePagePtr ptrPage)
		{
			try
			{
				ptrPage->Save(m_ptrStorage);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("TransactionFilePager failed read page, addr: {1}", ptrPage->GetAddr(), excSrc);
				throw;
			}
		}
	}
}