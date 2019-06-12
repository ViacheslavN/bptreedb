#include "pch.h"


void TestWritePageStream(bptreedb::TStoragePtr pStorage, int64_t nPageAddr, uint32_t nPageSize, bool bCreate, int64_t nCount)
{
	bptreedb::utils::TWriteStreamPage<bptreedb::IStorage> stream(pStorage);
	stream.Open(nPageAddr, nPageSize, false, bCreate);


	for (uint64_t i = 0; i < nCount; ++i)
	{
		stream.Write(i);
	}
	stream.Close();
}

void TestReadPageStream(bptreedb::TStoragePtr pStorage, int64_t nPageAddr, uint32_t nPageSize, int64_t nCount)
{
	bptreedb::utils::TReadStreamPage<bptreedb::IStorage> stream(pStorage);
	stream.Open(nPageAddr, nPageSize, false);


	for (uint64_t i = 0; i < nCount; ++i)
	{
		int64_t  dd = stream.ReadInt64();

		if (dd != i)
			std::cout << "error";
	}
	 
}