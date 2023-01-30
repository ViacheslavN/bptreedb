#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/stream/FixedMemoryStream.h"
#include "../../CommonLib/stream/MemoryStream.h"
#include "../../CommonLib/stream/StreamBaseEmpty.h"
#include "FilePage.h"

namespace bptreedb
{
	namespace storage
	{
	
		class TWriteStreamPage : public CommonLib::TMemoryStreamBaseEmpty<CommonLib::IMemoryWriteStream>
		{
		public:		 

			TWriteStreamPage(IFilePagerPtr ptrIFilePager);
			virtual ~TWriteStreamPage();

			void Open(int64_t nPageAddr, bool bCreate);
			virtual std::streamsize WriteBytes(const byte_t* buffer, size_t size);
			virtual std::streamsize WriteInverse(const byte_t* buffer, size_t size);
			virtual bool IsEnoughSpace(size_t size) const;
			virtual void Close();

		private:
			void write_bytes_impl(const byte_t* buffer, size_t size, bool bInvers);
			void OpenNextPage(int64_t nPageAddr, bool bCreate);

		private:
			int64_t m_nNextAddr{ -1 };
			int64_t m_nPrevAddr{ -1 };

			IFilePagerPtr m_ptrIFilePager;
			IFilePagePtr m_ptrCurrentPage;
			CommonLib::IMemoryWriteStreamPtr m_ptrStream;
		};


	}
}