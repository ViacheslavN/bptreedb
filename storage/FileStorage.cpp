#include "pch.h"
#include "FileStorage.h"

namespace bptreedb
{
	namespace storage
	{
		CFileStorage::CFileStorage(std::shared_ptr<CommonLib::IAlloc> pAlloc, int32_t storageId, int32_t cacheSize) : m_ptrAlloc(pAlloc), 
			m_storage_id(storageId), m_cacheSize(cacheSize)
		{

		}

		CFileStorage::~CFileStorage()
		{

		}

		void CFileStorage::Open(const char* pszNameUtf8, bool bCreate, uint64_t offset, uint32_t nMinPageSize)
		{
			try
			{
				if ((nMinPageSize % MIN_PAGE_SIZE) != 0)
					throw CommonLib::CExcBase("Wrong page size: %1", nMinPageSize);

				m_offset = offset;
				CommonLib::file::enOpenFileMode nOpenMode = bCreate ? CommonLib::file::ofmCreateAlways : CommonLib::file::ofmOpenExisting;
				CommonLib::file::enAccesRights nReadWrite = CommonLib::file::aeReadWrite;

				m_minPageSize = nMinPageSize;
				m_file.OpenFileA(pszNameUtf8, nOpenMode, nReadWrite, CommonLib::file::smNoMode, CommonLib::file::oftBinary);

				uint64_t nSize = m_file.GetFileSize();
				m_lastAddr = nSize / m_minPageSize;

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Error open storage path: %1, create: %2", pszNameUtf8, bCreate ? 1 : 0, excSrc);
				throw;
			}
		}

		void CFileStorage::Open(const wchar_t* pszName, bool bCreate, uint64_t offset, uint32_t nMinPageSize)
		{
			Open(CommonLib::StringEncoding::str_w2utf8_safe(pszName).c_str(), bCreate, nMinPageSize);
		}

		void CFileStorage::Close()
		{
			try
			{
				m_file.CloseFile();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Error close  storage", excSrc);
				throw;
			}
		}

		void CFileStorage::ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize)
		{
			try
			{
				if ((nSize % m_minPageSize) != 0)
					throw CommonLib::CExcBase("Wrong size: %1", nSize);

				std::lock_guard<std::recursive_mutex> locker(m_mutex);
				{
					int64_t nStartAddr = nAddr;
					byte_t* pStartData = pData;

					int32_t nPageCnt = nSize / m_minPageSize;

					for (int32_t i = 0; i < nPageCnt; ++i)
					{
						_ReadData(nStartAddr, pStartData);

						nStartAddr += m_minPageSize;
						pStartData += m_minPageSize;
					}
				}
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to read data, addr: %1, size: %2 ", nAddr, nSize, excSrc);
				throw;
			}
		}

		void CFileStorage::_ReadData(int64_t nAddr, byte_t* pData)
		{
			CommonLib::CPrefCounterHolder holder(m_ptrStoragePerformer, eReadData, m_minPageSize);

			TCacheFilePagePtr ptrPage = m_pageCache.GetElem(nAddr);
			if (ptrPage.get())
			{
				ptrPage->CopyTo(pData);
				return;
			}

			m_file.SetFilePos64(m_offset + (nAddr * m_minPageSize), CommonLib::soFromBegin);
			uint32_t nWCnt = (uint32_t)m_file.Read(pData, m_minPageSize);

			if (nWCnt != m_minPageSize)
				throw CommonLib::CExcBase("Can't read the requested size, page size: %1, returned size: %2", m_minPageSize, nWCnt);

			if (m_ptrStorageCipher.get())
			{
				CommonLib::CPrefCounterHolder holder(m_ptrStoragePerformer, eDecryptData, m_minPageSize);
				m_ptrStorageCipher->Decrypt(nAddr, pData, m_minPageSize);
			}

			if (m_pageCache.Size() == m_cacheSize)
			{
				TCacheFilePagePtr ptrPage = m_pageCache.RemoveBack();
				if (ptrPage->pageState == DIRTY)
				{
					_WriteData(ptrPage->nAddr, ptrPage->pageData.data());
				}

				ptrPage->CopyFrom(pData);
				ptrPage->nAddr = nAddr;
				m_pageCache.AddElem(ptrPage->nAddr, ptrPage);
			}
			else
			{
				TCacheFilePagePtr ptrPage = std::make_shared<SCacheFilePage>(nAddr, pData, m_minPageSize);
				m_pageCache.AddElem(nAddr, ptrPage);
			}

		}

