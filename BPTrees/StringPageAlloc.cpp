#include "stdafx.h"
#include "StringPageAlloc.h"

namespace bptreedb
{
	CStringPageAlloc::CStringPageAlloc(uint32_t nMaxLen, CommonLib::IAllocPtr pAlloc) : m_maxLen(nMaxLen), m_pAlloc(pAlloc)
	{

	}

	CStringPageAlloc::~CStringPageAlloc()
	{

	}

	void* CStringPageAlloc::Alloc(size_t size)
	{
		return m_pAlloc->Alloc(size);
	}

	void CStringPageAlloc::Free(void *ptr)
	{
		m_pAlloc->Free(ptr);
	}

	int64_t CStringPageAlloc::GetCurrentMemoryBalance() const
	{
		return m_pAlloc->GetCurrentMemoryBalance();
	}

	uint64_t CStringPageAlloc::GetAllocCount() const
	{
		return m_pAlloc->GetAllocCount();
	}

	uint64_t CStringPageAlloc::GetFreeCount() const
	{
		return m_pAlloc->GetFreeCount();
	}

	uint64_t CStringPageAlloc::GetTotalAlloc() const
	{
		return m_pAlloc->GetTotalAlloc();
	}

	uint64_t CStringPageAlloc::GetTotalFree() const
	{
		return m_pAlloc->GetTotalFree();
	}

	uint64_t CStringPageAlloc::GetSizeMax() const
	{
		return m_pAlloc->GetSizeMax();
	}



}