#include "Pager.h"
#include "../../Transaction/TransactionPage.h"

CPager::CPager(bptreedb::storage::IPageIOPtr ptrPageIO, CommonLib::IAllocPtr ptrAlloc, uint32_t pageSize) :
	m_ptrPageIO(ptrPageIO), m_ptrAlloc(ptrAlloc),m_pageSize(pageSize)
{

}

CPager::~CPager()
{

}

int64_t CPager::GetNewFilePageAddr()
{
	return m_ptrPageIO->GetNewFilePageAddr(m_pageSize);
}

bptreedb::storage::IFilePagePtr CPager::GetNewPage(int64_t nAddr)
{
	bptreedb::transaction::CTransactionPagePtr ptrPage = 
		std::make_shared<bptreedb::transaction::CTransactionPage>(m_ptrAlloc, m_pageSize, nAddr, 
			m_nTransactionId, m_nUserId,
			m_objectID, m_objectType, m_parentID, m_parentType);

	return ptrPage;
}

bptreedb::storage::IFilePagePtr CPager::ReadPage(int64_t nAddr)
{
	bptreedb::transaction::CTransactionPagePtr ptrPage =
		std::make_shared<bptreedb::transaction::CTransactionPage>(m_ptrAlloc, m_pageSize, nAddr, 
			m_nTransactionId, m_nUserId,
			m_objectID, m_objectType, m_parentID, m_parentType);

	ptrPage->Read(m_ptrPageIO, nAddr);

	return ptrPage;
}

void CPager::ReadPage(bptreedb::storage::IFilePagePtr ptrPage, int64_t nAddr)
{
	ptrPage->Read(m_ptrPageIO, nAddr);
}

void CPager::SavePage(bptreedb::storage::IFilePagePtr ptrPage)
{
	ptrPage->Save(m_ptrPageIO);
}