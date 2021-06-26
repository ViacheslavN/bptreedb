#pragma once
#include "storage.h"


namespace bptreedb
{

	template<class TCache>
	class TCacheStorage : public IStorage
	{
		public:
			typedef std::shared_ptr<TCache> TCachePtr;
			TCacheStorage(IStoragePtr pStorage, TCachePtr pCache, uint32_t cacheSize;) : m_pStorage(pStorage), m_pageCache(pCache), m_pageCache(cacheSize)
			{}
			virtual ~TCacheStorage() {}


			virtual void Open(const char* pszNameUtf8, bool bCreate, uint32_t nMinPageSize = 8192)
			{
				return m_pStorage->Open(pszNameUtf8, bCreate, nMinPageSize);
			}

			virtual void Close()
			{
				m_pStorage->Close();
				m_pageCache->Clear();
			}

			virtual void SetOffset(uint64_t offset)
			{
				m_pStorage->SetOffset(offset);
			}

			virtual void ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize, bool decrypt)
			{
				m_pStorage->ReadData(nAddr,  pData, nSize, decrypt)
			}

			virtual void WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize, bool decrypt)
			{
				m_pStorage->WriteData(nAddr, pData, nSize, decrypt)
			}

			virtual void GetFilePage(FilePagePtr& pPage, int64_t nAddr, uint32_t nSize, bool decrypt)
			{
				FilePagePtr pCachePage = m_pageCache.GetElem(nAddr);
				if (pCachePage.get() != nullptr)
				{
					pPage = pCachePage;
					return;
				}

				m_pStorage->GetFilePage(pPage, nAddr, nSize, decrypt);
				AddCache(pPage);
			}

			virtual FilePagePtr GetFilePage(int64_t nAddr, uint32_t nSize, bool decrypt)
			{
				try
				{
					FilePagePtr pPage = m_pageCache.GetElem(nAddr);
					if (pPage.get() != nullptr)
						return pPage;

					pPage = m_pStorage->GetFilePage(nAddr, nSize, decrypt);

					AddCache(pPage);
					return pPage;
				}
				catch (CommonLib::CExcBase& excSrc)
				{
					excSrc.AddMsgT(L"Cache storage");
					throw;
				}
			}

			virtual void SaveFilePage(FilePagePtr& pPage)
			{

				AddCache(pPage);
				m_pStorage->SaveFilePage(pPage);
			}

			virtual void DropFilePage(int64_t nAddr)
			{
				m_pStorage->DropFilePage(nAddr);
			}

			virtual FilePagePtr GetNewFilePage(uint32_t nSize = 0)
			{
				try
				{
					FilePagePtr pPage = m_pStorage->GetNewFilePage(nSize);

					AddCache(pPage);
					return pPage;
				}
				catch (CommonLib::CExcBase& excSrc)
				{
					excSrc.AddMsgT("Cache storage failed to get new filepage");
					throw;

				}
			}

			virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0)
			{
				return m_pStorage->GetNewFilePageAddr(nSize);
			}

			virtual FilePagePtr GetEmptyFilePage(int64_t nAddr, uint32_t nSize)
			{
				try
				{
					FilePagePtr pPage = m_pageCache.GetElem(nAddr);
					if (pPage.get() != nullptr)
						return pPage;

					return m_pStorage->GetEmptyFilePage(nAddr, nSize);
				}
				catch (CommonLib::CExcBase& excSrc)
				{
					excSrc.AddMsgT("Cache storage failed to get empty page");
					throw;
				}
			}

			virtual void Flush()
			{
				m_pStorage->Flush();
			}


	private:

		void AddCache(FilePagePtr& pPage)
		{
			if (m_pageCache->Size() > m_cacheSize)
				m_pageCache->RemoveBack();

			m_pageCache->AddElemIfNoExists(pPage->GetPageAddr(), pPage);
		}

	private:
		IStoragePtr m_pStorage;
		TCache m_pageCache;
		uint32_t m_cacheSize;

	};
}