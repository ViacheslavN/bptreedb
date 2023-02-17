#include "pch.h"
#include "FilePage.h"


namespace bptreedb
{
	namespace storage
	{
		

		IFilePagePtr CFilePage::Read(IStorageIOPtr ptrPageIO, CommonLib::IAllocPtr ptrAlloc, int64_t nAddr)
		{
			try
			{
				CPageMemoryBufferPtr ptrBuffer = std::make_shared<CPageMemoryBuffer>(ptrAlloc, ptrPageIO->GetPageSize());
				ptrPageIO->ReadData(nAddr, ptrBuffer->GetFullData(), ptrBuffer->GetFullSize());

				CFilePagePtr ptrPage = std::make_shared<CFilePage>(ptrBuffer, nAddr);
				ptrPage->CheckCRCAndThrow();
				
				return std::static_pointer_cast<storage::IFilePage>(ptrPage);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed to read file page addr: %1", nAddr, excSrc);
				throw;
			}
		}
		
	}
}