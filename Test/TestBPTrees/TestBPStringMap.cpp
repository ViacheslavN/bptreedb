#include "pch.h"
#include "PerfLog.h"
#include "StringVectorFile.h"




typedef bptreedb::TZLibEncoder<double> TValueEncoder;
typedef bptreedb::TZLibEncoder<int64_t> TLinkEncoder;
typedef bptreedb::BPTreeLeafNodeMap<bptreedb::StringValue, double, bptreedb::TBaseNodeCompressor<bptreedb::StringValue, double, bptreedb::CZlibStringComp, TValueEncoder > > TLeafNode;
typedef bptreedb::BPTreeInnerNode<bptreedb::StringValue, bptreedb::TBaseNodeCompressor<bptreedb::StringValue, int64_t, bptreedb::CZlibStringComp, TLinkEncoder > > TInnerNode;
 

typedef bptreedb::TBPSMap< double, bptreedb::CompStringValue, bptreedb::IStorage, TInnerNode, TLeafNode> TBPStringMap;







template <class TBPTree, class TStorage>
uint64_t CreateBPTreeStringMap(CommonLib::IAllocPtr pAlloc, TStorage pStorage)
{
	try
	{
		CommonLib::CLogInfo info(Log, "CreateBPStringTreeMap");

		TBPTree tree(-1, pStorage, pAlloc, 10, nPageSize, 25, false, false);

		bptreedb::TCompressorParamsBasePtr pCompParmas(new bptreedb::CompressorParamsBase());
		bptreedb::TCompressorParamsPtr pCompInnerKey(new bptreedb::CompressorParams());
		bptreedb::TCompressorParamsPtr pCompInnerValue(new bptreedb::CompressorParams());
		bptreedb::TCompressorParamsPtr pCompLeafKey(new bptreedb::CompressorParams());

		pCompInnerKey->SetIntParam("compressLevel", 9);
		pCompInnerKey->SetIntParam("compressRate", 5);

		pCompParmas->AddCompressParams(pCompInnerKey, bptreedb::eInnerKey);

		pCompInnerValue->SetIntParam("compressLevel", 9);
		pCompInnerValue->SetIntParam("compressRate", 5);

		pCompParmas->AddCompressParams(pCompInnerValue, bptreedb::eInnerValue);

		pCompLeafKey->SetIntParam("compressLevel", 9);
		pCompLeafKey->SetIntParam("compressRate", 5);

		pCompParmas->AddCompressParams(pCompLeafKey, bptreedb::eLeafKey);

		tree.InnitTree(pCompParmas,false, 0, bptreedb::eEmptyType, 0, bptreedb::eEmptyType);
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
void InsertBPTreeStringMap(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, uint32_t nCacheSize, const std::vector<astr>& vecStrs)
{
	try
	{
		int64_t nCount = vecStrs.size();
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "InsertBPTreeStringMap size %1", nCount);

		TBPTree tree(nBPTreePage, pStorage, pAlloc, nCacheSize, nPageSize, 25, false, false);
		tree.SetBPTreePerfCounter(pPerf);
		for (size_t i = 0; i < vecStrs.size(); ++i)
		{
			 
			size_t len = vecStrs[i].length();
			size_t size = vecStrs[i].size();

			if (i == vecStrs.size() -1)
			{
				int dd = 0;
				dd++;
			}

			tree.insert(vecStrs[i].c_str(), (uint32_t)len, (double)i);
		/*	auto it = tree.find(vecStrs[i].c_str(), (uint32_t)len);

			if (it.IsNull())
				throw CommonLib::CExcBase("Error find after1 insert %1, num %2", vecStrs[i], i);*/
 
			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}
		}

		for (size_t i = 0; i < vecStrs.size(); ++i)
		{
			size_t len = vecStrs[i].length();
			auto it = tree.find(vecStrs[i].c_str(), (uint32_t)len);

			if (it.IsNull())
				throw CommonLib::CExcBase("Error find after insert %1, num %2", vecStrs[i], i);
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
void FindBPTreeStringMap(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, uint32_t nCacheSize, const std::vector<astr>& vecStrs)
{
	try
	{
		int64_t nCount = vecStrs.size();
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "FindBPTreeStringMap size %1", nCount);

		TBPTree tree(nBPTreePage, pStorage, pAlloc, nCacheSize, nPageSize, 25, false, false);
		tree.SetBPTreePerfCounter(pPerf);
		for (size_t i = 0; i < vecStrs.size(); ++i)
		{
			size_t len = vecStrs[i].length();
			auto it = tree.find(vecStrs[i].c_str(), (uint32_t)len);

			if (it.IsNull())
				throw CommonLib::CExcBase("Error find %1, num %2", vecStrs[i], i);

			const bptreedb::StringValue& str = it.Key();
			astr key((const char*)it.Key().CStr());
			const astr& keyVal = vecStrs[i];

			size_t len1 = key.length();
			size_t len2 = keyVal.length();


			if (strcmp((const char*)str.CStr(), keyVal.c_str()) != 0)
				throw CommonLib::CExcBase("Error find key %1, vecValue %2, %3", key, keyVal, i);

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

template <class TBPTree, class TStorage>
void InfoBPStringTree(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, uint32_t nCacheSize)
{

	Log.Info("InfoBPTree");


	TBPTree tree(nBPTreePage, pStorage, pAlloc, nCacheSize, nPageSize, 25, false, false);
	tree.SetBPTreePerfCounter(pPerf);


	auto treeStat = tree.GetStatistics();
	treeStat->CalcNodesInTree();

	Log.InfoT("Inner Nodes: %1, Leaf Nodes: %2, Keys: %3", treeStat->m_nInnerNodeCount, treeStat->m_nLeafNodeCount, treeStat->m_nKeyCount);


	if (treeStat->m_setHeights.size() == 1)
		Log.InfoT("Height: %1, ", *treeStat->m_setHeights.begin());
	else
	{
		Log.Info(" Heights:");
		auto it = treeStat->m_setHeights.begin();
		auto end = treeStat->m_setHeights.end();

		for (; it != end; ++it)
		{
			Log.InfoT("%1", *it);
		}
	}
}


template <class TBPTree, class TStorage>
void TravelTree(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, uint32_t nCacheSize)
{
	try
	{
		CommonLib::CLogInfo info(Log, "Trave tree ");


		TBPTree tree(nBPTreePage, pStorage, pAlloc, nCacheSize, nPageSize, 25, false, false);
		tree.SetBPTreePerfCounter(pPerf);

		auto it = tree.begin();

		astr keyVal = "15115312543k671341";
		uint64_t nCount = 0;
		while (!it.IsNull())
		{
			const bptreedb::StringValue& str = it.Key();
			if (strcmp((const char*)str.CStr(), keyVal.c_str()) == 0)
			{
				std::cout << "Find  Addr: " << it.Addr() <<" Pos: " <<it.Pos() << "  ParentAddr: " <<  "\n";
			}
			nCount += 1;
			it.Next();
		}

		info.Complete("elements %1", nCount);
	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed to travel tree", exe);
	}
}

void TestBPTString()
{
	try
	{
		
		
	std::vector<astr> vecString;
	CStringVectorFile vecFile(L"F:\\StringTestData.strdata", 10000000);
	vecFile.ReadData(vecString, 10000000);
	//vecFile.ReadData(vecString, 1509730);
	//vecFile.ReadData(vecString, 492979);
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
	uint32_t nCacheSize = 10;
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
		InsertBPTreeStringMap<TBPStringMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nCacheSize, vecString);
	}
	
	{
		bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
		pStorage->Open(storagePath.c_str(), false, nPageSize);
		pStoragePerformer->Reset();
		pBPTreePerf->Reset();
		pStorage->SetStoragePerformer(pStoragePerformer);
		InfoBPStringTree<TBPStringMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nCacheSize);
	}
	{
		//nBPTreePage = 0;
		bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
		pStorage->Open(storagePath.c_str(), false, nPageSize);
		pStorage->SetStoragePerformer(pStoragePerformer);

		TravelTree<TBPStringMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nCacheSize);
	}

	{

		bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
		pStorage->Open(storagePath.c_str(), false, nPageSize);
		pStorage->SetStoragePerformer(pStoragePerformer);

		FindBPTreeStringMap<TBPStringMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nCacheSize, vecString);
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