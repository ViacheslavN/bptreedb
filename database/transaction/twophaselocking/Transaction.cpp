#include "stdafx.h"
#include "Transaction.h"

namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			CTransaction::CTransaction(const astr& transactionLogFile, const CommonLib::CGuid& guid, eTransactionDataType type, CommonLib::IAllocPtr ptrAlloc, bool bCheckCRC,
				IFileStoragesHolderPtr ptrStoragesHolder) : CTransactonBase(guid, type, ptrStoragesHolder),
				m_transactionLogFile(transactionLogFile)
			{
				m_ptrTransactionLog.reset(new CTransactionLog(ptrAlloc, bCheckCRC, ptrStoragesHolder));
			}

			CTransaction::~CTransaction()
			{

			}

			void CTransaction::Begin()
			{
				try
				{
					m_ptrTransactionLog->Open(m_transactionLogFile, true);
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("Failed to begin transaction", excSrc);
					throw;
				}
			}

			void CTransaction::Commit()
			{

			}

			void CTransaction::Rollback()
			{

			}
					

			IStatementPtr CTransaction::CreateStatement(const astr& sSQLQuery)
			{
				return IStatementPtr();
			}

			ICursorPtr CTransaction::ExecuteQuery(IStatementPtr pStatement)
			{
				return ICursorPtr();
			}

			ICursorPtr CTransaction::ExecuteSelectQuery(const astr& sTable, const std::vector<astr>& fileds)
			{
				return ICursorPtr();
			}

			ICursorPtr CTransaction::ExecuteSelectQuery(const astr& sTable, const std::vector<astr>& fileds, const astr& sField, IValuePtr ptrVal, eOpType opType)
			{
				return ICursorPtr();
			}

			IInsertCursorPtr CTransaction::CreateInsertCursor(const astr& sTable, const std::vector<astr>& fileds)
			{
				return IInsertCursorPtr();
			}

			IUpdateCursorPtr CTransaction::CreateUpdateCursor()
			{
				return IUpdateCursorPtr();
			}

			IDeleteCursorPtr CTransaction::CreateDeleteCursor(const astr& sTable)
			{
				return IDeleteCursorPtr();
			}

			//IDBTransaction
			FilePagePtr CTransaction::GetFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, bool decrypt)
			{
				return m_ptrTransactionLog->GetFilePage(nStorageId, nAddr, nSize, decrypt);
			}

			void CTransaction::SaveFilePage(int32_t nStorageId, FilePagePtr& ptrPage)
			{
				m_ptrTransactionLog->SaveFilePage(nStorageId, ptrPage);
			}

			void CTransaction::DropFilePage(int32_t nStorageId, int64_t nAddr)
			{
				m_ptrTransactionLog->DropFilePage(nStorageId, nAddr);
			}

			FilePagePtr CTransaction::GetNewFilePage(int32_t nStorageId, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize)
			{
				return 	m_ptrTransactionLog->GetNewFilePage(nStorageId, objectID, objecttype, parentID, parenttype, nSize);
			}

			int64_t CTransaction::GetNewFilePageAddr(int32_t nStorageId, uint32_t nSize)
			{
				return 	m_ptrTransactionLog->GetNewFilePageAddr(nStorageId,  nSize);
			}

			FilePagePtr CTransaction::GetEmptyFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype)
			{
				return 	m_ptrTransactionLog->GetEmptyFilePage(nStorageId, nAddr, nSize, objectID, objecttype, parentID, parenttype);
			}
		}
	}
}
