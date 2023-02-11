#pragma once
#include "../../CommonLib/filesystem/filesystem.h"
#include "FilePage.h"
#include "Storage.h"
#include "../utils/CacheLRU_2Q.h"


namespace bptreedb
{
	namespace storage
	{
		class CFileStorage : public IStorage
		{
		private:
			CFileStorage(const CFileStorage&);
			CFileStorage(const CFileStorage&&);
			CFileStorage& operator = (const CFileStorage&);
			CFileStorage& operator = (const CFileStorage&&);

		public:
			CFileStorage(CommonLib::IAllocPtr ptrAlloc, int32_t storageId, int32_t nCacheSize, IStorageCipherPtr ptrCipher);
			virtual ~CFileStorage();

			virtual void Open(const char* pszNameUtf8, bool bCreate, uint64_t offset, uint32_t nMinPageSize = 8192);
			virtual void Open(const wchar_t* pszName, bool bCreate, uint64_t offset, uint32_t nMinPageSize = 8192);
			virtual void Close();
			virtual void ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize);
			virtual void WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize);
			virtual void DropData(int64_t nAddr, uint32_t nSize);
			virtual void Flush();
			virtual void SetStoragePerformer(CommonLib::TPrefCounterPtr pStoragePerformer);
			virtual int32_t GetStorageId() const;
			virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0);

		private:

			void _ReadData(int64_t nAddr, byte_t* pData);
			void _WriteData(int64_t nAddr, const byte_t* pData);
			void _DeleteData(int64_t nAddr);

			const uint32_t MIN_PAGE_SIZE = 8192;

			CommonLib::IAllocPtr m_ptrAlloc;

			int32_t m_cacheSize{ 256*1024 };
			int32_t m_storage_id{ -1 };
			uint32_t m_minPageSize{ MIN_PAGE_SIZE };
			int64_t m_lastAddr{ 0 };
			uint64_t m_offset{ 0 };
			IStorageCipherPtr m_ptrStorageCipher;
			CommonLib::file::CFile m_file;


			typedef std::vector<byte_t> TBufferForChiper;
			TBufferForChiper m_bufForChiper;
			CommonLib::TPrefCounterPtr m_ptrStoragePerformer;
			mutable std::recursive_mutex m_mutex;


			enum EFilePageState
			{
				CLEAN,
				DIRTY
			};

			struct SCacheFilePage
			{
				SCacheFilePage(int64_t	_nAddr, const byte_t *pByte, size_t size) : nAddr(_nAddr)
				{
					pageData.resize(size);
					memcpy(pageData.data(), pByte, pageData.size());			 
				}

				int64_t	nAddr;
				EFilePageState pageState{ CLEAN };
				std::vector<byte_t> pageData;

				void CopyTo(byte_t* pData) const
				{
					memcpy(pData, pageData.data(), pageData.size());
				}

				void CopyFrom(const byte_t* pData)
				{
					memcpy(pageData.data(), pData, pageData.size());
					pageState = DIRTY;
				}

			};

			typedef std::shared_ptr<SCacheFilePage> TCacheFilePagePtr;

			utils::TCacheLRU_2Q<int64_t, TCacheFilePagePtr> m_pageCache;
		};


	}
}