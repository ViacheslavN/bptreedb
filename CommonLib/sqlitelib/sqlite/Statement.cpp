#include "stdafx.h"
#include "Statement.h"
#include "SqliteExc.h"
#include "db/sqlite3.h"

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{
			CStatement::CStatement(sqlite3_stmt *pStmt) : m_pStmt(pStmt)
			{}

			CStatement::~CStatement()
			{
				sqlite3_finalize(m_pStmt);
			}

			void CStatement::ThrowError() const
			{
				sqlite3* pDb = sqlite3_db_handle(m_pStmt);
				throw CSqlitExc(pDb, sqlite3_errcode(pDb));
			}

			bool CStatement::Next()
			{
				int nRetVal = sqlite3_step(m_pStmt);
				if (nRetVal != SQLITE_ROW && nRetVal != SQLITE_DONE)
					throw CSqlitExc(m_pStmt, nRetVal);

				return nRetVal == SQLITE_ROW;
			}

			void CStatement::Reset()
			{
				int nRetVal = sqlite3_reset(m_pStmt);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			int  CStatement::ColumnCount() const
			{
				int nCol = sqlite3_column_count(m_pStmt);
				if (nCol == 0)
					ThrowError();

				return nCol;
			}

            std::string CStatement::ColumnName(int col) const
			{
				const char* pszName = sqlite3_column_name(m_pStmt, col);
				if (pszName == nullptr)
					ThrowError();

				return pszName;
			}

			bool CStatement::ColumnIsNull(int col) const
			{
				return sqlite3_column_type(m_pStmt, col) == SQLITE_NULL;
			}

			EDBFieldType CStatement::GetColumnType(int col) const
			{
				int iType = sqlite3_column_type(m_pStmt, col);
				switch (iType)
				{
				case SQLITE_INTEGER:
					return ftInt64_t;
				case SQLITE_FLOAT:
					return ftDouble;
				case SQLITE_TEXT:
					return ftString;
				case SQLITE_BLOB:
					return ftBlob;
				case SQLITE_NULL:
					return ftNull;
				}

				ThrowError();
				return ftNull;
			}

			int32_t CStatement::GetColumnBytes(int32_t col) const
			{
				return sqlite3_column_bytes(m_pStmt, col);
			}

			int16_t CStatement::ReadInt16(int col) const
			{
				return (int16_t)ReadInt32(col);
			}

			uint16_t CStatement::ReadUInt16(int col) const
			{
				return (uint16_t)ReadInt32(col);
			}

			int32_t CStatement::ReadInt32(int col) const
			{
				return sqlite3_column_int(m_pStmt, col);
			}

			uint32_t CStatement::ReadUInt32(int col) const
			{
				return (uint32_t)ReadInt32(col);
			}

			int64_t CStatement::ReadInt64(int32_t col) const
			{
				return sqlite3_column_int64(m_pStmt, col);
			}

			uint64_t CStatement::ReadUInt64(int32_t col) const
			{
				return (uint64_t)ReadInt64(col);
			}

			float CStatement::ReadFloat(int col) const
			{
				return (float)ReadDouble(col);
			}

			double CStatement::ReadDouble(int col) const
			{
				return sqlite3_column_double(m_pStmt, col);
			}

			void CStatement::ReadText(int col, std::string& text) const
			{
				int len = sqlite3_column_bytes(m_pStmt, col);
				const unsigned char *pText = sqlite3_column_text(m_pStmt, col);
				if (len == 0 || pText == nullptr)
					text.clear();
				else
					text = (const char*)pText;
			}

            std::string  CStatement::ReadText(int32_t col) const
			{
                std::string text;
				ReadText(col, text);

				return text;
			}

			void CStatement::ReadBlob(int col, byte_t **pBuf, int32_t& size) const
			{
				if (pBuf == 0)
					throw CExcBase("invalid argument, buf is null");

				size = sqlite3_column_bytes(m_pStmt, col);
				*pBuf = (byte_t*)sqlite3_column_blob(m_pStmt, col);
			}

			void CStatement::BindInt16(int32_t col, int16_t val)
			{
				int nRetVal = sqlite3_bind_int(m_pStmt, col, val);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindUInt16(int32_t col, uint16_t val)
			{
				int nRetVal = sqlite3_bind_int(m_pStmt, col, val);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindInt32(int32_t col, int32_t val)
			{
				int nRetVal = sqlite3_bind_int(m_pStmt, col, val);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindUInt32(int32_t col, uint32_t val)
			{
				int nRetVal = sqlite3_bind_int(m_pStmt, col, val);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindInt64(int32_t col, int64_t val)
			{
				int nRetVal = sqlite3_bind_int64(m_pStmt, col, val);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindUInt64(int32_t col, uint64_t val)
			{
				int nRetVal = sqlite3_bind_int64(m_pStmt, col, val);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindFloat(int32_t col, float val)
			{
				int nRetVal = sqlite3_bind_double(m_pStmt, col, val);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindDouble(int32_t col, double val)
			{
				int nRetVal = sqlite3_bind_double(m_pStmt, col, val);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindText(int32_t col, const std::string& text, bool copy)
			{
				int nRetVal = sqlite3_bind_text(m_pStmt, col, text.c_str(), (int)text.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}

			void CStatement::BindBlob(int32_t col, const byte_t *pBuf, int32_t size, bool copy)
			{
				int nRetVal = sqlite3_bind_blob(m_pStmt, col, pBuf, size, copy ? SQLITE_TRANSIENT : SQLITE_STATIC);
				if (nRetVal != SQLITE_OK)
					throw CSqlitExc(m_pStmt, nRetVal);
			}
			
		}
	}
}