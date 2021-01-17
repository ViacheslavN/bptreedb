#include "pch.h"
#include "PerfLog.h"



void CPerfLog::LogPerfStorage(CommonLib::TPrefCounterPtr pStoragePerformer)
{

	Log.Info("------STORAGE PERF------");
	Log.InfoT("read io count %1, read data %2, total time %3", pStoragePerformer->GetCountOperation(bptreedb::IStorage::eReadData), pStoragePerformer->GetDataOperation(bptreedb::IStorage::eReadData), pStoragePerformer->GetTimeOperation(bptreedb::IStorage::eReadData));
	Log.InfoT("write io count %1, write data %2, total time %3", pStoragePerformer->GetCountOperation(bptreedb::IStorage::eWriteData), pStoragePerformer->GetDataOperation(bptreedb::IStorage::eWriteData), pStoragePerformer->GetTimeOperation(bptreedb::IStorage::eWriteData));

}

void CPerfLog::LogBPtreeInsertPerf(CommonLib::TPrefCounterPtr pPerf)
{
	Log.Info("------BPTREE INSERT------");
	Log.InfoT("insert total time: %1,", pPerf->GetTimeOperation(bptreedb::eInsertValue));
	Log.InfoT("Flush total time %1", pPerf->GetTimeOperation(bptreedb::eFlush));
}

void CPerfLog::LogBPtreePerf(CommonLib::TPrefCounterPtr  pPerf)
{
	Log.Info("------BPTREE NODE------");
	Log.InfoT("save node counts: %1, missed %2, total time %3", pPerf->GetCountOperation(bptreedb::eSaveNode), pPerf->GetCountOperation(bptreedb::eMissedNode), pPerf->GetTimeOperation(bptreedb::eSaveNode));
	Log.InfoT("load node %1, total time %2", pPerf->GetCountOperation(bptreedb::eLoadNode), pPerf->GetTimeOperation(bptreedb::eLoadNode));
	Log.InfoT("get node %1, total time %2", pPerf->GetCountOperation(bptreedb::eGetNode), pPerf->GetTimeOperation(bptreedb::eGetNode));
	Log.InfoT("get parent node %1, total time %2", pPerf->GetCountOperation(bptreedb::eGetParentNode), pPerf->GetTimeOperation(bptreedb::eGetParentNode));
	Log.InfoT("add to cache  %1, total time %2", pPerf->GetCountOperation(bptreedb::eAddToCache), pPerf->GetTimeOperation(bptreedb::eAddToCache)); 
		Log.InfoT("get node from cache  %1, total time %2", pPerf->GetCountOperation(bptreedb::eGetNodeFromCache), pPerf->GetTimeOperation(bptreedb::eGetNodeFromCache));
}

void CPerfLog::LogBPtreeRemovePerf(CommonLib::TPrefCounterPtr pPerf)
{
	Log.Info("------BPTREE DELETE------");
	Log.InfoT("remove total time: %1,", pPerf->GetTimeOperation(bptreedb::eRemoveValue));
	Log.InfoT("Flush total time %1", pPerf->GetTimeOperation(bptreedb::eFlush));
}

void CPerfLog::LogBPtreeFindPerf(CommonLib::TPrefCounterPtr pPerf)
{
	Log.Info("------BPTREE FIND------");
	Log.InfoT("find key total time: %1,", pPerf->GetTimeOperation(bptreedb::eFindKey));
}