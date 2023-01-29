#include "pch.h"
#include "FilePage.h"

namespace bptreedb
{
	namespace storage
	{
		CFilePage::CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr,
			uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype) :  m_nAddr(nAddr),
			m_objectID(objectID), m_objectType(objecttype), m_parentID(parentID), m_parentType(parenttype)
		{
			m_ptrBuffer = std::make_shared<CommonLib::CMemoryStreamBuffer>(pAlloc);
			m_ptrBuffer->Create(nSize);		
		}

		CFilePage::CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr,
			uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype) :
			 m_nAddr(nAddr), m_objectID(objectID), m_objectType(objecttype), m_parentID(parentID), m_parentType(parenttype)
		{
			m_ptrBuffer = std::make_shared<CommonLib::CMemoryStreamBuffer>(pAlloc);
			m_ptrBuffer->AttachBuffer(pData, nSize);
		}

		CFilePage::~CFilePage()
		{
			
		}

		bool CFilePage::CheckCRC() const
		{
			return CheckCRC(GetFullData(), GetFullPageSize());
		}

		bool CFilePage::CheckCRCAndThrow() const
		{
			if(!CheckCRC())
				throw CommonLib::CExcBase("Wrong crc for page addr %1", m_nAddr);
		}

		int64_t CFilePage::GetAddr() const
		{
			return m_nAddr;
		}

		void CFilePage::SetAddr(int64_t nAddr)
		{
			m_nAddr = nAddr;
		}

		ObjectPageType CFilePage::GetObjectType() const
		{
			return (ObjectPageType)m_objectType;
		}

		uint32_t CFilePage::GetObjectID() const
		{
			return m_objectID;
		}

		ObjectPageType CFilePage::GetParentType() const
		{
			return (ObjectPageType)m_parentType;
		}

		uint32_t CFilePage::GetParentObjectID() const
		{
			return m_parentID;
		}

		byte_t* CFilePage::GetData()
		{
			return m_ptrBuffer->GetData() + sizeof(uint32_t) + page_header_size;
		}

		uint32_t CFilePage::GetPageSize() const
		{
			return m_ptrBuffer->GetSize() - sizeof(uint32_t) - page_header_size;
		}

		byte_t* CFilePage::GetFullData()
		{
			return  m_ptrBuffer->GetData();
		}

		const byte_t* CFilePage::GetFullData() const
		{
			return  m_ptrBuffer->GetData();
		}

		uint32_t CFilePage::GetFullPageSize() const
		{
			return m_ptrBuffer->GetSize();
		}

		CommonLib::IMemoryWriteStreamPtr CFilePage::GetWriteStream() const
		{
			CommonLib::IMemoryWriteStreamPtr ptrStream = std::make_shared<CommonLib::CFxMemoryWriteStream>();
			ptrStream->AttachBuffer(m_ptrBuffer);

			return ptrStream;
		}

		CommonLib::IMemoryReadStreamPtr CFilePage::GetReadStream() const
		{
			CommonLib::IMemoryReadStreamPtr ptrStream = std::make_shared<CommonLib::CReadMemoryStream>();
			ptrStream->AttachBuffer(m_ptrBuffer);

			return ptrStream;
		}

		void CFilePage::Save(IStoragePtr ptrStorage)
		{
			try
			{
			}		
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Filed to save file page addr: %1", m_nAddr, excSrc);
				throw;
			}
		}

		CFilePagePtr CFilePage::Read(IStoragePtr ptrStorage, std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr)
		{
			try
			{
				return CFilePagePtr();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Filed to read file page addr: %1", nAddr, excSrc);
				throw;
			}
		}
	}
}