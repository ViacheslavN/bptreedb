#pragma once

#include "../storage/FilePage.h"

namespace bptreedb
{
	namespace transaction
	{

		class ITransactionPage : public storage::IFilePage
		{
		public:
			ITransactionPage(){}
			virtual ~ITransactionPage(){}

			virtual uint32_t GetObjectType() const = 0;
			virtual uint32_t GetObjectID() const = 0;
			virtual uint32_t GetParentType() const = 0;
			virtual uint32_t GetParentObjectID() const = 0;
		};


		class CTransactionPage : public ITransactionPage
		{
		public:
			CTransactionPage(CommonLib::IAllocPtr ptrAlloc, uint32_t nSize, int64_t nAddr,
				int64_t nTransactionId, int32_t nUserId,
				uint32_t objectID, uint32_t objecttype, uint32_t parentID, uint32_t parenttype);

			CTransactionPage(CommonLib::IAllocPtr ptrAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr,
				int64_t nTransactionId, int32_t nUserId,
				uint32_t objectID, uint32_t objecttype, uint32_t parentID, uint32_t parenttype);

			CTransactionPage(storage::CPageMemoryBufferPtr ptrBuffer, int64_t nAddr,
				int64_t nTransactionId, int32_t nUserId,
				uint32_t objectID, uint32_t objecttype, uint32_t parentID, uint32_t parenttype);

			CTransactionPage(storage::CPageMemoryBufferPtr ptrBuffer, int64_t nAddr);


			virtual ~CTransactionPage();



			virtual int64_t GetAddr() const;
			virtual void SetAddr(int64_t nAddr);
			virtual uint32_t GetObjectType() const;
			virtual uint32_t GetObjectID() const;
			virtual uint32_t GetParentType() const;
			virtual uint32_t GetParentObjectID() const;
			virtual CommonLib::IMemoryWriteStreamPtr GetWriteStream() const;
			virtual CommonLib::IMemoryReadStreamPtr GetReadStream() const ;

			virtual void Save(storage::IPageIOPtr ptrPageIO);
			virtual void Read(storage::IPageIOPtr ptrPageIO, int64_t nAddr);

		private:
			void ReadMetaData();
		private:
			storage::CPageMemoryBufferPtr m_ptrBuffer;
			storage::CFilePagePtr m_ptrPage;

			//Transaction header
			int64_t m_nTransactionId{ -1 };
			uint64_t m_nDate{ 0 };
			int32_t m_nUserId{ -1 };

			//Object header
			uint32_t m_objectType{ 0 };
			uint32_t m_objectID{ 0 };
			uint32_t m_parentID{ 0 };
			uint32_t m_parentType{ 0 };

			const uint32_t page_header_size = sizeof(int64_t) + sizeof(uint64_t) + 5 * sizeof(uint32_t);

		};

		typedef std::shared_ptr<class CTransactionPage> CTransactionPagePtr;
	}
}