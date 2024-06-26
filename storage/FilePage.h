#pragma once
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/alloc/alloc.h"
#include "Storage.h"
#include "PageMemoryBuffer.h"

#include "PageObject.h"
#include "../utils/CRC.h"

namespace bptreedb
{
	namespace storage
	{

		template <class TIPage>
		class CBaseFilePagewithCRC : public TIPage
		{
			CBaseFilePagewithCRC(const CBaseFilePagewithCRC&);
			CBaseFilePagewithCRC(const CBaseFilePagewithCRC&&);
			CBaseFilePagewithCRC& operator = (const CBaseFilePagewithCRC&);
			CBaseFilePagewithCRC& operator = (const CBaseFilePagewithCRC&&);

		public:
			CBaseFilePagewithCRC(CommonLib::IAllocPtr ptrAlloc, uint32_t nSize, int64_t nAddr, uint32_t additionalOffset) : m_nAddr(nAddr)
			{
				m_ptrBuffer = std::make_shared<CPageMemoryBuffer>(ptrAlloc, nSize);
				m_ptrBuffer->SetOffset(page_header_size + additionalOffset);
			}

			CBaseFilePagewithCRC(CommonLib::IAllocPtr ptrAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr, uint32_t additionalOffset) : m_nAddr(nAddr)
			{
				m_ptrBuffer = std::make_shared<CPageMemoryBuffer>(ptrAlloc);
				m_ptrBuffer->AttachBuffer(pData, nSize);
				m_ptrBuffer->SetOffset(page_header_size + additionalOffset);

			}

			CBaseFilePagewithCRC(CPageMemoryBufferPtr ptrBuffer, int64_t nAddr, uint32_t additionalOffset) : m_nAddr(nAddr)
			{
				m_ptrBuffer = ptrBuffer;
				m_ptrBuffer->SetOffset(page_header_size + additionalOffset);
			}
	 
			virtual ~CBaseFilePagewithCRC(){}


			virtual int64_t GetAddr() const
			{
				return m_nAddr;
			}

			virtual void SetAddr(int64_t nAddr)
			{
				m_nAddr = nAddr;
			}

			virtual uint32_t GetSize() const
			{
				return GetFullPageSize();
			}

			virtual CommonLib::IMemoryWriteStreamPtr GetWriteStream()
			{
	
				CommonLib::IMemoryWriteStreamPtr ptrStream = std::make_shared<CommonLib::CFxMemoryWriteStream>();
				ptrStream->AttachBuffer(m_ptrBuffer);

				return ptrStream;
			}

			virtual CommonLib::IMemoryReadStreamPtr GetReadStream() const
			{
				CommonLib::IMemoryReadStreamPtr ptrStream = std::make_shared<CommonLib::CReadMemoryStream>();
				ptrStream->AttachBuffer(m_ptrBuffer);

				return ptrStream;
			}

			virtual void Save(IStorageIOPtr ptrPageIO, int64_t nAddr = -1)
			{
				try
				{
					if (nAddr == -1)
						nAddr = GetAddr();

					SaveAdditionalData();
					WriteCRC(GetFullData(), GetFullPageSize());
					ptrPageIO->WriteData(nAddr, GetFullData(), GetFullPageSize());
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("Failed to save file page addr: {0}", nAddr, excSrc);
					throw;
				}
			}

			virtual void Read(IStorageIOPtr ptrPageIO, int64_t nAddr = -1)
			{
				try
				{
					if (nAddr == -1)
						nAddr = GetAddr();

					ptrPageIO->ReadData(nAddr, m_ptrBuffer->GetFullData(), m_ptrBuffer->GetFullSize());
					CheckCRCAndThrow();
					ReadAdditionalData();
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("Failed to read file page addr: {0}", nAddr, excSrc);
					throw;
				}
			}


		protected:
			virtual void SaveAdditionalData() = 0;
			virtual void ReadAdditionalData() = 0;

			bool CheckCRC() const
			{
				return CheckCRC(GetFullData(), GetFullPageSize());
			}

			void CheckCRCAndThrow() const
			{
				if (!CheckCRC())
					throw CommonLib::CExcBase("Wrong crc for page addr {0}", m_nAddr);
			}

			uint32_t GetPageSize() const
			{
				return m_ptrBuffer->GetSize();
			}

			byte_t* GetFullData()
			{
				return  m_ptrBuffer->GetFullData();
			}

			const byte_t* GetFullData() const
			{
				return  m_ptrBuffer->GetFullData();
			}

			uint32_t GetFullPageSize() const
			{
				return m_ptrBuffer->GetFullSize();
			}

			byte_t* GetDataWithoutCRC() const
			{
				return m_ptrBuffer->GetFullData() + m_crc_offcet;
			}

			void WriteCRC(byte_t* pData, uint32_t nSize)
			{
				uint32_t calcCrc = (uint32_t)util::CCRC::Crc32(pData + sizeof(uint32_t), nSize - sizeof(uint32_t));
				memcpy(pData, &calcCrc, sizeof(uint32_t));
			}

			bool CheckCRC(const byte_t* pData, uint32_t nSize) const
			{
				uint32_t crc = 0;
				memcpy(&crc, pData, sizeof(uint32_t));
				uint32_t calcCrc = (uint32_t)util::CCRC::Crc32(pData + sizeof(uint32_t), nSize - sizeof(uint32_t));

				return crc == calcCrc;
			}

		protected:

			int64_t m_nAddr{ -1 };

			CPageMemoryBufferPtr m_ptrBuffer;
			static const uint32_t m_crc_offcet = sizeof(uint32_t);
			static const uint32_t page_header_size = m_crc_offcet;

		};



		typedef std::shared_ptr<class CFilePage> CFilePagePtr;
		class CFilePage : public CBaseFilePagewithCRC<IFilePage>
		{
			CFilePage(const CFilePage&);
			CFilePage(const CFilePage&&);
			CFilePage& operator = (const CFilePage&);
			CFilePage& operator = (const CFilePage&&);

			typedef CBaseFilePagewithCRC<IFilePage> TBase;

		public:
			CFilePage(CommonLib::IAllocPtr ptrAlloc, uint32_t nSize, int64_t nAddr) :
				TBase(ptrAlloc, nSize, nAddr, 0)
			{}

			CFilePage(CommonLib::IAllocPtr ptrAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr) :
				TBase(ptrAlloc, pData, nSize, nAddr, 0)
			{}

			CFilePage(CPageMemoryBufferPtr ptrBuffer, int64_t nAddr) :
				TBase(ptrBuffer, nAddr, 0)
			{}

			virtual ~CFilePage(){}

			virtual void SaveAdditionalData() {}
			virtual void ReadAdditionalData() {}

			static IFilePagePtr Read(IStorageIOPtr ptrPageIO, CommonLib::IAllocPtr ptrAlloc, int64_t nAddr);
		};
	}
}