		void CFileStorage::WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize)
		{
			try
			{
				if ((nSize % m_minPageSize) != 0)
					throw CommonLib::CExcBase("Wrong size: %1", nSize);

				std::lock_guard<std::recursive_mutex> locker(m_mutex);
				{
					int64_t nStartAddr = nAddr;
					const byte_t* pStartData = pData;

					int32_t nPageCnt = nSize / m_minPageSize;

					for (int32_t i = 0; i < nPageCnt; ++i)
					{
						_WriteData(nStartAddr, pStartData);

						nStartAddr += m_minPageSize;
						pStartData += m_minPageSize;
					}
				}
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to write data, addr: %1, size %2", nAddr, nSize, excSrc);
				throw;
			}
		}

		void CFileStorage::_WriteData(int64_t nAddr, const byte_t* pData)
		{
			TCacheFilePagePtr ptrPage = m_pageCache.GetElem(nAddr);
			if (ptrPage.get())
			{
				ptrPage->CopyFrom(pData);
				return;
			}

			int64_t nFileAddr = nAddr * m_minPageSize;
			m_file.SetFilePos64(m_offset + nFileAddr, CommonLib::soFromBegin);

			uint32_t nCnt = 0;
			if (m_ptrStorageCipher.get() != nullptr)
			{
				if (m_bufForChiper.size() < m_minPageSize)
					m_bufForChiper.resize(m_minPageSize);
				{
					CommonLib::CPrefCounterHolder holder(m_ptrStoragePerformer, eEncryptData, m_minPageSize);
					m_ptrStorageCipher->Encrypt(nAddr, pData, m_minPageSize,  &m_bufForChiper[0], m_minPageSize);
				}
				{
					CommonLib::CPrefCounterHolder holder(m_ptrStoragePerformer, eWriteData, m_minPageSize);
					nCnt = (uint32_t)m_file.Write(&m_bufForChiper[0], m_minPageSize);
				}
			}
			else
			{
				CommonLib::CPrefCounterHolder holder(m_ptrStoragePerformer, eWriteData, m_minPageSize);
				nCnt = (uint32_t)m_file.Write(pData, m_minPageSize);
			}

			if (nCnt != m_minPageSize)
				throw CommonLib::CExcBase(L"Can't write page, page size: %1, written bytes %2", m_minPageSize, nCnt);		
		}

		void CFileStorage::Flush()
		{
			try
			{
				m_file.Flush();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to flush storage", excSrc);
				throw;
			}
		}

		void CFileStorage::SetStoragePerformer(CommonLib::TPrefCounterPtr pStoragePerformer)
		{
			m_ptrStoragePerformer = pStoragePerformer;
		}

		int32_t CFileStorage::GetStorageId() const
		{
			return m_storage_id;
		}

		int64_t CFileStorage::GetNewFilePageAddr(uint32_t nSize)
		{
			try
			{
				std::lock_guard<std::recursive_mutex> locker(m_mutex);

				if (nSize == 0)
					nSize = m_minPageSize;

				if ((nSize % m_minPageSize) != 0)
					throw CommonLib::CExcBase("Wrong page size: %1", nSize);

				uint32_t nCount = nSize / m_minPageSize;

				int64_t nAddr = m_lastAddr;
				m_lastAddr += nCount;

				return nAddr;

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to get new page addr", excSrc);
				throw;
			}
		}
	}
}