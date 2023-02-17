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

		class CTransactionPage : public storage::CBaseFilePagewithCRC<ITransactionPage>
		{

			typedef storage::CBaseFilePagewithCRC<ITransactionPage> TBase;
		public:
			CTransactionPage(CommonLib::IAllocPtr ptrAlloc, uint32_t nSize, int64_t nAddr, int32_t nStorageId,
				const util::CUInt128& lsn, int32_t nUserId);

			CTransactionPage(CommonLib::IAllocPtr ptrAlloc, byte_t* pData, uint32_t nSize, int64_t nAddr, int32_t nStorageId,
				const util::CUInt128& lsn, int32_t nUserId);
	
			virtual ~CTransactionPage();


			virtual int32_t GetStorageId() const;
			virtual uint32_t GetObjectID() const;
			virtual util::CUInt128 GetTransactionLSN() const;
			virtual uint64_t GetDate() const;
			virtual int32_t GetUserId() const;
		protected:
			virtual void SaveAdditionalData();
			virtual void ReadAdditionalData();

		private:
			void ReadMetaData();
		private:

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