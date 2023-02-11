#pragma once

#include "../Transaction.h"
#include "../../utils/Int128.h"


namespace bptreedb
{
	namespace transaction
	{
		class CTransaction2PL : public IDBTransaction
		{
		public:
			CTransaction2PL(CommonLib::IAllocPtr ptrAlloc, int32_t storageId, int32_t nCacheSize, storage::IStorageCipherPtr ptrCipher,
				storage::IStoragesHolderPtr ptrStoragesHolder, const util::CUInt128& lsn, uint32_t userId, const char* pszNameUtf8, uint32_t nMinPageSize);

			CTransaction2PL(CommonLib::IAllocPtr ptrAlloc, int32_t storageId, int32_t nCacheSize, storage::IStorageCipherPtr ptrCipher,
				storage::IStoragesHolderPtr ptrStoragesHolder, const char* pszNameUtf8, uint32_t nMinPageSize);


			virtual ~CTransaction2PL();

			virtual void Commit();
			virtual void Rollback();
			virtual util::CUInt128 GetTransactionLSN() const;

			virtual ITransactionPagePtr ReadPage(uint32_t nStorageId, int64_t nAddr, uint32_t nSize);
			virtual void ReadPage(ITransactionPagePtr ptrPage, uint32_t nStorageId, int64_t nAddr);
			virtual void SavePage(ITransactionPagePtr ptrPage);
			virtual void DropPage(ITransactionPagePtr ptrPage);
			virtual int64_t GetNewFilePageAddr(uint32_t nStorageId, uint32_t nSize);
			virtual ITransactionPagePtr CreateNewPage(uint32_t nStorageId, int64_t nAddr, uint32_t nSize);

		private:
			int64_t GetAddressInLog(uint32_t nStorageId, int64_t nAddr);

			util::CUInt128 m_Lsn;
			CommonLib::IAllocPtr m_ptrAlloc;
			uint32_t m_UserId;

			struct SPageKey : public std::pair<uint32_t, int64_t>
			{
				SPageKey(uint32_t nStorageId, int64_t nAddr) :
					std::pair<uint32_t, int64_t>(nStorageId, nAddr)
				{}
			};


			typedef std::map<SPageKey, int64_t> TMapPageAddrToLog;
			TMapPageAddrToLog m_PagesAddr;


			storage::IStoragePtr m_ptrLogStorage;
			storage::IStoragesHolderPtr m_ptrStoragesHolder;
			
		};
	}
}