#pragma once

#include "../Transaction.h"
#include "../../../storage/FileStorage.h"

namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			class CTransactionManager : public IDBTransactionManager
			{
			public:
				CTransactionManager(const astr& path, CommonLib::IAllocPtr ptrAlloc, bool bCheckCRC, IFileStoragesHolderPtr ptrStoragesHolder, 
					uint64_t nTransactionId);
				virtual ~CTransactionManager();

				virtual IDBTransactionPtr StartTransaction(eTransactionDataType type);
				virtual void CloseTransaction(IDBTransactionPtr ptrIDBTransaction);
				virtual void Restore(IStoragePtr ptrStorage);
			private:
				astr m_path;
				CommonLib::IAllocPtr m_ptrAlloc;
				bool m_bCheckCRC;
				IFileStoragesHolderPtr m_ptrStoragesHolder;
				uint64_t m_nTransactionId;

			};
		}
	}
}