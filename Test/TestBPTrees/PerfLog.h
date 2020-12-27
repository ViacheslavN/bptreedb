#pragma once

class CPerfLog
{
public:
	static void LogPerfStorage(CommonLib::TPrefCounterPtr pStoragePerformer);
	static void LogBPtreeInsertPerf(CommonLib::TPrefCounterPtr pPerf);
	static void LogBPtreePerf(CommonLib::TPrefCounterPtr  pPerf);
	static void LogBPtreeRemovePerf(CommonLib::TPrefCounterPtr pPerf);
	static void LogBPtreeFindPerf(CommonLib::TPrefCounterPtr pPerf);
};