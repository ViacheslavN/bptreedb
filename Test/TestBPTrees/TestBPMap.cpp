#include "pch.h"
#include "PerfLog.h"


typedef bptreedb::TZLibEncoderDiff<int64_t> TKeyEncoder;
//typedef bptreedb::TZLibEncoder<double> TValueEncoder;


//typedef bptreedb::TEmptyValueEncoder<int64_t> TKeyEncoder;
typedef bptreedb::TEmptyValueEncoder<double> TValueEncoder;


typedef bptreedb::BPTreeLeafNodeMap<int64_t, double,  bptreedb::TBaseNodeCompressor<int64_t, double, TKeyEncoder, TValueEncoder > > TLeafNode;


typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t, TKeyEncoder, TKeyEncoder > > TInnerNode;
typedef bptreedb::TBPMap<int64_t, double, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPMap;


typedef TDataGenerator<int64_t, TRandomInegerGenerator<int64_t> > TTestDataGenerator;
typedef std::shared_ptr<TTestDataGenerator> TTestDataGeneratorPtr;


template <class TBPTree, class TStorage>
uint64_t CreateBPTreeMap(CommonLib::IAllocPtr pAlloc, TStorage pStorage)
{
	try
	{
		CommonLib::CLogInfo info(Log, "CreateBPTreeMap");

		TBPMap tree(-1, pStorage, pAlloc, 10, nPageSize);

		int nMaxCount = 8192 * 8;;

		bptreedb::TCompressorParamsBasePtr pCompParmas(new bptreedb::CompressorParamsBase());
		bptreedb::TCompressorParamsPtr pCompInnerKey(new bptreedb::CompressorParams());
		bptreedb::TCompressorParamsPtr pCompInnerValue(new bptreedb::CompressorParams());
		bptreedb::TCompressorParamsPtr pCompLeafKey(new bptreedb::CompressorParams());

		pCompInnerKey->SetIntParam("compressLevel", 3);
		pCompInnerKey->SetIntParam("compressRate", 4);
		pCompInnerKey->SetIntParam("MaxCount", nMaxCount);

		pCompParmas->AddCompressParams(pCompInnerKey, bptreedb::eInnerKey);

		pCompInnerValue->SetIntParam("compressLevel", 3);
		pCompInnerValue->SetIntParam("compressRate", 4);
		pCompInnerValue->SetIntParam("MaxCount", nMaxCount);

		pCompParmas->AddCompressParams(pCompInnerValue, bptreedb::eInnerValue);

		pCompLeafKey->SetIntParam("compressLevel", 3);
		pCompLeafKey->SetIntParam("compressRate", 4);
		pCompLeafKey->SetIntParam("MaxCount", nMaxCount);

		pCompParmas->AddCompressParams(pCompLeafKey, bptreedb::eLeafKey);

		tree.InnitTree(pCompParmas, false);
		tree.Flush();

		info.Complete();
		return tree.GetPageBTreeInfo();
	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed create BPTreeSet", exe);
		throw;
	}
}





template <class TBPTree, class TStorage>
void InsertBPTreeMap(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, uint32_t nNodeCache, int64_t nBegin, int64_t nEnd, TTestDataGeneratorPtr pDataGenerator)
{
	try
	{
		int64_t nCount = abs(nEnd - nBegin);
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "InsertBPTreeMap count: %1", nCount);

		TBPTree tree(nBPTreePage, pStorage, pAlloc, nNodeCache, nPageSize);
		tree.SetBPTreePerfCounter(pPerf);


		for (int64_t i = nBegin; i < nEnd; i++)
		{
				 
			int64_t val = (int64_t)pDataGenerator->GetValue((uint32_t)i);
			tree.insert(val, (double)i);
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
void FindBPTreeMap(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, int64_t nBegin, int64_t nEnd, TTestDataGeneratorPtr pDataGenerator)
{
	try
	{
		int64_t nCount = abs(nEnd - nBegin);
		int64_t nStep = nCount / 100;
		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "FindBPTreeMap begin count: %1", nCount);
		TBPTree tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize);
		tree.SetBPTreePerfCounter(pPerf);

		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			int64_t val = (int64_t)pDataGenerator->GetValue((uint32_t)i);
			auto it = tree.find(val);

			if (it.IsNull())
				throw CommonLib::CExcBase("Error find %1, idx %2", val, i);

			if (it.Key() != val)
				throw CommonLib::CExcBase("Error find key %1", it.Key(), val);

			if (it.Value() != (double)i)
				throw CommonLib::CExcBase("Error find value %1", it.Value(), i);

			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}
		}
	
		tree.Flush();

		info.Complete("FindBPTreeMap finish");
	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed FindBPTreeMap", exe);
	}
}



