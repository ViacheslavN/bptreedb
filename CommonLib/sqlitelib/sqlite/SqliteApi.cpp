#include "stdafx.h"
#include "SqliteApi.h"
#include "SqliteExc.h"
#include "Statement.h"
#include "db/sqlite3.h"
#include "../../str/str.h"

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{
			CSQliteApi::CSQliteApi(sqlite3* pDB) : m_pDB(pDB)
			{

			}

			CSQliteApi::~CSQliteApi()
			{
				const int nRetVal = sqlite3_close(m_pDB);
				if (nRetVal != SQLITE_OK)
				{
					//TODO log
				}
			}

			sqlite3_stmt* CSQliteApi::PrepareQuery(const char *pszQuery)
			{
				sqlite3_stmt* pStmt = 0;
				const int nRetVal = sqlite3_prepare_v2(m_pDB, pszQuery, -1, &pStmt, 0);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pDB, nRetVal);

				return pStmt;
			}

			void CSQliteApi::Execute(const char *pszQuery)
			{
				const int nRetVal = sqlite3_exec(m_pDB, pszQuery, 0, 0, 0);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pDB, nRetVal);
			}

			sqlite3* CSQliteApi::GetDB()
			{
				return m_pDB;
			}
		}
	}
}