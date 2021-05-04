#pragma once

namespace bptreedb
{
	namespace transaction
	{
		namespace wal
		{
			class CTransactionLog
			{
			public:
				CTransactionLog(const astr& logFilePach, uint32_t pageSize, uint32_t pagesInBlock);
				~CTransactionLog();

				CTransactionLog(const CTransactionLog&) = delete;
				CTransactionLog& operator=(const CTransactionLog&) = delete;
				CTransactionLog(CTransactionLog&&) = delete;
				CTransactionLog& operator=(const CTransactionLog&&) = delete;

				int64_t GetFreeBlock();
				void DropFreeBlock(int64_t blockId);

				void WriteFilePage(FilePagePtr ptrPage);
				void ReadFilePage(FilePagePtr ptrPage);


			private:
				const uint32_t m_transaction_block;
				typedef std::vector<byte_t> TBufferForChiper;
				TBufferForChiper m_bufForChiper;
				CommonLib::TPrefCounterPtr m_pStoragePerformer;
				CommonLib::file::CFile m_file;
			};
		}
	}
}