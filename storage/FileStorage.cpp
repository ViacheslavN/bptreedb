#include "stdafx.h"
#include "FileStorage.h"
#include "FilePage.h"


namespace bptreedb
{

	CFileStorage::CFileStorage(CommonLib::IAllocPtr pAlloc, int32_t storageId, bool bCheckCRC) : m_pAlloc(pAlloc),  m_checkCRC(bCheckCRC), m_storage_id(storageId)
	{
		 
	}

	CFileStorage::~CFileStorage()
	{

	}

	void CFileStorage::Open(const char* pszNameUtf8, bool bCreate, uint32_t nMinPageSize )
	{
		try
		{
			CommonLib::file::enOpenFileMode nOpenMode = bCreate ? CommonLib::file::ofmCreateAlways : CommonLib::file::ofmOpenExisting;
			CommonLib::file::enAccesRights nReadWrite = CommonLib::file::aeReadWrite;

			m_minPageSize = nMinPageSize;
			m_file.OpenFileA(pszNameUtf8, nOpenMode, nReadWrite, CommonLib::file::smNoMode, CommonLib::file::oftBinary);

			uint64_t nSize = m_file.GetFileSize();
			m_lastAddr = nSize / m_minPageSize;

		}
		catch (std::exception& excSrc)
		{
			CommonLib::CExcBase::RegenExcT("Error open  storage path: %1, create: %2", pszNameUtf8, bCreate ? 1 : 0, excSrc);
			throw;
		}
	}

	void CFileStorage::Open(const wchar_t* pszName, bool bCreate, uint32_t nMinPageSize)
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

	void CFileStorage::SetOffset(uint64_t offset)
	{
		m_offset = offset;
	}

	void CFileStorage::GetFilePage(FilePagePtr& pPage, int64_t nAddr, uint32_t nSize, bool decrypt)
	{
		
		try
		{
			if ((nSize % m_minPageSize) != 0)
				throw CommonLib::CExcBase("Wrong size");

			if (nSize == 0)
				nSize = m_minPageSize;

			byte_t* data = 0;		 
			bool reload = true;

			if (pPage.get() == nullptr)
			{
				data = (byte_t*)m_pAlloc->Alloc(sizeof(byte_t) * nSize);
				reload = false;
			}
			else
			{
				data = pPage->GetFullData();
				nSize = pPage->GetFullPageSize();
			}

			ReadData(nAddr, data, nSize, decrypt);

			if (pPage.get() == nullptr)
			{
				pPage.reset(new CFilePage(m_pAlloc, data, nSize, nAddr, m_checkCRC));
			}
			else
			{
				pPage->ReloadHeader();
			}			

			if (m_pageCipher.get() && decrypt)
			{
				pPage->SetNeedEncrypt(true);
			}						 

			if (m_checkCRC)
			{
				if (!pPage->CheckCRC())
					throw CommonLib::CExcBase("Wrong crc");

			}

		}
		catch (std::exception& excSrc)
		{
			CommonLib::CExcBase::RegenExcT("Failed to get file page addr: %1, size: %2", nAddr, nSize, excSrc);
			throw;
		}
	}

	FilePagePtr CFileStorage::GetFilePage(int64_t nAddr, uint32_t nSize, bool decrypt)
	{
		FilePagePtr pPage;
		GetFilePage(pPage, nAddr, nSize, decrypt);

		return pPage;
	}

	FilePagePtr CFileStorage::GetEmptyFilePage(int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype)
	{
		try
		{
			
			if ((nSize % m_minPageSize) != 0)
				throw CommonLib::CExcBase("Wrong size");

			FilePagePtr  pPage(new CFilePage(m_pAlloc, nSize, nAddr, m_checkCRC,  objectID,  objecttype,  parentID,  parenttype));
			return pPage;

		}
		catch (std::exception &excSrc)
		{
			CommonLib::CExcBase::RegenExcT("Failed to create page addr: %1, size: %2", nAddr, nSize, excSrc);
			throw;
		}

	}


