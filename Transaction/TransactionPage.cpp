#include "pch.h"
#include "TransactionPage.h"
#include "../../CommonLib/date/DateUtil.h"

namespace bptreedb
{
	namespace transaction
	{
		CTransactionPage::CTransactionPage(CommonLib::IAllocPtr ptrAlloc, uint32_t nSize, int64_t nAddr,
			int64_t nTransactionId, int32_t nUserId,
			uint32_t objectID, uint32_t objecttype, uint32_t parentID, uint32_t parenttype): 
			m_objectID(objectID), m_objectType(objecttype), m_parentID(parentID), m_parentType(parenttype)
		{
			m_ptrBuffer = std::make_shared<storage::CPageMemoryBuffer>(ptrAlloc);
			m_ptrBuffer->Create(nSize);
			m_ptrPage = std::make_shared<storage::CFilePage>(m_ptrBuffer, nAddr);
		}

		CTransactionPage::CTransactionPage(CommonLib::IAllocPtr ptrAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr,
			int64_t nTransactionId, int32_t nUserId,
			uint32_t objectID, uint32_t objecttype, uint32_t parentID, uint32_t parenttype) :
			m_objectID(objectID), m_objectType(objecttype), m_parentID(parentID), m_parentType(parenttype)
		{
			m_ptrBuffer = std::make_shared<storage::CPageMemoryBuffer>(ptrAlloc, page_header_size);
			m_ptrBuffer->AttachBuffer(pData, nSize);

			m_ptrPage = std::make_shared<storage::CFilePage>(m_ptrBuffer, nAddr);
		}

		CTransactionPage::CTransactionPage(storage::CPageMemoryBufferPtr ptrBuffer, int64_t nAddr,
			int64_t nTransactionId, int32_t nUserId,
			uint32_t objectID, uint32_t objecttype, uint32_t parentID, uint32_t parenttype) :
			m_objectID(objectID), m_objectType(objecttype), m_parentID(parentID), m_parentType(parenttype)
		{
			m_ptrBuffer = ptrBuffer;
			m_ptrPage = std::make_shared<storage::CFilePage>(m_ptrBuffer, nAddr);
		}

		CTransactionPage::CTransactionPage(storage::CPageMemoryBufferPtr ptrBuffer, int64_t nAddr)
		{
			m_ptrBuffer = ptrBuffer;
			m_ptrPage = std::make_shared<storage::CFilePage>(m_ptrBuffer, nAddr);
		}

		CTransactionPage::~CTransactionPage()
		{

		}

		int64_t CTransactionPage::GetAddr() const
		{
			return m_ptrPage->GetAddr();
		}

		void CTransactionPage::SetAddr(int64_t nAddr)
		{
			m_ptrPage->SetAddr(nAddr);
		}

		uint32_t CTransactionPage::GetObjectType() const
		{
			return m_objectType;
		}

		uint32_t CTransactionPage::GetObjectID() const
		{
			return m_objectID;
		}

		uint32_t CTransactionPage::GetParentType() const
		{
			return m_parentType;
		}

		uint32_t CTransactionPage::GetParentObjectID() const
		{
			return m_parentID;
		}
 

		void CTransactionPage::ReadMetaData()
		{
			CommonLib::IMemoryReadStreamPtr ptrStream = m_ptrPage->GetReadStream();
			m_nTransactionId = ptrStream->ReadIntu64();
			m_nDate = ptrStream->ReadIntu64();
			m_nUserId = ptrStream->ReadInt32();

			m_objectType = ptrStream->ReadIntu32();
			m_objectID = ptrStream->ReadIntu32();
			m_parentID = ptrStream->ReadIntu32();
			m_parentType = ptrStream->ReadIntu32();
		}

		CommonLib::IMemoryWriteStreamPtr CTransactionPage::GetWriteStream() const
		{
			CommonLib::IMemoryWriteStreamPtr ptrBaseStream = m_ptrPage->GetWriteStream();
			CommonLib::IMemoryWriteStreamPtr ptrStream = std::make_shared<CommonLib::CFxMemoryWriteStream>();
			ptrStream->AttachBuffer(ptrBaseStream->Buffer() + page_header_size, ptrBaseStream->Size() - page_header_size);
 
			return ptrStream;
		}

		CommonLib::IMemoryReadStreamPtr CTransactionPage::GetReadStream() const
		{
			CommonLib::IMemoryReadStreamPtr ptrBaseStream = m_ptrPage->GetReadStream();
			CommonLib::IMemoryReadStreamPtr ptrStream = std::make_shared<CommonLib::CReadMemoryStream>();
			ptrStream->AttachBuffer(ptrBaseStream->Buffer() + page_header_size, ptrBaseStream->Size() - page_header_size);

			return ptrStream;
		}


		void CTransactionPage::Save(storage::IPageIOPtr ptrPageIO)
		{
			CommonLib::IMemoryWriteStreamPtr ptrStream = m_ptrPage->GetWriteStream();
			ptrStream->Write(m_nTransactionId);
			m_nDate = CommonLib::dateutil::CDateUtil::GetCurrentDateTime();
			ptrStream->Write(m_nDate);
			ptrStream->Write(m_nUserId);

			ptrStream->Write(m_objectType);
			ptrStream->Write(m_objectID);
			ptrStream->Write(m_parentID);
			ptrStream->Write(m_parentType);

			m_ptrPage->Save(ptrPageIO);
		}

		void CTransactionPage::Read(storage::IPageIOPtr ptrPageIO, int64_t nAddr)
		{
			m_ptrPage->Read(ptrPageIO, nAddr);
			ReadMetaData();
		}
	}

}