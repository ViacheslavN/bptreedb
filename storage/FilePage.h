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
		typedef std::shared_ptr<class CFilePage> CFilePagePtr;

		class CFilePage
		{
			CFilePage(const CFilePage&);
			CFilePage(const CFilePage&&);
			CFilePage& operator = (const CFilePage&);
			CFilePage& operator = (const CFilePage&&);
		public:
			CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr, 
				uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype);

			CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr,
				uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype);

			CFilePage(CPageMemoryBufferPtr ptrBuffer, int64_t nAddr,
				uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype);

			CFilePage(CPageMemoryBufferPtr ptrBuffer, int64_t nAddr);


			~CFilePage();


			int64_t GetAddr() const;
			void SetAddr(int64_t nAddr);
			ObjectPageType GetObjectType() const;
			uint32_t GetObjectID() const;
			ObjectPageType GetParentType() const;
			uint32_t GetParentObjectID() const;
		
			CommonLib::IMemoryWriteStreamPtr GetWriteStream() const;
			CommonLib::IMemoryReadStreamPtr GetReadStream() const;
			void Save(IPageIOPtr ptrStorage);
			static CFilePagePtr Read(IPageIOPtr ptrStorage, std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr);
			static void  Read(IPageIOPtr ptrStorage, CFilePagePtr ptrPage, int64_t nAddr);

		private:
			bool CheckCRC() const;
			void CheckCRCAndThrow() const;
			bool CheckCRC(const byte_t* pData, uint32_t nSize) const;
			byte_t* GetDataWithoutCRC() const;
			void WriteCRC(byte_t* pData, uint32_t nSize);
			void ReloadHeader();

			byte_t* GetData();
			uint32_t GetPageSize() const;
			byte_t* GetFullData();
			const byte_t* GetFullData() const;
			uint32_t GetFullPageSize() const;

		private:

			int64_t m_nAddr{ -1 };
		 
			CPageMemoryBufferPtr m_ptrBuffer;

			uint32_t m_objectType{ 0 };
			uint32_t m_objectID{ 0 };
			uint32_t m_parentID{ 0 };
			uint32_t m_parentType{ 0 };

			static const uint32_t m_crc_offcet = sizeof(uint32_t);
			static const uint32_t m_metainfo_block_size = 50;
			static const uint32_t object_id_size = 4 * sizeof(uint32_t);
			static const uint32_t page_header_size = m_crc_offcet + object_id_size + m_metainfo_block_size;

		};
	}
}