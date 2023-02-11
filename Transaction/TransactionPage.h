#pragma once

#include "../../CommonLib/guid/guid.h"
#include "../storage/FilePage.h"
#include "Transaction.h"
#include "../utils/Int128.h"


namespace bptreedb
{
	namespace transaction
	{
		/*--------------------------------------------*/
		//FilePage
		//CRC
			//TranPage
				// Header
				// Lsn	
				// StorageId
				// Addr in Storage
				// Date
				// UseId
					//RowData



		/*--------------------------------------------*/

		class CTransactionPage : public ITransactionPage
		{
		public:
			CTransactionPage(CommonLib::IAllocPtr ptrAlloc, uint32_t nSize, int64_t nAddr, int32_t nStorageId,
				const util::CUInt128& lsn, int32_t nUserId);

			CTransactionPage(CommonLib::IAllocPtr ptrAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr, int32_t nStorageId,
				const util::CUInt128& lsn, int32_t nUserId);
	
			CTransactionPage(storage::IFilePagePtr ptrPage);


			virtual ~CTransactionPage();

			virtual int64_t GetAddr() const;
			virtual void SetAddr(int64_t nAddr);
			virtual uint32_t GetSize() const;
			virtual const byte_t* GetData() const;

			virtual CommonLib::IMemoryWriteStreamPtr GetWriteStream() const;
			virtual CommonLib::IMemoryReadStreamPtr GetReadStream() const ;

			virtual void Save(storage::IPageIOPtr ptrPageIO, int64_t nAddr);
			virtual void Read(storage::IPageIOPtr ptrPageIO, int64_t nAddr);

			virtual uint32_t GetStorageId() const;
			virtual uint32_t GetObjectID() const;
			virtual util::CUInt128 GetTransactionLSN() const;
			virtual uint64_t GetDate() const;
			virtual int32_t GetUserId() const;

		private:
			void ReadMetaData();
		private:
			storage::IFilePagePtr m_ptrPage;		

			//Transaction header
			util::CUInt128 m_nLSN;
			int32_t m_StorageId{ -1 };
			int64_t m_nAddr{ -1 };
			uint64_t m_nDate{ 0 };
			int32_t m_nUserId{ -1 };


			const uint32_t page_header_size =
				2 * sizeof(int64_t) //Transaction Lsn
				+ sizeof(int32_t) //storageid
				+ sizeof(uint64_t) //Addr in storage
				+ sizeof(uint64_t)	// Date of transaction completed
				+ sizeof(int32_t);	// userid
		};

		typedef std::shared_ptr<class CTransactionPage> CTransactionPagePtr;
	}
}