#pragma once
#include "../CommonLib/CommonLib.h"
#include "../CommonLib/alloc/alloc.h"
#include "../UtilsLib/CacheLRU_2Q.h"
#include "Storage.h"
#include "FilePage.h"

namespace bptreedb
{
	namespace storage
	{
		// Provides file pages on top of an IStorage, with a write-back page cache.
		// Dirty pages reach the underlying storage on eviction or on Flush().
		class CFilePager : public IFilePager
		{
		private:
			CFilePager(const CFilePager&);
			CFilePager(const CFilePager&&);
			CFilePager& operator = (const CFilePager&);
			CFilePager& operator = (const CFilePager&&);

		public:
			CFilePager(CommonLib::IAllocPtr ptrAlloc, IStoragePtr ptrStorage, uint32_t nCacheSize);
			virtual ~CFilePager();

			// IFilePager
			virtual int64_t GetNewFilePageAddr();
			virtual IFilePagePtr GetNewPage(int64_t nAddr);
			virtual IFilePagePtr ReadPage(int64_t nAddr);
			virtual void ReadPage(IFilePagePtr ptrPage, int64_t nAddr);
			virtual void SavePage(IFilePagePtr ptrPage);

			// Writes all dirty pages to the storage and flushes it.
			void Flush();
			// Drops every cached page without writing (dirty data is lost).
			void ClearCache();

			IStoragePtr GetStorage() const;
			uint32_t GetPageSize() const;
			uint32_t GetCacheSize() const;
			uint32_t GetCachedPageCount() const;

		private:

			enum EFilePageState
			{
				CLEAN,
				DIRTY
			};

			struct SCacheFilePage
			{
				SCacheFilePage(int64_t _nAddr, const byte_t* pByte, size_t size) : nAddr(_nAddr)
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

				void Reset(int64_t _nAddr, const byte_t* pData, size_t size, EFilePageState state)
				{
					nAddr = _nAddr;
					pageData.resize(size);
					memcpy(pageData.data(), pData, pageData.size());
					pageState = state;
				}
			};

			typedef std::shared_ptr<SCacheFilePage> TCacheFilePagePtr;

			// Internal cached view of the storage. IFilePage::Read/Save work through an
			// IStorageIO, so this is what the pager hands to the pages; it is never
			// exposed outside CFilePager.
			class CCachedStorageIO : public IStorageIO
			{
			public:
				CCachedStorageIO(CommonLib::IAllocPtr ptrAlloc, IStoragePtr ptrStorage, uint32_t nCacheSize);
				virtual ~CCachedStorageIO();

				virtual uint32_t GetPageSize() const;
				virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0);
				virtual void ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize);
				virtual void WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize);
				virtual void DropData(int64_t nAddr, uint32_t nSize);

				void Flush();
				void Clear();
				IStoragePtr GetStorage() const;
				uint32_t GetCacheSize() const;
				uint32_t GetCachedPageCount() const;

			private:
				void _ReadData(int64_t nAddr, byte_t* pData);
				void _WriteData(int64_t nAddr, const byte_t* pData);
				void _DropData(int64_t nAddr);
				void _CheckSize(uint32_t nSize) const;
				// Puts a page into the cache, evicting (and writing back if dirty) the
				// least recently used one when the cache is full.
				void _AddToCache(int64_t nAddr, const byte_t* pData, EFilePageState state);

			private:
				IStoragePtr m_ptrStorage;
				uint32_t m_cacheSize{ 0 };
				mutable std::recursive_mutex m_mutex;
				utils::TCacheLRU_2Q<int64_t, TCacheFilePagePtr> m_pageCache;
			};

			typedef std::shared_ptr<CCachedStorageIO> TCachedStorageIOPtr;

		private:
			CommonLib::IAllocPtr m_ptrAlloc;
			TCachedStorageIOPtr m_ptrIO;
		};

		typedef std::shared_ptr<CFilePager> CFilePagerPtr;
	}
}
