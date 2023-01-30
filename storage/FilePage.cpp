#include "pch.h"
#include "FilePage.h"
#include "../utils/CRC.h"

namespace bptreedb
{
	namespace storage
	{
		CFilePage::CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr) :  m_nAddr(nAddr)
		{
			m_ptrBuffer = std::make_shared<CPageMemoryBuffer>(pAlloc, page_header_size);
			m_ptrBuffer->Create(nSize);
		}

		CFilePage::CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr) :
			 m_nAddr(nAddr)
		{
			m_ptrBuffer = std::make_shared<CPageMemoryBuffer>(pAlloc, page_header_size);
			m_ptrBuffer->AttachBuffer(pData, nSize);
		}

		CFilePage::CFilePage(CPageMemoryBufferPtr ptrBuffer, int64_t nAddr) : m_nAddr(nAddr)
		{
			m_ptrBuffer = ptrBuffer;
			m_ptrBuffer->SetOffset(page_header_size);
 		}

		CFilePage::~CFilePage()
		{
			
		}

		bool CFilePage::CheckCRC() const
		{
			return CheckCRC(GetFullData(), GetFullPageSize());
		}

		void CFilePage::CheckCRCAndThrow() const
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

		byte_t* CFilePage::GetData()
		{
			return m_ptrBuffer->GetData();
		}

		uint32_t CFilePage::GetPageSize() const
		{
			return m_ptrBuffer->GetSize();
		}

		byte_t* CFilePage::GetFullData()
		{
			return  m_ptrBuffer->GetFullData();
		}

		const byte_t* CFilePage::GetFullData() const
		{
			return  m_ptrBuffer->GetFullData();
		}

		uint32_t CFilePage::GetFullPageSize() const
		{
			return m_ptrBuffer->GetFullSize();
		}

		byte_t* CFilePage::GetDataWithoutCRC() const
		{
			return m_ptrBuffer->GetFullData() + m_crc_offcet;
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

		void CFilePage::WriteCRC(byte_t* pData, uint32_t nSize)
		{
			uint32_t calcCrc = (uint32_t)util::CCRC::Crc32(pData + sizeof(uint32_t), nSize - sizeof(uint32_t));
			memcpy(pData, &calcCrc, sizeof(uint32_t));
		}

		bool CFilePage::CheckCRC(const byte_t* pData, uint32_t nSize) const
		{
			uint32_t crc = 0;
			memcpy(&crc, pData, sizeof(uint32_t));
			uint32_t calcCrc = (uint32_t)util::CCRC::Crc32(pData + sizeof(uint32_t), nSize - sizeof(uint32_t));

			return crc == calcCrc;
		}

		

		void CFilePage::Save(IPageIOPtr ptrPageIO)
		{
			try
			{			
				WriteCRC(GetFullData(), GetFullPageSize());
				ptrPageIO->WriteData(m_nAddr, GetFullData(), GetFullPageSize());
			}		
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Filed to save file page addr: %1", m_nAddr, excSrc);
				throw;
			}
		}

		IFilePagePtr CFilePage::Read(IPageIOPtr ptrPageIO, std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr)
		{
			try
			{
				CPageMemoryBufferPtr ptrBuffer = std::make_shared<CPageMemoryBuffer>(pAlloc, page_header_size);
				ptrBuffer->Create(nSize);
				ptrPageIO->ReadData(nAddr, ptrBuffer->GetFullData(), ptrBuffer->GetFullSize());

				CFilePagePtr ptrPage = std::make_shared<CFilePage>(ptrBuffer, nAddr);
				ptrPage->CheckCRCAndThrow();

				return ptrPage;
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Filed to read file page addr: %1", nAddr, excSrc);
				throw;
			}
		}

		void  CFilePage::Read(IPageIOPtr ptrPageIO, int64_t nAddr)
		{
			try
			{
				ptrPageIO->ReadData(nAddr, m_ptrBuffer->GetFullData(), m_ptrBuffer->GetFullSize());
				CheckCRCAndThrow();
				SetAddr(nAddr);		 
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Filed to read file page addr: %1", nAddr, excSrc);
				throw;
			}
		}
	}
}