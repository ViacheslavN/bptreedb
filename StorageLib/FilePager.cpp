#include "pch.h"
#include "FilePager.h"

namespace bptreedb
{
	namespace storage
	{
		//
		// CFilePager
		//

		CFilePager::CFilePager(CommonLib::IAllocPtr ptrAlloc, IStoragePtr ptrStorage, uint32_t nCacheSize) :
			m_ptrAlloc(ptrAlloc)
		{
			if (ptrStorage.get() == nullptr)
				throw CommonLib::CExcBase("CFilePager: storage is null");

			m_ptrIO = std::make_shared<CCachedStorageIO>(ptrAlloc, ptrStorage, nCacheSize);
		}

		CFilePager::~CFilePager()
		{
			// Flush must be called explicitly; a destructor cannot safely propagate I/O errors.
		}

		int64_t CFilePager::GetNewFilePageAddr()
		{
			return m_ptrIO->GetNewFilePageAddr();
		}

		IFilePagePtr CFilePager::GetNewPage(int64_t nAddr)
		{
			try
			{
				CFilePagePtr ptrPage = std::make_shared<CFilePage>(m_ptrAlloc, m_ptrIO->GetPageSize(), nAddr);
				return std::static_pointer_cast<IFilePage>(ptrPage);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to create new page addr: {0}", nAddr, excSrc);
				throw;
			}
		}

		IFilePagePtr CFilePager::ReadPage(int64_t nAddr)
		{
			try
			{
				return CFilePage::Read(m_ptrIO, m_ptrAlloc, nAddr);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to read page addr: {0}", nAddr, excSrc);
				throw;
			}
		}

		void CFilePager::ReadPage(IFilePagePtr ptrPage, int64_t nAddr)
		{
			try
			{
				if (ptrPage.get() == nullptr)
					throw CommonLib::CExcBase("Page is null");

				ptrPage->Read(m_ptrIO, nAddr);
				ptrPage->SetAddr(nAddr);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to read page addr: {0}", nAddr, excSrc);
				throw;
			}
		}

		void CFilePager::SavePage(IFilePagePtr ptrPage)
		{
			try
			{
				if (ptrPage.get() == nullptr)
					throw CommonLib::CExcBase("Page is null");

				ptrPage->Save(m_ptrIO);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to save page addr: {0}", ptrPage.get() ? ptrPage->GetAddr() : -1, excSrc);
				throw;
			}
		}

		void CFilePager::Flush()
		{
			m_ptrIO->Flush();
		}

		void CFilePager::ClearCache()
		{
			m_ptrIO->Clear();
		}

		IStoragePtr CFilePager::GetStorage() const
		{
			return m_ptrIO->GetStorage();
		}

		uint32_t CFilePager::GetPageSize() const
		{
			return m_ptrIO->GetPageSize();
		}

		uint32_t CFilePager::GetCacheSize() const
		{
			return m_ptrIO->GetCacheSize();
		}

		uint32_t CFilePager::GetCachedPageCount() const
		{
			return m_ptrIO->GetCachedPageCount();
		}

		//
		// CFilePager::CCachedStorageIO
		//

		CFilePager::CCachedStorageIO::CCachedStorageIO(CommonLib::IAllocPtr ptrAlloc, IStoragePtr ptrStorage, uint32_t nCacheSize) :
			m_ptrStorage(ptrStorage), m_cacheSize(nCacheSize), m_pageCache(ptrAlloc)
		{
		}

		CFilePager::CCachedStorageIO::~CCachedStorageIO()
		{
		}

		uint32_t CFilePager::CCachedStorageIO::GetPageSize() const
		{
			return m_ptrStorage->GetPageSize();
		}

		int64_t CFilePager::CCachedStorageIO::GetNewFilePageAddr(uint32_t nSize)
		{
			return m_ptrStorage->GetNewFilePageAddr(nSize);
		}

		void CFilePager::CCachedStorageIO::_CheckSize(uint32_t nSize) const
		{
			uint32_t nPageSize = GetPageSize();
			if (nPageSize == 0 || (nSize % nPageSize) != 0)
				throw CommonLib::CExcBase("Wrong size: {0}, page size: {1}", nSize, nPageSize);
		}

		void CFilePager::CCachedStorageIO::ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize)
		{
			try
			{
				_CheckSize(nSize);

				std::lock_guard<std::recursive_mutex> locker(m_mutex);

				uint32_t nPageSize = GetPageSize();
				uint32_t nPageCnt = nSize / nPageSize;

				for (uint32_t i = 0; i < nPageCnt; ++i)
					_ReadData(nAddr + i, pData + (size_t)i * nPageSize);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to read data, addr: {0}, size: {1}", nAddr, nSize, excSrc);
				throw;
			}
		}

