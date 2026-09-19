#include "stdafx.h"
#include "SqliteExc.h"
#include "db/sqlite3.h"

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{

			CSqlitExc::CSqlitExc(sqlite3* db, int sqliteErr) : m_sqliteErr(sqliteErr)
			{
				AddMessages(db, m_sqliteErr);
			}

			CSqlitExc::CSqlitExc(sqlite3_stmt* stmt, int sqliteErr) : m_sqliteErr(sqliteErr)
			{
				sqlite3* db = sqlite3_db_handle(stmt);
				AddMessages(db, m_sqliteErr);
			}

			void CSqlitExc::AddMessages(sqlite3* db, int sqliteErr)
			{
				CExcBase::AddMsg(GetErrorDesc(sqliteErr));
				int extCode = sqlite3_extended_errcode(db);
				const char *pszMessage = sqlite3_errmsg(db);

				if (pszMessage != nullptr)
					CExcBase::AddMsg(pszMessage);

				if (extCode != sqliteErr)
					CExcBase::AddMsg(GetErrorDesc(extCode));
			}

			CSqlitExc::CSqlitExc(int sqliteErr) : m_sqliteErr(sqliteErr)
			{
				CExcBase::AddMsg(GetErrorDesc(m_sqliteErr));
			}

			CSqlitExc::CSqlitExc(const std::string& err_msg, int sqliteErr) : m_sqliteErr(sqliteErr)
			{
				CExcBase::AddMsg(err_msg);
				CExcBase::AddMsg(GetErrorDesc(sqliteErr));
			}

			CSqlitExc::~CSqlitExc()
			{

			}

			std::shared_ptr<CExcBase> CSqlitExc::Clone() const
			{

				std::shared_ptr<CExcBase> ptrExc(new CExcBase(*this));
				return ptrExc;
			}

            std::string CSqlitExc::GetErrorDesc(int sqliteErr)
			{
				const char * pszError = sqlite3_errstr(sqliteErr);
				if (pszError != nullptr)
					return pszError;

				return "Unknown error";
			}
		}
		
	}
}