#pragma once

#include "../TransactionBase.h"
#include "TransactionLog2PL.h"

namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			class CTransaction : public CTransactonBase
			{
			public:

				CTransaction(const astr& transactionLogFile, const CommonLib::CGuid& guid, eTransactionDataType type, CommonLib::IAllocPtr ptrAlloc, bool bCheckCRC);
				~CTransaction();

				//ITransaction

				virtual void Begin();
				virtual void Commit();
				virtual void Rollback();


				virtual IStatementPtr CreateStatement(const astr& sSQLQuery);
				virtual ICursorPtr ExecuteQuery(IStatementPtr pStatement);

				virtual ICursorPtr ExecuteSelectQuery(const astr& sTable, const std::vector<astr>& fileds);
				virtual ICursorPtr ExecuteSelectQuery(const astr& sTable, const std::vector<astr>& fileds, const astr& sField, IValuePtr ptrVal, eOpType opType);

				virtual IInsertCursorPtr CreateInsertCursor(const astr& sTable, const std::vector<astr>& fileds);
				virtual IUpdateCursorPtr CreateUpdateCursor();
				virtual IDeleteCursorPtr CreateDeleteCursor(const astr& sTable);

				//IDBTransaction
				virtual FilePagePtr GetFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, bool decrypt);
				virtual void SaveFilePage(int32_t nStorageId, FilePagePtr& ptrPage);
				virtual void DropFilePage(int32_t nStorageId, int64_t nAddr);
				virtual FilePagePtr GetNewFilePage(int32_t nStorageId, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize = 0);
				virtual int64_t GetNewFilePageAddr(int32_t nStorageId, uint32_t nSize = 0);
				virtual FilePagePtr GetEmptyFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype);


			private:
				astr m_transactionLogFile;
				CTransactionLogPtr m_ptrTransactionLog;
			};
		}
	}
}