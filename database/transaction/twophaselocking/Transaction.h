#pragma once

#include "../Transaction.h"

namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			class CTransaction : public IDBTransaction
			{
			public:

				CTransaction();
				~CTransaction();

				//ITransaction
				virtual eTransactionDataType GetType() const;
				virtual void Begin();
				virtual void Commit();
				virtual void Rollback();
				virtual bool IsError() const;
				virtual CommonLib::CGuid GetId() const;

				virtual IStatementPtr CreateStatement(const astr& sSQLQuery);
				virtual ICursorPtr ExecuteQuery(IStatement* pStatement);

				virtual ICursorPtr ExecuteSelectQuery(const astr& sTable, const std::vector<astr>& fileds);
				virtual ICursorPtr ExecuteSelectQuery(const astr& sTable, const std::vector<astr>& fileds, const astr& sField, IValuePtr ptrVal, eOpType opType);

				virtual IInsertCursorPtr CreateInsertCursor(const astr& sTable, const std::vector<astr>& fileds);
				virtual IUpdateCursorPtr CreateUpdateCursor();
				virtual IDeleteCursorPtr CreateDeleteCursor(const astr& sTable);

				//IDBTransaction
				virtual FilePagePtr GetFilePage(int64_t nAddr, uint32_t nSize, bool decrypt);
				virtual void SaveFilePage(FilePagePtr& ptrPage);
				virtual void DropFilePage(int64_t nAddr);
				virtual FilePagePtr GetNewFilePage(uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize = 0);
				virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0);
				virtual FilePagePtr GetEmptyFilePage(int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype);

				virtual void AddInnerTransactions(IDBTransactionPtr ptrTransaction);
				virtual FilePagesPtr GetFilePagesForStorage(IStoragePtr ptrStorage);


			private:
			};
		}
	}
}