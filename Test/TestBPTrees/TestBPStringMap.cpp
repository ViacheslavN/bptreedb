#include "pch.h"
#include "PerfLog.h"




typedef bptreedb::TZLibEncoder<double> TValueEncoder;
typedef bptreedb::TZLibEncoder<int64_t> TLinkEncoder;
typedef bptreedb::BPTreeLeafNodeMap<bptreedb::StringValue, double, bptreedb::TBaseNodeCompressor<bptreedb::StringValue, double, bptreedb::CZlibStringComp, TValueEncoder > > TLeafNode;


typedef bptreedb::BPTreeInnerNode<bptreedb::StringValue, bptreedb::TBaseNodeCompressor<bptreedb::StringValue, int64_t, bptreedb::CZlibStringComp, TLinkEncoder > > TInnerNode;
typedef bptreedb::TBPSMap< double, bptreedb::CompStringValue, bptreedb::IStorage, TInnerNode, TLeafNode> TBPStringMap;


void GetRandomValues(byte *pData, uint32_t nSize)
{
	if (!nSize)
		return;


	for (uint32_t i = 0; i < nSize; ++i)
	{
		uint32_t val = (rand() % 10 + 1);

		if (val > 9)
			pData[i] = byte_t(val) + 'a';
		else
			pData[i] = byte_t(val) + '0';
	}

}





template <class TBPTree, class TStorage>
uint64_t CreateBPTreeStringMap(CommonLib::IAllocPtr pAlloc, TStorage pStorage)
{
	try
	{
		CommonLib::CLogInfo info(Log, "CreateBPStringTreeMap");

		TBPTree tree(-1, pStorage, pAlloc, 10, nPageSize, 25);

		bptreedb::TCompressorParamsBasePtr pCompParmas(new bptreedb::CompressorParamsBase());
		bptreedb::TCompressorParamsPtr pCompInnerKey(new bptreedb::CompressorParams());
		bptreedb::TCompressorParamsPtr pCompInnerValue(new bptreedb::CompressorParams());
		bptreedb::TCompressorParamsPtr pCompLeafKey(new bptreedb::CompressorParams());

		pCompInnerKey->SetIntParam("compressLevel", 9);
		pCompInnerKey->SetIntParam("compressRate", 10);

		pCompParmas->AddCompressParams(pCompInnerKey, bptreedb::eInnerKey);

		pCompInnerValue->SetIntParam("compressLevel", 9);
		pCompInnerValue->SetIntParam("compressRate", 3);

		pCompParmas->AddCompressParams(pCompInnerValue, bptreedb::eInnerValue);

		pCompLeafKey->SetIntParam("compressLevel", 9);
		pCompLeafKey->SetIntParam("compressRate", 10);

		pCompParmas->AddCompressParams(pCompLeafKey, bptreedb::eLeafKey);

		tree.InnitTree(pCompParmas, false);
		tree.Flush();

		info.Complete();
		return tree.GetPageBTreeInfo();
	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed create BPTStringMap", exe);
		throw;
	}
}





template <class TBPTree, class TStorage>
void InsertBPTreeStringMap(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, const std::vector<astr>& vecStrs)
{
	try
	{
		int64_t nCount = vecStrs.size();
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "InsertBPTreeStringMap size %1", nCount);

		TBPTree tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize, 25);
		tree.SetBPTreePerfCounter(pPerf);
		for (size_t i = 0; i < vecStrs.size(); ++i)
		{
			 
			size_t len = vecStrs[i].length();
			size_t size = vecStrs[i].size();
			tree.insert(vecStrs[i].c_str(), (uint32_t)len, (double)i);
 
			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}
		}

		tree.Flush();

		info.Complete("InsertBPTreeSet finish");

	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed to insert BPTreeSet", exe);
	}
}


template <class TBPTree, class TStorage>
void FindBPTreeStringMap(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, const std::vector<astr>& vecStrs)
{
	try
	{
		int64_t nCount = vecStrs.size();
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "FindBPTreeStringMap size %1", nCount);

		TBPTree tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize, 25);
		tree.SetBPTreePerfCounter(pPerf);
		for (size_t i = 0; i < vecStrs.size(); ++i)
		{
			size_t len = vecStrs[i].length();
			auto it = tree.find(vecStrs[i].c_str(), (uint32_t)len);

			if (it.IsNull())
				throw CommonLib::CExcBase("Error find %1", vecStrs[i]);

			const bptreedb::StringValue& str = it.Key();
			astr key((const char*)it.Key().CStr());
			const astr& keyVal = vecStrs[i];

			size_t len1 = key.length();
			size_t len2 = keyVal.length();


			if (strcmp((const char*)str.CStr(), keyVal.c_str()) != 0)
				throw CommonLib::CExcBase("Error find key %1", key, i);

			if (it.Value() != (double)i)
				throw CommonLib::CExcBase("Error find value %1", it.Value(), i);

			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}

			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}
		}

		tree.Flush();

		info.Complete("FindBPTreeSet finish");

	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed to find BPTreeSet", exe);
	}
}


void TestBPTString()
{
	try
	{

	std::vector<astr> vecString;
	int count = 100000;
	char rndstr[15];
	memset(rndstr, 0, sizeof(rndstr));

	srand((int)time(0));

	for (int i = 0; i < count; ++i)
	{
		astr str = CommonLib::str_utils::AStrInt32(i);

		GetRandomValues((byte *)rndstr, sizeof(rndstr));

		rndstr[sizeof(rndstr) - 1] = '\0';

		str += rndstr;

		astr utf8 = CommonLib::StringEncoding::str_a2utf8(str);
		vecString.push_back(utf8);
	}


#ifdef _WIN32
	wstr storagePath = L"F:\\BPMapTest.btdb";
#else
	bool bCycle = false;
	while (bCycle)
	{
		sleep(1);
	}


	wstr storagePath = L"/home/slava/BPMapTest";
#endif
	int64_t nBPTreePage = -1;
	CommonLib::IAllocPtr pAlloc(new CommonLib::CSimpleAlloc(true));
	CommonLib::TPrefCounterPtr pStoragePerformer(new CommonLib::CPerfCounter(10));
	CommonLib::TPrefCounterPtr pBPTreePerf(new CommonLib::CPerfCounter(10));
	{
		bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
		pStorage->Open(storagePath.c_str(), true, nPageSize);
		pStorage->SetStoragePerformer(pStoragePerformer);
		nBPTreePage = CreateBPTreeStringMap<TBPStringMap, bptreedb::TStoragePtr>(pAlloc, pStorage);
		//LogPerf(pStoragePerformer);
	}
	{

		bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
		pStorage->Open(storagePath.c_str(), false, nPageSize);
		pStorage->SetStoragePerformer(pStoragePerformer);
		InsertBPTreeStringMap<TBPStringMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, vecString);
	}

	{

		bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
		pStorage->Open(storagePath.c_str(), false, nPageSize);
		pStorage->SetStoragePerformer(pStoragePerformer);
		FindBPTreeStringMap<TBPStringMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, vecString);
	}

	

	}
	catch (std::exception& exe)
	{
		std::cout << "TestBPTStringMap failed: " << "\n";

		astrvec msgChain = CommonLib::CExcBase::GetChainFromExc(exe);


		for (size_t i = 0, sz = msgChain.size(); i < sz; ++i)
		{
			if (i != 0)
				std::cout << "\r\n";

			std::cout << msgChain[i];
		}

	}
}