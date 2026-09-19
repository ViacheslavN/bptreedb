#pragma once

#include "../../exception/exc_base.h"
 
struct sqlite3;
struct sqlite3_stmt;

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{

			class CSqlitExc : public CExcBase
			{
			public:
				CSqlitExc(sqlite3* db, int sqliteErr);
				CSqlitExc(sqlite3_stmt* stmt, int sqliteErr);
				CSqlitExc(int sqliteErr);
				CSqlitExc(const std::string& err_msg, int sqliteErr);

				template<typename... Types>
				CSqlitExc(int sqliteErr, const std::string& format, Types... args) : CExcBase(format, args...), m_sqliteErr(sqliteErr)
				{
					CExcBase::AddMsg(GetErrorDesc(sqliteErr));
				}

				~CSqlitExc() noexcept;
				virtual std::shared_ptr<CExcBase> Clone() const;

				static std::string GetErrorDesc(int sqliteErr);

			private:
				void AddMessages(sqlite3* db, int sqliteErr);

			private:
				int m_sqliteErr;
			};

		}
	}
}