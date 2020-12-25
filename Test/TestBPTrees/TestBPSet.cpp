#include "pch.h"

uint32_t nPageSize = 8192;

template<class _Ty>
struct comp
{
	bool LE(const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left < _Right);
	}
	bool EQ(const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left == _Right);
	}
	bool operator() (const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left < _Right);
	}
	/*bool MR(const _Ty& _Left, const _Ty& _Right){
	return (_Left > _Right);
	}*/
};

/*
typedef bptreedb::TZstdEncoder<int64_t> TEncoder;
typedef bptreedb::BPTreeLeafNode<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t, TEncoder> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t, TEncoder, TEncoder> > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;
*/

/*
typedef bptreedb::TZstdEncoderDiff<int64_t> TEncoder;
typedef bptreedb::BPTreeLeafNode<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t, TEncoder> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t, TEncoder, TEncoder > > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;*/



typedef bptreedb::TZLibEncoderDiff<int64_t> TEncoder;
typedef bptreedb::BPTreeLeafNode<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t, TEncoder> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t, TEncoder, TEncoder > > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;


/*
typedef bptreedb::TZLibEncoder<int64_t> TEncoder;
typedef bptreedb::BPTreeLeafNode<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t, TEncoder> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t, TEncoder, TEncoder > > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;*/

/*
typedef bptreedb::BPTreeLeafNode<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t> > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;*/



void LogPerfStorage(CommonLib::TPrefCounterPtr pStoragePerformer)
{

	Log.Info("------STORAGE PERF------");
	Log.InfoT("read io count %1, read data %2, total time %3", pStoragePerformer->GetCountOperation(bptreedb::IStorage::eReadData), pStoragePerformer->GetDataOperation(bptreedb::IStorage::eReadData), pStoragePerformer->GetTimeOperation(bptreedb::IStorage::eReadData));
	Log.InfoT("write io count %1, write data %2, total time %3", pStoragePerformer->GetCountOperation(bptreedb::IStorage::eWriteData), pStoragePerformer->GetDataOperation(bptreedb::IStorage::eWriteData), pStoragePerformer->GetTimeOperation(bptreedb::IStorage::eWriteData));

}

void LogBPtreeInsertPerf(CommonLib::TPrefCounterPtr pPerf)
{
	Log.Info("------BPTREE INSERT------");
	Log.InfoT("insert total time: %1,", pPerf->GetTimeOperation(bptreedb::eInsertValue));
	Log.InfoT("Flush total time %1", pPerf->GetTimeOperation(bptreedb::eFlush));
}

void LogBPtreePerf(CommonLib::TPrefCounterPtr  pPerf)
{
	Log.Info("------BPTREE NODE------");
	Log.InfoT("save node counts: %1, missed %2, total time %3", pPerf->GetCountOperation(bptreedb::eSaveNode), pPerf->GetCountOperation(bptreedb::eMissedNode), pPerf->GetTimeOperation(bptreedb::eSaveNode));
	Log.InfoT("load node %1, total time %2", pPerf->GetCountOperation(bptreedb::eLoadNode), pPerf->GetTimeOperation(bptreedb::eLoadNode));
	Log.InfoT("get node %1, total time %2", pPerf->GetCountOperation(bptreedb::eGetNode), pPerf->GetTimeOperation(bptreedb::eGetNode));
}

void LogBPtreeRemovePerf(CommonLib::TPrefCounterPtr pPerf)
{
	Log.Info("------BPTREE DELETE------");
	Log.InfoT("remove total time: %1,", pPerf->GetTimeOperation(bptreedb::eRemoveValue));
	Log.InfoT("Flush total time %1", pPerf->GetTimeOperation(bptreedb::eFlush));
}

void LogBPtreeFindPerf(CommonLib::TPrefCounterPtr pPerf)
{
	Log.Info("------BPTREE FIND------");
	Log.InfoT("find key total time: %1,", pPerf->GetTimeOperation(bptreedb::eFindKey));
}


template <class TBPTree, class TStorage>
uint64_t CreateBPTreeSet(CommonLib::IAllocPtr pAlloc, TStorage pStorage)
{
	try
	{
		CommonLib::CLogInfo info(Log, "CreateBPTreeSet");

		TBPSet tree(-1, pStorage, pAlloc, 10, nPageSize);
	
		bptreedb::TCompressorParamsBasePtr pCompParmas (new bptreedb::CompressorParamsBase());
		bptreedb::TCompressorParamsPtr pCompInnerKey(new bptreedb::CompressorParams());
		bptreedb::TCompressorParamsPtr pCompInnerValue(new bptreedb::CompressorParams());
		bptreedb::TCompressorParamsPtr pCompLeafKey(new bptreedb::CompressorParams());

		pCompInnerKey->SetIntParam("compressLevel", 9);
		pCompInnerKey->SetIntParam("compressLevel", 9);
		pCompInnerKey->SetIntParam("compressRate", 10);

		pCompParmas->AddCompressParams(pCompInnerKey, bptreedb::eInnerKey);

		pCompInnerValue->SetIntParam("compressLevel", 9);
		pCompInnerValue->SetIntParam("compressLevel", 9);
		pCompInnerValue->SetIntParam("compressRate", 3);

		pCompParmas->AddCompressParams(pCompInnerValue, bptreedb::eInnerValue);

		pCompLeafKey->SetIntParam("compressLevel", 9);
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
		CommonLib::CExcBase::RegenExc("Failed create BPTreeSet", exe);
		throw;
	}
}