	void CFileStorage::SaveFilePage(FilePagePtr& pPage)
	{
		try
		{

			if (pPage.get() == nullptr)
				throw CommonLib::CExcBase("the page is null");			

			if (m_checkCRC)
				pPage->WriteCRC();

			WriteData(pPage->GetPageAddr(), pPage->GetFullData(), pPage->GetFullPageSize(), pPage->IsNeedEncrypt());

		/*	int64_t nFileAddr = pPage->GetPageAddr() * m_minPageSize;
			m_file.SetFilePos64(m_offset + nFileAddr, CommonLib::soFromBegin);

			uint32_t nCnt = 0;
			if (m_pageCipher.get() != nullptr && pPage->IsNeedEncrypt())
			{
				if(m_bufForChiper.size() < pPage->GetFullPageSize())
					m_bufForChiper.resize(pPage->GetFullPageSize());
				m_pageCipher->encrypt(pPage.get(), &m_bufForChiper[0], pPage->GetFullPageSize());
				nCnt = m_file.WriteFile((void*)&m_bufForChiper[0], pPage->GetFullPageSize());
			}
			else
			{
				nCnt = m_file.WriteFile((void*)pPage->GetFullData(), pPage->GetFullPageSize());
			}

			if(nCnt != pPage->GetFullPageSize())
				throw CommonLib::CExcBase(L"Can't write page, page size: %1, written bytes %2", pPage->GetFullPageSize(), nCnt);*/


		}
		catch (std::exception& excSrc)
		{
			CommonLib::CExcBase::RegenExcT("Failed to save file page addr: %1", pPage.get() != nullptr ? pPage->GetPageAddr() : -1, excSrc);
			throw;
		}

	}
	
	void CFileStorage::ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize, bool decrypt)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> locker(m_mutex);
			{
				CommonLib::CPrefCounterHolder holder(m_pStoragePerformer, eReadData, nSize);

				m_file.SetFilePos64(m_offset + (nAddr * m_minPageSize), CommonLib::soFromBegin);
				uint32_t nWCnt = (uint32_t)m_file.Read(pData, nSize);

				if (nWCnt != nSize)
					throw CommonLib::CExcBase("can't read the requested size, page size: %1, returned size: %2", nSize, nWCnt);
			}

			if (m_pageCipher.get() && decrypt)
			{
				CommonLib::CPrefCounterHolder holder(m_pStoragePerformer, eDecryptData, nSize);
				m_pageCipher->DecryptBuf(pData, decrypt);
			}
		}
		catch (std::exception& excSrc)
		{
			CommonLib::CExcBase::RegenExcT("Failed to read data, addr: %1, size: %2 ", nAddr, nSize, excSrc);
			throw;
		}
	}

	void CFileStorage::WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize, bool decrypt)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> locker(m_mutex);

			int64_t nFileAddr = nAddr * m_minPageSize;
			m_file.SetFilePos64(m_offset + nFileAddr, CommonLib::soFromBegin);

			uint32_t nCnt = 0;
			if (m_pageCipher.get() != nullptr && decrypt)
			{
				if (m_bufForChiper.size() < nSize)
					m_bufForChiper.resize(nSize);
				{
					CommonLib::CPrefCounterHolder holder(m_pStoragePerformer, eEncryptData, nSize);
					m_pageCipher->EncryptBuf(pData, &m_bufForChiper[0], nSize);
				}
				{
					CommonLib::CPrefCounterHolder holder(m_pStoragePerformer, eWriteData, nSize);
					nCnt = (uint32_t)m_file.Write(&m_bufForChiper[0], nSize);
				}
			}
			else
			{
				CommonLib::CPrefCounterHolder holder(m_pStoragePerformer, eWriteData, nSize);
				nCnt = (uint32_t)m_file.Write(pData, nSize);
			}

			if (nCnt != nSize)
				throw CommonLib::CExcBase(L"Can't write page, page size: %1, written bytes %2", nSize, nCnt);
		}
		catch (std::exception& excSrc)
		{
			CommonLib::CExcBase::RegenExcT("Failed to write data, addr: %1, size %2", nAddr, nSize, excSrc);
			throw;
		}
	}


	FilePagePtr CFileStorage::GetNewFilePage(uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize)
	{
		try
		{

			int64_t nAddr =	GetNewFilePageAddr(nSize);
			FilePagePtr pPage(new CFilePage(m_pAlloc, nSize, nAddr, m_checkCRC, objectID, objecttype, parentID, parenttype));
				
			return pPage;
		}
		catch (std::exception& excSrc)
		{
			CommonLib::CExcBase::RegenExcT("Failed to get new page", excSrc);
			throw;
		}
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

	void CFileStorage::DropFilePage(int64_t nAddr)
	{
		
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
		m_pStoragePerformer = pStoragePerformer;
	}

	int32_t CFileStorage::GetStorageId() const
	{
		return m_storage_id;
	}
		   	 
}