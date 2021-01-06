#include "stdafx.h"
#include "AllocsSet.h"

namespace bptreedb
{

	CAllocsSet::CAllocsSet(CommonLib::IAllocPtr pAlloc) : m_pCommonAlloc(pAlloc)
	{

	}

	CAllocsSet::~CAllocsSet()
	{

	}

	void CAllocsSet::AddAlloc(CommonLib::IAllocPtr pAlloc, uint32_t id)
	{
		m_allocs.insert(std::make_pair(id, pAlloc));
	}

	CommonLib::IAllocPtr CAllocsSet::GetAlloc(uint32_t id, bool retCommonIfNull)
	{
		auto it = m_allocs.find(id);
		if (it != m_allocs.end())
			return it->second;

		if (!retCommonIfNull)
			return CommonLib::IAllocPtr();

		return GetCommonAlloc();
	}

	CommonLib::IAllocPtr CAllocsSet::GetCommonAlloc()
	{
		return m_pCommonAlloc;
	}
}