#include "stdafx.h"
#include "Transaction.h"
#include "SqliteExc.h"

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{
			CTransaction::CTransaction(CSQliteApiPtr ptrDB, bool isReadOnly) : m_ptrDB(ptrDB), m_IsReadOnly(isReadOnly)
			{

			}

			CTransaction::~CTransaction()
			{
				if (m_start)
				{
					try
					{
						m_start = false;
						RollbackImpl();
					}
					catch (std::exception& exc)
					{
						exc;
					}
				}
			}

			void CTransaction::Begin()
			{
				if(m_IsReadOnly)
					m_ptrDB->Execute("BEGIN");
				else
					m_ptrDB->Execute("BEGIN IMMEDIATE");

				m_start = true;
			}

			void CTransaction::Commit()
			{
				m_start = false;
				m_ptrDB->Execute("COMMIT");
			}

			void CTransaction::Rollback()
			{
				RollbackImpl();
			}

			void CTransaction::RollbackImpl()
			{
				m_start = false;
				m_ptrDB->Execute("ROLLBACK");
			}
		}
	}
}