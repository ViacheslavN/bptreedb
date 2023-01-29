#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/CommonLib_lib.h"
#include "../../../CommonLib/exception/exc_base.h"
#include "../../../CommonLib/alloc/alloc.h"
#include "../../../CommonLib/perf/PerfCount.h"
#include "../../storage/Storage.h"
#include "../../storage/FilePage.h"
#include "../../storage/FileStorage.h"
#include "../../storage/ReadStreamPage.h"
#include "../../storage/WriteStreamPage.h"

#include <iostream>


int64_t TestWrite(const astr& fileName, int pageSize, int cacheSize, int storageid, bool bCreate, int64_t nBegin, int64_t nEnd)
{
	try
	{
		CommonLib::IAllocPtr ptrAlloc = CommonLib::IAlloc::CreateSimpleAlloc();
		bptreedb::storage::IStoragePtr ptrStorage = std::make_shared<bptreedb::storage::CFileStorage>(ptrAlloc, storageid, cacheSize);
		ptrStorage->Open(fileName.c_str(), bCreate, 0, pageSize);
		int64_t nAddr = ptrStorage->GetNewFilePageAddr(pageSize);

		bptreedb::storage::TWriteStreamPage<bptreedb::storage::IPageIO> stream(ptrStorage, ptrAlloc, 1, 1, 1);

		stream.Open(nAddr, pageSize, bCreate);

		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			stream.Write(i);
		}

		stream.Close();
		ptrStorage->Flush();

		return nAddr;

	}
	catch (CommonLib::CExcBase& excSrc)
	{
		excSrc.AddMsg("Failed to test write");
		throw;
	}
}


void TestRead(const astr& fileName, int pageSize, int cacheSize, int storageid, int64_t nAddr, int64_t nBegin, int64_t nEnd)
{
	try
	{
		CommonLib::IAllocPtr ptrAlloc = CommonLib::IAlloc::CreateSimpleAlloc();
		bptreedb::storage::IStoragePtr ptrStorage = std::make_shared<bptreedb::storage::CFileStorage>(ptrAlloc, storageid, cacheSize);
		ptrStorage->Open(fileName.c_str(), false, 0, pageSize);

		bptreedb::storage::TReadStreamPage<bptreedb::storage::IPageIO> stream(ptrStorage, ptrAlloc);

		stream.Open(nAddr, pageSize);

		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			int64_t val =  stream.ReadInt64();
			if (val != i)
				throw CommonLib::CExcBase("Failed test read val: %1, i: %2", val, i);
		}


	}
	catch (CommonLib::CExcBase& excSrc)
	{
		excSrc.AddMsg("Failed to test write");
		throw;
	}
}


int main()
{
	try
	{
		int pageSize = 8192;
		int cacheSize = 5;
		int32_t storageId = 1;
		int64_t nBegin = 0;
		int64_t nEnd = 1024*1024;

		astr file = "D:\\streamTest";

		int nAddr = TestWrite(file, pageSize, cacheSize, storageId, true, nBegin, nEnd);
		TestRead(file, pageSize, cacheSize, storageId, nAddr, nBegin, nEnd);


	}
	catch (CommonLib::CExcBase& exc)
	{
		std::cout << "Exception: " << exc.what();
	}





	return 0;
}