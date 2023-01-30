#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/stream/stream.h"
#include "../../CommonLib/stream/StreamBaseEmpty.h"
#include "FilePage.h"

namespace bptreedb
{
	namespace storage
	{

 
		class TReadStreamPage : public CommonLib::TMemoryStreamBaseEmpty<CommonLib::IMemoryReadStream>
		{
		public:
 

			TReadStreamPage(IFilePagerPtr ptrIFilePager);
			virtual ~TReadStreamPage();

			void Open(int64_t nPageAddr);
			virtual std::streamsize ReadBytes(byte_t* dst, size_t size);
			virtual std::streamsize ReadInverse(byte_t* dst, size_t size);
			virtual bool ReadBytesSafe(byte_t* dst, size_t size);
			virtual bool ReadInverseSafe(byte_t* dst, size_t size);
			virtual bool IsEnoughSpace(size_t size) const;

		private:
			void read_bytes_impl(byte_t* buffer, size_t size, bool bInvers);
			void OpenNextPage(int64_t nPageAddr);

		private:
			int64_t m_nNextAddr{ -1 }; 
			IFilePagerPtr m_ptrIFilePager;
			IFilePagePtr m_ptrCurrentPage; 
			CommonLib::IMemoryReadStreamPtr m_ptrStream;
		};
	}
}
