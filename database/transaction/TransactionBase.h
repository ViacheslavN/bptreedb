#pragma once

#include "Transaction.h"
 

namespace bptreedb
{
	namespace transaction
	{
		class CTransactonBase : public IDBTransaction
		{
		public:
			CTransactonBase(const CommonLib::CGuid& guid, eTransactionDataType type, IFileStoragesHolderPtr ptrStoragesHolder);
			~CTransactonBase() = default;

			//ITransaction
			virtual eTransactionDataType GetType() const;
			const CommonLib::CGuid& GetId() const;
			virtual bool IsError() const;

			//IDBTransaction
			virtual void AddInnerTransactions(IDBTransactionPtr ptrTransaction);

		private:
			typedef std::map<int32_t, IStoragePtr> TMapStorages;
			typedef std::vector<IDBTransactionPtr> TVecInnerTransactions;

			TMapStorages m_Storages;
			TVecInnerTransactions m_innerTransactions;

			CommonLib::CGuid m_guid;
			eTransactionDataType m_type;
			bool m_bError;
			IFileStoragesHolderPtr m_ptrFileStorageHolder;
		};
	}
}