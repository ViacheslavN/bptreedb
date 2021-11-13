#include "stdafx.h"
#include "TransactionManager.h"
#include "Transaction.h"

namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{

			CTransactionManager::CTransactionManager(const astr& path, CommonLib::IAllocPtr ptrAlloc, bool bCheckCRC, IFileStoragesHolderPtr ptrStoragesHolder) : m_path(path),
				m_ptrAlloc(ptrAlloc), m_bCheckCRC(bCheckCRC), m_ptrStoragesHolder(ptrStoragesHolder)
			{

			}

			CTransactionManager::~CTransactionManager()
			{

			}

			IDBTransactionPtr CTransactionManager::StartTransaction(eTransactionDataType type)
			{
				try
				{
					CommonLib::CGuid tranGuid = CommonLib::CGuid::CreateNew();
					astr tranLog = CommonLib::str_format::StrFormatSafe("%1\\%2", m_path, tranGuid.ToAstr(false));

					IDBTransactionPtr ptrTransaction(new CTransaction(tranLog, tranGuid, type, m_ptrAlloc, m_bCheckCRC, m_ptrStoragesHolder));
					return ptrTransaction;
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionManager: Failed to start transaction", excSrc);
					throw;
				}
			} 

			void CTransactionManager::CloseTransaction(IDBTransactionPtr ptrIDBTransaction)
			{

			}

			void CTransactionManager::Restore(IStoragePtr ptrStorage)
			{
				try
				{
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("TransactionManager: Failed to restore", excSrc);
					throw;
				}
			}
		}
	}
}