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
#include "Pager.h"


int64_t TestWrite(const astr& fileName, int pageSize, int cacheSize, int storageid, int nStreamAddr, int64_t nBegin, int64_t nEnd)
{
	try
	{
		CommonLib::IAllocPtr ptrAlloc = CommonLib::IAlloc::CreateSimpleAlloc();
		bptreedb::storage::IStoragePtr ptrStorage = std::make_shared<bptreedb::storage::CFileStorage>(ptrAlloc, storageid, cacheSize);
		ptrStorage->Open(fileName.c_str(), nStreamAddr == -1, 0, pageSize);
		int64_t nAddr = nStreamAddr == -1 ? ptrStorage->GetNewFilePageAddr(pageSize) : nStreamAddr;

		CPagerPtr ptrPager = std::make_shared<CPager>(ptrStorage, ptrAlloc, pageSize);

		bptreedb::storage::TWriteStreamPage stream(ptrPager);

		stream.Open(nAddr, nStreamAddr == -1);

		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			stream.Write(i);
		}

		stream.Close();
		ptrStorage->Flush();

		return nAddr;

	}
	catch (std::exception& excSrc)
	{
		CommonLib::CExcBase::RegenExcT("Failed to test write", excSrc);
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

		CPagerPtr ptrPager = std::make_shared<CPager>(ptrStorage, ptrAlloc, pageSize);

		bptreedb::storage::TReadStreamPage stream(ptrPager);

		stream.Open(nAddr);

		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			int64_t val =  stream.ReadInt64();
			if (val != i)
				throw CommonLib::CExcBase("val: {0}, i: {1}", val, i);
		}

	}
	catch (std::exception& excSrc)
	{
		CommonLib::CExcBase::RegenExcT("Failed test read ", excSrc);
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

		int64_t nAddr = -1;

		nAddr = TestWrite(file, pageSize, cacheSize, storageId, nAddr, nBegin, nEnd);
		TestRead(file, pageSize, cacheSize, storageId, nAddr, nBegin, nEnd);
		TestWrite(file, pageSize, cacheSize, storageId, nAddr, nBegin, nEnd);
		TestRead(file, pageSize, cacheSize, storageId, nAddr, nBegin, nEnd);

	}
	catch (std::exception& exc)
	{
		std::cout << "Exception: " << exc.what();
	}





	return 0;
}