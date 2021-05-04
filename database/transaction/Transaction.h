#pragma once

#include "../include/bptreedb.h"
#include "../../storage/storage.h"
 

namespace bptreedb
{
	namespace transaction
	{

		typedef std::shared_ptr<class IDBTransaction> IDBTransactionPtr;
		typedef std::shared_ptr<class IDBTransactionManager> IDBTransactionManagePtr;

		class IDBTransaction : public ITransaction
		{
		public:
			IDBTransaction(){}
			virtual ~IDBTransaction(){}

			//storage
			virtual FilePagePtr GetFilePage(int64_t nAddr, uint32_t nSize, bool decrypt) = 0;
			virtual void SaveFilePage(FilePagePtr& ptrPage) = 0;
			virtual void DropFilePage(int64_t nAddr) = 0;
			virtual FilePagePtr GetNewFilePage(uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize = 0) = 0;
			virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0) = 0;
			virtual FilePagePtr GetEmptyFilePage(int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype) = 0;

			virtual void AddInnerTransactions(IDBTransactionPtr ptrTransaction) = 0;
			virtual FilePagesPtr GetFilePagesForStorage(IStoragePtr ptrStorage) = 0;
		};


		class IDBTransactionManager
		{
		public:
			IDBTransactionManager() {}
			virtual ~IDBTransactionManager(){}

			virtual IDBTransactionPtr StartTransaction() = 0;
			virtual void CloseTransaction(IDBTransactionPtr ptrIDBTransaction) = 0;
			virtual void Restore(IStoragePtr ptrStorage) = 0;

		};

	}
}