		void CFilePager::CCachedStorageIO::WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize)
		{
			try
			{
				_CheckSize(nSize);

				std::lock_guard<std::recursive_mutex> locker(m_mutex);

				uint32_t nPageSize = GetPageSize();
				uint32_t nPageCnt = nSize / nPageSize;

				for (uint32_t i = 0; i < nPageCnt; ++i)
					_WriteData(nAddr + i, pData + (size_t)i * nPageSize);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to write data, addr: {0}, size: {1}", nAddr, nSize, excSrc);
				throw;
			}
		}

		void CFilePager::CCachedStorageIO::DropData(int64_t nAddr, uint32_t nSize)
		{
			try
			{
				_CheckSize(nSize);

				std::lock_guard<std::recursive_mutex> locker(m_mutex);

				uint32_t nPageCnt = nSize / GetPageSize();

				for (uint32_t i = 0; i < nPageCnt; ++i)
					_DropData(nAddr + i);

				m_ptrStorage->DropData(nAddr, nSize);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to drop data, addr: {0}, size: {1}", nAddr, nSize, excSrc);
				throw;
			}
		}

		// Cache internals (caller holds m_mutex)

		void CFilePager::CCachedStorageIO::_ReadData(int64_t nAddr, byte_t* pData)
		{
			TCacheFilePagePtr ptrPage = m_pageCache.GetElem(nAddr);
			if (ptrPage.get() != nullptr)
			{
				ptrPage->CopyTo(pData);
				return;
			}

			m_ptrStorage->ReadData(nAddr, pData, GetPageSize());
			_AddToCache(nAddr, pData, CLEAN);
		}

		void CFilePager::CCachedStorageIO::_WriteData(int64_t nAddr, const byte_t* pData)
		{
			TCacheFilePagePtr ptrPage = m_pageCache.GetElem(nAddr);
			if (ptrPage.get() != nullptr)
			{
				ptrPage->CopyFrom(pData);
				return;
			}

			// Write-back: keep the page in the cache, it reaches the storage on eviction or Flush().
			_AddToCache(nAddr, pData, DIRTY);
		}

		void CFilePager::CCachedStorageIO::_DropData(int64_t nAddr)
		{
			// Dropped pages are discarded, dirty or not.
			m_pageCache.Remove(nAddr);
		}

		void CFilePager::CCachedStorageIO::_AddToCache(int64_t nAddr, const byte_t* pData, EFilePageState state)
		{
			uint32_t nPageSize = GetPageSize();

			if (m_cacheSize == 0)
			{
				// Cache disabled: write-through.
				if (state == DIRTY)
					m_ptrStorage->WriteData(nAddr, pData, nPageSize);
				return;
			}

			if (m_pageCache.Size() >= m_cacheSize)
			{
				TCacheFilePagePtr ptrEvicted = m_pageCache.RemoveBack();
				if (ptrEvicted.get() != nullptr)
				{
					if (ptrEvicted->pageState == DIRTY)
						m_ptrStorage->WriteData(ptrEvicted->nAddr, ptrEvicted->pageData.data(), (uint32_t)ptrEvicted->pageData.size());

					// Reuse the evicted buffer instead of reallocating.
					ptrEvicted->Reset(nAddr, pData, nPageSize, state);
					m_pageCache.AddElem(nAddr, ptrEvicted);
					return;
				}
			}

			TCacheFilePagePtr ptrPage = std::make_shared<SCacheFilePage>(nAddr, pData, nPageSize);
			ptrPage->pageState = state;
			m_pageCache.AddElem(nAddr, ptrPage);
		}

		void CFilePager::CCachedStorageIO::Flush()
		{
			try
			{
				std::lock_guard<std::recursive_mutex> locker(m_mutex);

				auto it = m_pageCache.Begin();
				while (!it.IsNull())
				{
					TCacheFilePagePtr& ptrPage = it.Object();
					if (ptrPage->pageState == DIRTY)
					{
						m_ptrStorage->WriteData(ptrPage->nAddr, ptrPage->pageData.data(), (uint32_t)ptrPage->pageData.size());
						ptrPage->pageState = CLEAN;
					}

					it.Next();
				}

				m_ptrStorage->Flush();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to flush pager", excSrc);
				throw;
			}
		}

		void CFilePager::CCachedStorageIO::Clear()
		{
			std::lock_guard<std::recursive_mutex> locker(m_mutex);
			m_pageCache.Clear();
		}

		IStoragePtr CFilePager::CCachedStorageIO::GetStorage() const
		{
			return m_ptrStorage;
		}

		uint32_t CFilePager::CCachedStorageIO::GetCacheSize() const
		{
			return m_cacheSize;
		}

		uint32_t CFilePager::CCachedStorageIO::GetCachedPageCount() const
		{
			std::lock_guard<std::recursive_mutex> locker(m_mutex);
			return m_pageCache.Size();
		}
	}
}
