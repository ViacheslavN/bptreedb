#include "pch.h"
#include "TransactionPage.h"
#include "../../CommonLib/date/DateUtil.h"

namespace bptreedb
{
	namespace transaction
	{

		CTransactionPage::CTransactionPage(CommonLib::IAllocPtr ptrAlloc, uint32_t nSize, int64_t nAddr, int32_t nStorageId,
			const util::CUInt128& lsn, int32_t nUserId) : m_nLSN(lsn), m_nUserId(m_nUserId), m_StorageId(nStorageId)
		{
			m_ptrPage = std::make_shared<storage::CFilePage>(ptrAlloc, nSize, nAddr);
		}

		CTransactionPage::CTransactionPage(CommonLib::IAllocPtr ptrAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr, int32_t nStorageId,
			const util::CUInt128& lsn, int32_t nUserId) : m_nLSN(lsn), m_nUserId(m_nUserId), m_StorageId(nStorageId)
		{
			m_ptrPage = std::make_shared<storage::CFilePage>(ptrAlloc, pData, nSize, nAddr);
		}

		CTransactionPage::CTransactionPage(storage::IFilePagePtr ptrPage)
		{
			m_ptrPage = ptrPage;
			ReadMetaData();
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

		uint32_t CTransactionPage::GetSize() const
		{
			return m_ptrPage->GetSize();
		}
 

		void CTransactionPage::ReadMetaData()
		{
			CommonLib::IMemoryReadStreamPtr ptrStream = m_ptrPage->GetReadStream();
			m_nLSN.Load(ptrStream);
			m_StorageId = ptrStream->ReadInt32();
			ptrStream->ReadInt64(); //nAddr
			m_nDate = ptrStream->ReadIntu64();
			m_nUserId = ptrStream->ReadInt32();

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

		uint32_t CTransactionPage::GetStorageId() const
		{
			return m_StorageId;
		} 

		util::CUInt128 CTransactionPage::GetTransactionLSN() const
		{
			return m_nLSN;
		}

		uint64_t CTransactionPage::GetDate() const
		{
			return m_nDate;
		}

		int32_t CTransactionPage::GetUserId() const
		{
			return m_nUserId;
		}

		void CTransactionPage::Save(storage::IPageIOPtr ptrPageIO, int64_t nAddr)
		{
			try
			{
				CommonLib::IMemoryWriteStreamPtr ptrStream = m_ptrPage->GetWriteStream();
				m_nLSN.Save(ptrStream);
				ptrStream->Write(m_StorageId);
				ptrStream->Write(m_ptrPage->GetAddr());
				m_nDate = CommonLib::dateutil::CDateUtil::GetCurrentDateTime();
				ptrStream->Write(m_nDate);
				ptrStream->Write(m_nUserId);

				m_ptrPage->Save(ptrPageIO, nAddr);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to save transaction page %1", nAddr, excSrc);
				throw;
			}
		}

		void CTransactionPage::Read(storage::IPageIOPtr ptrPageIO, int64_t nAddr)
		{
			try
			{
				m_ptrPage->Read(ptrPageIO, nAddr);
				ReadMetaData();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to read transaction page %1", nAddr, excSrc);
				throw;
			}
		}
	}

}