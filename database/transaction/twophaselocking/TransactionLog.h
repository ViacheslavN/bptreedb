#pragma once


namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			class CTransactionLog
			{
			public:
				CTransactionLog(std::shared_ptr<CommonLib::IAlloc> pAlloc);
				~CTransactionLog();

				void Open(const astr& fileName);
				void Close();
				void Flush();

				FilePagePtr GetFilePage(int64_t nAddr, uint32_t nSize, bool decrypt);
				void SaveFilePage(FilePagePtr& ptrPage);
				void DropFilePage(int64_t nAddr);
				FilePagePtr GetNewFilePage(uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize = 0);
				int64_t GetNewFilePageAddr(uint32_t nSize = 0);
				FilePagePtr GetEmptyFilePage(int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype);

			private:
				CommonLib::file::CFile m_file;
				IPageCipherPtr m_pageCipher;
				std::shared_ptr<CommonLib::IAlloc> m_pAlloc
			};
		}

	}
}