#pragma once
#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/CommonLib_lib.h"
#include "../../../CommonLib/exception/exc_base.h"
#include "../../../CommonLib/alloc/alloc.h"
#include "../../../CommonLib/perf/PerfCount.h"
#include "../../storage/FilePage.h"
#include "../../storage/Storage.h"


typedef std::shared_ptr<class CPager> CPagerPtr;

class CPager : public bptreedb::storage::IFilePager
{
public:
	CPager(bptreedb::storage::IPageIOPtr ptrPageIO, CommonLib::IAllocPtr ptrAlloc, uint32_t pageSize);
	virtual ~CPager();

	virtual int64_t GetNewFilePageAddr();
	virtual bptreedb::storage::IFilePagePtr GetNewPage(int64_t nAddr);
	virtual bptreedb::storage::IFilePagePtr ReadPage(int64_t nAddr);
	virtual void ReadPage(bptreedb::storage::IFilePagePtr ptrPage, int64_t nAddr);
	virtual void SavePage(bptreedb::storage::IFilePagePtr ptrPage);

private:
	bptreedb::storage::IPageIOPtr m_ptrPageIO;
	CommonLib::IAllocPtr m_ptrAlloc;
	uint32_t m_pageSize;

	//Transaction header
	int64_t m_nTransactionId{ -1 };
	uint64_t m_nDate{ 0 };
	int32_t m_nUserId{ -1 };

	//Object header
	uint32_t m_objectType{ 0 };
	uint32_t m_objectID{ 0 };
	uint32_t m_parentID{ 0 };
	uint32_t m_parentType{ 0 };


};