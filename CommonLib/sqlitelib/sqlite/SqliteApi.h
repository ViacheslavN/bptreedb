#pragma once

#include "../Database.h"

struct sqlite3;
struct sqlite3_stmt;

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{

			typedef std::shared_ptr<class CSQliteApi> CSQliteApiPtr;

			class CSQliteApi
			{
			public:
				CSQliteApi(sqlite3* pDB);
				virtual ~CSQliteApi();

				sqlite3_stmt* PrepareQuery(const char *pszQuery);
				void Execute(const char *pszQuery);
				sqlite3* GetDB();			
			private:
				sqlite3* m_pDB;
	
			};

		}
	}
}