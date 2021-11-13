#pragma once

#include "../include/bptreedb.h"
#include "../../storage/storage.h"
 

namespace bptreedb
{
	namespace transaction
	{

		typedef std::shared_ptr<class IDBTransaction> IDBTransactionPtr;
		typedef std::shared_ptr<class IDBTransactionManager> IDBTransactionManagePtr;
		typedef std::shared_ptr<class IDBStorageManager> IDBStorageManagerPtr;

		class IDBTransaction : public ITransaction
		{
		public:
			IDBTransaction(){}
			virtual ~IDBTransaction(){}

			//storage
			virtual FilePagePtr GetFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, bool decrypt) = 0;
			virtual void SaveFilePage(int32_t nStorageId, FilePagePtr& ptrPage) = 0;
			virtual void DropFilePage(int32_t nStorageId, int64_t nAddr) = 0;
			virtual FilePagePtr GetNewFilePage(int32_t nStorageId, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize = 0) = 0;
			virtual int64_t GetNewFilePageAddr(int32_t nStorageId, uint32_t nSize = 0) = 0;
			virtual FilePagePtr GetEmptyFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype) = 0;

			virtual void AddInnerTransactions(IDBTransactionPtr ptrTransaction) = 0;
		};


		class IDBTransactionManager
		{
		public:
			IDBTransactionManager() {}
			virtual ~IDBTransactionManager(){}

			virtual IDBTransactionPtr StartTransaction(eTransactionDataType type) = 0;
			virtual void CloseTransaction(IDBTransactionPtr ptrIDBTransaction) = 0;
			virtual void Restore(IStoragePtr ptrStorage) = 0;

		};

		class IDBStrorageManager
		{
		public:
			IDBStrorageManager(){}
			virtual ~IDBStrorageManager();

			virtual IStoragePtr GetStorage(int32_t storageId) const = 0;
			virtual void AddStorage(IStoragePtr pIStorage) = 0;
		};

	}
}