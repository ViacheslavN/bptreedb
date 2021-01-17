#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"
#include "storage.h"
#include "../utils/CacheLRU_2Q.h"
#include "../../CommonLib/filesystem/filesystem.h"
#include "../crypto/PageCipher.h"


namespace bptreedb
{
	class CFileStorage : public IStorage
	{
	private:
		CFileStorage(const CFileStorage&);
		CFileStorage(const CFileStorage&&);
		CFileStorage& operator = (const CFileStorage&);
		CFileStorage& operator = (const CFileStorage&&);
	public:

		
		CFileStorage(std::shared_ptr<CommonLib::IAlloc> pAlloc, /*uint32_t nCacheSize = 1000,*/ bool bCheckCRC = true);
		virtual ~CFileStorage();



		virtual void Open(const wchar_t* pszName, bool bCreate, uint32_t nMinPageSize = 8192);
		virtual void Close();



		virtual void ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize, bool decrypt);
		virtual void WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize, bool decrypt);
		virtual void SetOffset(uint64_t offset);
		virtual void GetFilePage(FilePagePtr& pPage, int64_t nAddr, uint32_t nSize, bool decrypt);
		virtual FilePagePtr GetFilePage(int64_t nAddr, uint32_t nSize, bool decrypt);
		virtual void SaveFilePage(FilePagePtr& pPage);
		virtual void DropFilePage(int64_t nAddr);
		virtual FilePagePtr GetNewFilePage(uint32_t nSize = 0);
		virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0);

		virtual FilePagePtr GetEmptyFilePage(int64_t nAddr, uint32_t nSize);

		virtual void Flush();
		virtual void SetStoragePerformer(CommonLib::TPrefCounterPtr pStoragePerformer);

	private:

		CommonLib::file::CFile m_file;
		std::shared_ptr<IPageCipher> m_pageCipher;
		CommonLib::IAllocPtr m_pAlloc;

		uint64_t m_offset{ 0 };
		uint32_t m_minPageSize{ 256 };
		uint64_t m_lastAddr{ 0 };

		struct TPageFreeChecker
		{
			bool IsFree(FilePagePtr& pObj)
			{				
				return  pObj.use_count() == 1;
			}
		};

	//	typedef utils::TCacheLRU_2Q<int64_t, FilePagePtr, TPageFreeChecker> TNodesCache;
	//	TNodesCache m_pageCache;
	//	uint32_t m_cacheSize;
		bool m_checkCRC;

		typedef std::vector<byte_t> TBufferForChiper;
		TBufferForChiper m_bufForChiper;
		CommonLib::TPrefCounterPtr m_pStoragePerformer;
	};
}