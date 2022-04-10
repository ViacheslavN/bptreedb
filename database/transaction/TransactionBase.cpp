#include "stdafx.h"
#include "TransactionBase.h"

namespace bptreedb
{
	namespace transaction
	{
		CTransactonBase::CTransactonBase(const CommonLib::CGuid& guid, eTransactionDataType type, IFileStoragesHolderPtr ptrFileStoragesHolder) : m_guid(guid), m_type(type), m_bError(false),
			m_ptrFileStorageHolder(ptrFileStoragesHolder)
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