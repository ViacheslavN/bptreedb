#include "stdafx.h"
#include "Database.h"
#include "SqliteExc.h"
#include "Statement.h"
#include "db/sqlite3.h"
#include "filesys/SqliteVfs.h"
#include "../../str/str.h"
#include "CryptoContextHolder.h"
#include "Transaction.h"

namespace CommonLib
{
	namespace database
	{
		IDatabasePtr IDatabaseSQLiteCreator::Create(const char *pszFile, uint32_t flags)
		{
			int mode = 0;

			if (flags & ReadOnlyMode)
				mode |= SQLITE_OPEN_READONLY;
			else
				mode |= SQLITE_OPEN_READWRITE;

			if (flags & CreateDatabase)
				mode |= SQLITE_OPEN_CREATE;

			if (flags & MultithreadMode)
				mode |= SQLITE_OPEN_FULLMUTEX;
			else
				mode |= SQLITE_OPEN_NOMUTEX;

			if (flags & WAL)
				mode |= SQLITE_OPEN_WAL;

			sqlite3* pDB = nullptr;

			ICryptoContextPtr ptrCryptoContext = CCryptoContextHolder::Instance().GetCryptoContext(pszFile);

			if (ptrCryptoContext.get() != nullptr)
			{
				int retVal = sqlite::CVfs::VfsCreate(NULL, 1);
				if (retVal != SQLITE_OK)
					throw sqlite::CSqlitExc(retVal);
			}
			//https://www.sqlite.org/src/doc/trunk/src/test_demovfs.c
			//https://www.sqlite.org/vfs.html
			int retVal = sqlite3_open_v2(pszFile, &pDB, mode, 0);
			if (retVal != SQLITE_OK)
				throw sqlite::CSqlitExc(retVal);

			IDatabasePtr ptrDatabase = IDatabasePtr(new sqlite::CDatabase(pDB, flags & ReadOnlyMode ? true : false));

			if (flags & WAL)
				ptrDatabase->Execute("PRAGMA journal_mode=WAL");

			return ptrDatabase;
		}

		namespace sqlite
		{		


			CDatabase::CDatabase(sqlite3* pDB, bool readOnly) :  m_readOnly(readOnly)
			{
				m_ptrDBApi.reset(new CSQliteApi(pDB));
			}

			CDatabase::~CDatabase()
			{
			 
			}

			IStatmentPtr CDatabase::PrepareQuery(const char *pszQuery) const
			{
				sqlite3_stmt* pStmt = m_ptrDBApi->PrepareQuery(pszQuery);
				return std::shared_ptr<IStatment>(new CStatement(pStmt));
			}

			void CDatabase::Execute(const char *pszQuery)
			{
				m_ptrDBApi->Execute(pszQuery);
			}

			int32_t CDatabase::GetChanges() const noexcept
			{
				return sqlite3_changes(m_ptrDBApi->GetDB());
			}

			int64_t CDatabase::GetLastInsertRowID() const noexcept
			{
				return sqlite3_last_insert_rowid(m_ptrDBApi->GetDB());
			}

			bool CDatabase::IsReadOnly() const noexcept
			{
				return m_readOnly;
			}

			bool CDatabase::IsTableExists(const char *pszTable) const
			{
				if (pszTable == nullptr)
					throw CExcBase("Invalid argument, table name is null");

                std::string stm = str_format::StrFormatSafe("SELECT name FROM sqlite_master WHERE type = 'table' AND name = '{0}'", pszTable);
				IStatmentPtr ptrStatemnt = PrepareQuery(stm.c_str());
				return ptrStatemnt->Next();
			}

			void CDatabase::SetBusyTimeout(int ms) noexcept
			{
				sqlite3_busy_timeout(m_ptrDBApi->GetDB(), ms);
			}
			
			ITransactionPtr CDatabase::CreateTransaction()
			{
				return std::shared_ptr<ITransaction>(new CTransaction(m_ptrDBApi, m_readOnly));
			}

		}
	}
}