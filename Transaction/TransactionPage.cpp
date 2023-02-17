#include "pch.h"
#include "TransactionPage.h"
#include "../../CommonLib/date/DateUtil.h"

namespace bptreedb
{
	namespace transaction
	{

		CTransactionPage::CTransactionPage(CommonLib::IAllocPtr ptrAlloc, uint32_t nSize, int64_t nAddr, int32_t nStorageId,
			const util::CUInt128& lsn, int32_t nUserId) : 
			TBase(ptrAlloc, nSize, nAddr, page_header_size),
			m_nLSN(lsn), m_nUserId(m_nUserId), m_StorageId(nStorageId) 
				
		{
		 
		}

		CTransactionPage::CTransactionPage(CommonLib::IAllocPtr ptrAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr, int32_t nStorageId,
			const util::CUInt128& lsn, int32_t nUserId) :
			TBase(ptrAlloc, pData,  nSize, nAddr, page_header_size),
			m_nLSN(lsn), m_nUserId(m_nUserId), m_StorageId(nStorageId)
		{

		}

		CTransactionPage::~CTransactionPage()
		{

		}

 

		void CTransactionPage::ReadMetaData()
		{
			CommonLib::CReadMemoryStream readStream;
			readStream.AttachBuffer(m_ptrBuffer->GetFullData() + page_header_size, m_ptrBuffer->GetSize() - page_header_size);
			m_nLSN.m_hi = readStream.ReadIntu64(); 
			m_nLSN.m_lo = readStream.ReadIntu64();
			m_StorageId = readStream.ReadInt32();
			readStream.ReadInt64(); //nAddr
			m_nDate = readStream.ReadIntu64();
			m_nUserId = readStream.ReadInt32();
		}
		

		int32_t CTransactionPage::GetStorageId() const
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

		void CTransactionPage::SaveAdditionalData()
		{
			CommonLib::CFxMemoryWriteStream writeStream;
			writeStream.AttachBuffer(m_ptrBuffer->GetFullData() + page_header_size, m_ptrBuffer->GetSize() - page_header_size);
			writeStream.Write(m_nLSN.m_hi);
			writeStream.Write(m_nLSN.m_lo);
			writeStream.Write(m_StorageId);
			writeStream.Write(m_nAddr);
			m_nDate = CommonLib::dateutil::CDateUtil::GetCurrentDateTime();
			writeStream.Write(m_nDate);
			writeStream.Write(m_nUserId);
		}

		void CTransactionPage::ReadAdditionalData()
		{
			ReadMetaData();
		}
	}

}