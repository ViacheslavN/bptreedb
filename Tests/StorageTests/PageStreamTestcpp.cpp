#include "pch.h"


void TestWritePageStream(	bptreedb::storage::IFilePagerPtr pFilePager , int64_t nPageAddr, uint32_t nPageSize, bool bCreate, int64_t nCount)
{


	bptreedb::storage::TWriteStreamPage  stream(pFilePager);
	stream.Open(nPageAddr, bCreate);


	for (int64_t i = 0; i < nCount; ++i)
	{
		stream.Write(i);
	}
	stream.Close();
}

void TestReadPageStream(	bptreedb::storage::IFilePagerPtr pFilePager,  int64_t nPageAddr, uint32_t nPageSize, int64_t nCount)
{
	bptreedb::storage::TReadStreamPage stream(pFilePager);
	stream.Open(nPageAddr);


	for (int64_t i = 0; i < nCount; ++i)
	{
		int64_t  val = stream.ReadInt64();

		if (val != i)
			throw CommonLib::CExcBase("val != i, val: {0}, i: {1}", val, i);
	}
	 
}