template <class TBPTree, class TStorage>
void DeleteFromBPTreeMap(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, uint32_t nNodeCache, int64_t nBegin, int64_t nEnd, TTestDataGeneratorPtr pDataGenerator)
{
	try
	{
		int64_t nCount = abs((nEnd - nBegin));
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "DeleteFromBPTreeMap start, count %1", nCount);

		TBPTree tree(nBPTreePage, pStorage, pAlloc, nNodeCache, nPageSize);
		tree.SetBPTreePerfCounter(pPerf);

		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			if (i == nEnd - 1)
			{
				int dd = 0;
				dd++;
			}

		

			int64_t val = (int64_t)pDataGenerator->GetValue((uint32_t)i);
			tree.remove(val);
			
			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}
		}
		
		Log.Info("Flush");
 
		tree.Flush();
 

		info.Complete("DeleteFromBPTreeMap finish");

	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed to insert BPTreeMap", exe);
	}
}


template <class TBPTree, class TStorage>
void TravelTree(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage)
{
	CommonLib::CLogInfo info(Log, "Trave tree ");


	TBPTree tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize);
	tree.SetBPTreePerfCounter(pPerf);

	auto it = tree.begin();
	uint32_t nCount = 0;
	while (!it.IsNull())
	{
		if (it.Key() == 4239185589)
		{
			Log.InfoT("found addr: %1, pos: %2", it.Addr(), it.Pos());
		}
		it.Next();
		

		nCount += 1;
	}
	info.Complete("elements %1", nCount);
}


template <class TBPTree, class TStorage>
void InfoBPTree(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage)
{

	Log.Info("InfoBPTree");


	TBPTree tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize);
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



void TestBPTreeMap()
{


	try
	{
#ifdef _WIN32
		wstr storagePath = L"F:\\BPMapTest.btdb";
		wstr dataPath = L"F:\\BPMapTestData.data";
#else
		bool bCycle = false;
		while (bCycle)
		{
			sleep(1);
		}


		wstr storagePath = L"/home/slava/BPMapTest";
#endif

		int64_t nBegin = 0;
		int64_t nEnd = 1000000;
		int64_t nBPTreePage = -1;
		uint32_t nNodeCache = 10;
		
		TTestDataGeneratorPtr pDataGenerator(new TTestDataGenerator((uint32_t)nEnd, dataPath));
		pDataGenerator->Open();


		CommonLib::IAllocPtr pAlloc(new CommonLib::CSimpleAlloc(true));
		CommonLib::TPrefCounterPtr pStoragePerformer(new CommonLib::CPerfCounter(10));
		CommonLib::TPrefCounterPtr pBPTreePerf(new CommonLib::CPerfCounter(10));
		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), true, nPageSize);
			pStorage->SetStoragePerformer(pStoragePerformer);
			nBPTreePage = CreateBPTreeMap<TBPMap, bptreedb::TStoragePtr>(pAlloc, pStorage);
			//LogPerf(pStoragePerformer);
		}

		{

			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);
			InsertBPTreeMap<TBPMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nNodeCache, nBegin, nEnd, pDataGenerator);

			pStorage->Close();

			CommonLib::file::TFilePtr file = CommonLib::file::CFileCreator::OpenFile(storagePath.c_str(), CommonLib::file::ofmOpenExisting, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
			//std::cout << "Storage size: " << file->GetFileSize() << "\n";
			Log.InfoT("Storage size: %1", file->GetFileSize());
			CPerfLog::LogBPtreeInsertPerf(pBPTreePerf);
			CPerfLog::LogBPtreePerf(pBPTreePerf);
			CPerfLog::LogPerfStorage(pStoragePerformer);
		}


		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);
			TravelTree<TBPMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage);
		}

		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);
			InfoBPTree<TBPMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage);
		}


		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);

			FindBPTreeMap<TBPMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nBegin, nEnd, pDataGenerator);

			CPerfLog::LogBPtreeFindPerf(pBPTreePerf);
			CPerfLog::LogBPtreePerf(pBPTreePerf);
			CPerfLog::LogBPtreeRemovePerf(pBPTreePerf);
			CPerfLog::LogPerfStorage(pStoragePerformer);
		}


		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);

			DeleteFromBPTreeMap<TBPMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nNodeCache, nBegin, nEnd / 2, pDataGenerator);


			CPerfLog::LogBPtreeFindPerf(pBPTreePerf);
			CPerfLog::LogBPtreePerf(pBPTreePerf);
			CPerfLog::LogBPtreeRemovePerf(pBPTreePerf);
			CPerfLog::LogPerfStorage(pStoragePerformer);
		}
		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);
			InfoBPTree<TBPMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage);
		}


		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);

			FindBPTreeMap<TBPMap, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nEnd / 2 + 1, nEnd, pDataGenerator);

			CPerfLog::LogBPtreeFindPerf(pBPTreePerf);
			CPerfLog::LogBPtreePerf(pBPTreePerf);
			CPerfLog::LogBPtreeRemovePerf(pBPTreePerf);
			CPerfLog::LogPerfStorage(pStoragePerformer);
		}
		 
	}
	catch (std::exception& exe)
	{
		std::cout << "TestBPTreeSet failed: " << "\n";

		astrvec msgChain = CommonLib::CExcBase::GetChainFromExc(exe);


		for (size_t i = 0, sz = msgChain.size(); i < sz; ++i)
		{
			if (i != 0)
				std::cout << "\r\n";

			std::cout << msgChain[i];
		}

	}

}