#pragma once

#include "../storage/Storage.h"
#include "../utils/Int128.h"

namespace bptreedb
{
	namespace transaction
	{
		typedef std::shared_ptr<class CTransactionFilePager> CTransactionFilePagerPtr;

		class CTransactionFilePager : public storage::IFilePager
		{
		public:
			CTransactionFilePager(storage::IStoragePtr ptrStorage, util::CUInt128 lsn, CommonLib::IAllocPtr ptrAlloc, uint32_t userId);
			virtual ~CTransactionFilePager();

			virtual int64_t GetNewFilePageAddr();
			virtual storage::IFilePagePtr GetNewPage(int64_t nAddr);
			virtual storage::IFilePagePtr ReadPage(int64_t nAddr);
			virtual void ReadPage(storage::IFilePagePtr ptrPage, int64_t nAddr);
			virtual void SavePage(storage::IFilePagePtr ptrPage);

		private:
			storage::IStoragePtr m_ptrStorage;
			util::CUInt128 m_Lsn;
			CommonLib::IAllocPtr m_ptrAlloc;
			uint32_t m_UserId;
		};

	}
}