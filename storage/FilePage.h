#pragma once
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/alloc/alloc.h"
#include "Storage.h"
#include "../../CommonLib/stream/FixedMemoryStream.h"
#include "PageMemoryBuffer.h"

#include "PageObject.h"

namespace bptreedb
{
	namespace storage
	{

		class IFilePage
		{
		public:

			IFilePage(){}
			virtual ~IFilePage(){}

			virtual int64_t GetAddr() const = 0;
			virtual void SetAddr(int64_t nAddr) = 0;
			virtual CommonLib::IMemoryWriteStreamPtr GetWriteStream() const = 0;
			virtual CommonLib::IMemoryReadStreamPtr GetReadStream() const = 0;

			virtual void Save(IPageIOPtr ptrPageIO) = 0;
			virtual void Read(IPageIOPtr ptrPageIO, int64_t nAddr) = 0;
		};

		typedef std::shared_ptr<class IFilePage> IFilePagePtr;
		typedef std::shared_ptr<class CFilePage> CFilePagePtr;
		typedef std::shared_ptr<class IFilePager> IFilePagerPtr;

		class IFilePager
		{
		public:

			IFilePager() {}
			virtual ~IFilePager() {}

			virtual int64_t GetNewFilePageAddr() = 0;
			virtual IFilePagePtr GetNewPage(int64_t nAddr) = 0;
			virtual IFilePagePtr ReadPage(int64_t nAddr) = 0;
			virtual void ReadPage(IFilePagePtr ptrPage, int64_t nAddr) = 0;
			virtual void SavePage(IFilePagePtr ptrPage) = 0;
		};

	 
		class CFilePage : public IFilePage
		{
			CFilePage(const CFilePage&);
			CFilePage(const CFilePage&&);
			CFilePage& operator = (const CFilePage&);
			CFilePage& operator = (const CFilePage&&);
		public:
			CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr);
			CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr);
			CFilePage(CPageMemoryBufferPtr ptrBuffer, int64_t nAddr);

			~CFilePage();


			virtual int64_t GetAddr() const;
			virtual void SetAddr(int64_t nAddr);	
			virtual CommonLib::IMemoryWriteStreamPtr GetWriteStream() const;
			virtual CommonLib::IMemoryReadStreamPtr GetReadStream() const;
			virtual void Save(IPageIOPtr ptrPageIO);
			virtual void Read(IPageIOPtr ptrPageIO, int64_t nAddr);


			static IFilePagePtr Read(IPageIOPtr ptrPageIO, std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr);
		 

		private:
			bool CheckCRC() const;
			void CheckCRCAndThrow() const;
			bool CheckCRC(const byte_t* pData, uint32_t nSize) const;
			byte_t* GetDataWithoutCRC() const;
			void WriteCRC(byte_t* pData, uint32_t nSize);
			byte_t* GetData();
			uint32_t GetPageSize() const;
			byte_t* GetFullData();
			const byte_t* GetFullData() const;
			uint32_t GetFullPageSize() const;

		private:

			int64_t m_nAddr{ -1 };
		 
			CPageMemoryBufferPtr m_ptrBuffer;


			static const uint32_t m_crc_offcet = sizeof(uint32_t);
	 		static const uint32_t page_header_size = m_crc_offcet;

		};
	}
}