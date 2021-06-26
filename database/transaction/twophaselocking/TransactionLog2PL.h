#pragma once
#include "../../../storage/FileStorage.h"
 
namespace bptreedb
{
	namespace transaction
	{
		namespace twoPL
		{
			class CTransactionLog
			{

				enum ePageFlags
				{
					eFP_NEW = 1,
					eFP_CHANGE = 2,
					eFP_EMPTY = 4,
					eFP_REMOVE = 8,
					eFP_TRANLOG_PAGES = 16,
				};

			public:
				CTransactionLog(CommonLib::IAllocPtr ptrAlloc, bool bCheckCRC);
				~CTransactionLog();

				void Open(const astr& fileName, bool bCreate, uint32_t nMinPageSize = 8192);
				void Close();
				void Flush();

				FilePagePtr GetFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, bool decrypt);
				void SaveFilePage(int32_t nStorageId, FilePagePtr& ptrPage);
				void DropFilePage(int32_t nStorageId, int64_t nAddr);
				FilePagePtr GetNewFilePage(int32_t nStorageId, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize = 0);
				int64_t GetNewFilePageAddr(int32_t nStorageId, uint32_t nSize = 0);
				FilePagePtr GetEmptyFilePage(int32_t nStorageId, int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype);

			private:
				CommonLib::IAllocPtr m_ptrAlloc;
				CFileStoragePtr m_ptrFileStorage;


				struct SPageInfo
				{
					int64_t m_nAddr;
					uint32_t m_flags;

					int64_t m_nOrignAddr;
				};

				class CPageKey : public std::pair<int32_t, int64_t>
				{
				public:
					CPageKey(int32_t storageId, int64_t nAddr) : std::pair<int32_t, int64_t>(storageId, nAddr)
					{}

					int32_t GetStorageId() const
					{
						return first;
					}

					int64_t GetAddr() const
					{
						return second;
					}
				};

				typedef std::map<CPageKey, SPageInfo> TMapPages;
				TMapPages m_pages;

			};

			typedef std::shared_ptr< CTransactionLog> CTransactionLogPtr;
		}

	}
}