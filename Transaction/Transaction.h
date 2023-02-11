#pragma once
 
#include "../BPTreeDatabase/SDK/bptreedb.h"
#include "../utils/Int128.h"
#include "../storage/FilePage.h"

namespace bptreedb
{
	namespace transaction
	{
		typedef std::shared_ptr<class ITransactionPage> ITransactionPagePtr;
		typedef std::shared_ptr<class IDBTransaction> IDBTransactionPtr;

		class ITransactionPage : public storage::IFilePage
		{
		public:
			ITransactionPage() {}
			virtual ~ITransactionPage() {}

			virtual uint32_t GetStorageId() const = 0;
			virtual uint32_t GetObjectID() const = 0;
			virtual util::CUInt128 GetTransactionLSN() const = 0;
			virtual uint64_t GetDate() const = 0;
			virtual int32_t GetUserId() const = 0;
 
		};

		class IDBTransaction : public ITransaction
		{
		public:
			IDBTransaction() {};
			virtual ~IDBTransaction(){}

			virtual void Commit() = 0;
			virtual void Rollback() = 0;
			virtual util::CUInt128 GetTransactionLSN() const = 0;

			virtual ITransactionPagePtr ReadPage(uint32_t nStorageI, int64_t nAddr, uint32_t nSize) = 0;
			virtual void ReadPage(ITransactionPagePtr ptrPage, uint32_t nStorageId, int64_t nAddr) = 0;
			virtual void SavePage(ITransactionPagePtr ptrPage) = 0;
			virtual void DropPage(ITransactionPagePtr ptrPage) = 0;
			virtual int64_t GetNewFilePageAddr(uint32_t nStorageId, uint32_t nSize) = 0;
			virtual ITransactionPagePtr CreateNewPage(uint32_t nStorageId, int64_t nAddr, uint32_t nSize) = 0;
		};
	}
}