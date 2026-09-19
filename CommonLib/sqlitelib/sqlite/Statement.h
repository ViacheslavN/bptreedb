#pragma once

struct sqlite3_stmt;
#include "../Database.h"

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{

			class CStatement : public IStatment
			{
			public:
				CStatement(sqlite3_stmt *pStmt);
				~CStatement();

				bool Next();
				void Reset();

				int32_t  ColumnCount() const override;
                std::string ColumnName(int32_t col) const override;
				bool ColumnIsNull(int32_t col) const override;
				EDBFieldType GetColumnType(int32_t col) const override;
				int32_t GetColumnBytes(int32_t col) const override;

				int16_t ReadInt16(int32_t col) const override;
				uint16_t ReadUInt16(int32_t col) const override;
				int32_t ReadInt32(int32_t col) const override;
				uint32_t ReadUInt32(int32_t col) const override;
				int64_t ReadInt64(int32_t col) const override;
				uint64_t ReadUInt64(int32_t col) const override;
				float ReadFloat(int32_t col) const override;
				double ReadDouble(int32_t col) const override;
				void ReadText(int32_t col, std::string& text) const override;
                std::string ReadText(int32_t col) const override;
				void ReadBlob(int col, byte_t **pBuf, int32_t& size) const override;

				void BindInt16(int32_t col, int16_t nVal) override;
				void BindUInt16(int32_t col, uint16_t nVal) override;
				void BindInt32(int32_t col, int32_t nVal) override;
				void BindUInt32(int32_t col, uint32_t nVal) override;
				void BindInt64(int32_t col, int64_t nVal) override;
				void BindUInt64(int32_t col, uint64_t nVal) override;
				void BindFloat(int32_t col, float val) override;
				void BindDouble(int32_t col, double val) override;
				void BindText(int32_t col, const std::string& text, bool copy) override;
				void BindBlob(int32_t col, const byte_t *pBuf, int32_t size, bool copy) override;

			private:
				void ThrowError() const;

			private:
				sqlite3_stmt *m_pStmt;
			};
		}
		
	}
}