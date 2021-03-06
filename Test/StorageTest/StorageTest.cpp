// StorageTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"


uint32_t nPageSize = 8192;

void TestWritePageStream(bptreedb::TStoragePtr pStorage, int64_t nPageAddr, uint32_t nPageSize, bool bCreate, int64_t nCount);
void TestReadPageStream(bptreedb::TStoragePtr pStorage, int64_t nPageAddr, uint32_t nPageSize, int64_t nCount);
int main()
{
	try
	{
		CommonLib::IAllocPtr pAlloc(new CommonLib::CSimpleAlloc(true));

		void* ptr = pAlloc->Alloc(nPageSize);
		pAlloc->Free(ptr);

		bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));

		pStorage->Open(L"F:\\stream_test.btdb", true, nPageSize);


		int64_t addr = pStorage->GetNewFilePageAddr(nPageSize);

		TestWritePageStream(pStorage, addr, nPageSize, true, 100000);
		TestWritePageStream(pStorage, addr, nPageSize, false, 50000);

		TestReadPageStream(pStorage, addr, nPageSize, 100000);




	}
	catch (CommonLib::CExcBase& exc)
	{
		std::cout << "Exception: " << exc.what();
	}

	return 0;


}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
