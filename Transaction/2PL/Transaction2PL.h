#pragma once

#include "../Transaction.h"
#include "../../utils/Int128.h"
#include "../../utils/CacheLRU_2Q.h"

namespace bptreedb
{
	namespace transaction
	{
		class CTransaction2PL : public IDBTransaction
		{
			enum TransactionState
			{
				eInitState = 0,
				eSavePages = 1,
				eStartCommit = 2
			};


		public:


			CTransaction2PL(CommonLib::IAllocPtr ptrAlloc, int32_t storageId, int32_t nCacheSize, storage::IStorageCipherPtr ptrCipher,
				storage::IStoragesHolderPtr ptrStoragesHolder, const util::CUInt128& lsn, const util::CUInt128& prevLsn, uint32_t userId, const char* pszNameUtf8, uint32_t nMinPageSize);

			CTransaction2PL(CommonLib::IAllocPtr ptrAlloc, int32_t storageId, int32_t nCacheSize, storage::IStorageCipherPtr ptrCipher,
				storage::IStoragesHolderPtr ptrStoragesHolder, const char* pszNameUtf8, uint32_t nMinPageSize);

			virtual ~CTransaction2PL();

			virtual void Commit();
			virtual void Rollback();
			virtual util::CUInt128 GetTransactionLSN() const;

			virtual uint32_t GetPageSize() const;
			virtual storage::IFilePagePtr ReadPage(int32_t nStorageId, int64_t nAddr);
			virtual void ReadPage(storage::IFilePagePtr ptrPage, int32_t nStorageId, int64_t nAddr);
			virtual void SavePage(storage::IFilePagePtr ptrPage, int32_t nStorageId);
			virtual void DropPage(storage::IFilePagePtr ptrPage, int32_t nStorageId);
			virtual int64_t GetNewFilePageAddr(int32_t nStorageId);
			virtual storage::IFilePagePtr CreateNewPage(int32_t nStorageId, int64_t nAddr);

		private:
			void StartTransaction();
			void SavePageForUndo(storage::IFilePagePtr ptrPage);


			int64_t GetAddressInLog(int32_t nStorageId, int64_t nAddr);
			void AddAddressInLog(int32_t nStorageId, int64_t nAddr, int64_t nAddrInLog);
			void SaveHeader(TransactionState state);

			void StartCommit();
			void SaveUndoPageAddrs();
			void SaveChangedPageAddrs();
			void CommitToStorage(std::vector<int64_t>& addrs, int64_t nStorageId, std::vector<storage::IFilePagePtr>& cachePages);

			util::CUInt128 m_Lsn;
			util::CUInt128 m_PrevLsn;
			CommonLib::IAllocPtr m_ptrAlloc;
			uint32_t m_UserId;
			
			typedef std::map<int64_t, int64_t> TMapPageAddr;
			typedef std::map<int64_t, TMapPageAddr> TMapPageAddrbyStorage;
			typedef std::vector<int64_t> TPagesForUndo;
			TMapPageAddrbyStorage m_PagesAddr;
			TPagesForUndo	  m_PagesForUndo;

			const int64_t m_nTranInfoStreamAddr = 0;
			int64_t m_nUndoStreamAddr{ -1 };
			int64_t m_nChangedPageStreamAddr{ -1 };

			storage::IStoragePtr m_ptrLogStorage;
			storage::IStoragesHolderPtr m_ptrStoragesHolder;
			utils::TCacheLRU_2Q<int64_t, ITransactionPagePtr> m_pageCache;
			
		};
	}
}