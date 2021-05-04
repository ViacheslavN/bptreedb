#pragma once

#include "../Transaction.h"

namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			class CTransactionManager : public IDBTransactionManager
			{
			public:
				CTransactionManager() {}
				virtual ~CTransactionManager() {}

				virtual IDBTransactionPtr StartTransaction();
				virtual CloseTransaction(IDBTransactionPtr ptrIDBTransaction);
				virtual void Restore(IStoragePtr ptrStorage);
			};
		}
	}
}