template <class TBPTree, class TStorage>
void InsertBPTreeSet(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, int64_t nBegin, int64_t nEnd)
{
	try
	{
		int64_t nCount = (nEnd - nBegin);
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "InsertBPTreeSet start: %1 end: %2", nBegin, nEnd);

		TBPSet tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize);
		tree.SetBPTreePerfCounter(pPerf);
		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			if (i == 261120)
			{
				int dd = 0;
				dd++;
			}

			tree.insert(i);

			if (i == 261120)
			{
				auto it = tree.find(i);
			}

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
void FindBPTreeSet(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, int64_t nBegin, int64_t nEnd)
{
	try
	{
		int64_t nCount = (nEnd - nBegin);
		int64_t nStep = nCount / 100;
		if (nStep == 0)
			nStep = 1;
		
		CommonLib::CLogInfo info(Log, "FindBPTreeSet begin: %1, end %2", nBegin, nEnd);
		TBPSet tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize);
		tree.SetBPTreePerfCounter(pPerf);
		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			auto it = tree.find(i);

			if (it.IsNull())
				throw CommonLib::CExcBase("Error find %1", i);

			if (it.Key() != i)
				throw CommonLib::CExcBase("Error find key %1", it.Key(), i);

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
		CommonLib::CExcBase::RegenExc("Failed FindBPTreeSet", exe);
	}
}




template <class TBPTree, class TStorage>
void DeleteFromBPTreeSet(CommonLib::IAllocPtr pAlloc, CommonLib::TPrefCounterPtr pPerf, TStorage pStorage, int64_t nBPTreePage, int64_t nBegin, int64_t nEnd)
{
	try
	{
		int64_t nCount = (nEnd - nBegin);
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		CommonLib::CLogInfo info(Log, "DeleteFromBPTreeSet start: %1 end: %2", nBegin, nEnd);

		TBPSet tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize);
		tree.SetBPTreePerfCounter(pPerf);
		for (int64_t i = nBegin; i < nEnd; ++i)
		{			

			if (i == 91379)
			{
				int dd = 0;
				dd++;
			}


			tree.remove(i);

			
			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}
		}

		tree.Flush();

		info.Complete("DeleteFromBPTreeSet finish");

	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed to insert BPTreeSet", exe);
	}
}


void TestBPTreeSet()
{


	try
	{
#ifdef _WIN32
		wstr storagePath = L"F:\\BPSetTest.btdb";
#else
		bool bCycle = false;
		while (bCycle)
		{
			sleep(1);
		}


		wstr storagePath = L"/home/slava/BPSetTest";
#endif

		int64_t nBegin = 0;
		int64_t nEnd = 100000;
		int64_t nBPTreePage = -1;

		CommonLib::IAllocPtr pAlloc(new CommonLib::CSimpleAlloc(true));
		CommonLib::TPrefCounterPtr pStoragePerformer(new CommonLib::CPerfCounter(10));
		CommonLib::TPrefCounterPtr pBPTreePerf(new CommonLib::CPerfCounter(10));
		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), true, nPageSize);
			pStorage->SetStoragePerformer(pStoragePerformer);
			nBPTreePage = CreateBPTreeSet<TBPSet, bptreedb::TStoragePtr>(pAlloc, pStorage);
			//LogPerf(pStoragePerformer);
		}

		{

			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);
			InsertBPTreeSet<TBPSet, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nBegin, nEnd);

			pStorage->Close();

			CommonLib::file::TFilePtr file = CommonLib::file::CFileCreator::OpenFile(storagePath.c_str(), CommonLib::file::ofmOpenExisting, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
			//std::cout << "Storage size: " << file->GetFileSize() << "\n";
			Log.InfoT("Storage size: %1", file->GetFileSize());
			LogBPtreeInsertPerf(pBPTreePerf);
			LogBPtreePerf(pBPTreePerf);
			LogPerfStorage(pStoragePerformer);
		}

		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(storagePath.c_str(), false, nPageSize);
			pStoragePerformer->Reset();
			pBPTreePerf->Reset();
			pStorage->SetStoragePerformer(pStoragePerformer);

			FindBPTreeSet<TBPSet, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nBegin, nEnd);
			DeleteFromBPTreeSet<TBPSet, bptreedb::TStoragePtr>(pAlloc, pBPTreePerf, pStorage, nBPTreePage, nBegin, nEnd);

			LogBPtreeFindPerf(pBPTreePerf);			
			LogBPtreePerf(pBPTreePerf);
			LogBPtreeRemovePerf(pBPTreePerf);
			LogPerfStorage(pStoragePerformer);
		}
	}
	catch (std::exception& exe )
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