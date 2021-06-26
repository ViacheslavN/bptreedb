#include "stdafx.h"

#include "TransactionBase.h"

namespace bptreedb
{
	namespace transaction
	{
		CTransactonBase::CTransactonBase(const CommonLib::CGuid& guid, eTransactionDataType type) : m_guid(guid), m_type(type), m_bError(false)
		{

		}

		const CommonLib::CGuid& CTransactonBase::GetId() const
		{
			return m_guid;
		}


		bool CTransactonBase::IsError() const
		{
			return m_bError;
		}

		void CTransactonBase::AddStorage(IStoragePtr ptrStorage)
		{
			m_Storages.insert(std::make_pair(ptrStorage->GetStorageId(), ptrStorage));
		}

		void CTransactonBase::AddInnerTransactions(IDBTransactionPtr ptrTransaction)
		{
			m_innerTransactions.push_back(ptrTransaction);
		}

		eTransactionDataType CTransactonBase::GetType() const
		{
			return m_type;
		}
	}
}