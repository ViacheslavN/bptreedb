#include "stdafx.h"
#include "BPTreeContext.h"

namespace bptreedb
{
	CBPTreeContext::CBPTreeContext(CommonLib::IAllocPtr pAlloc) : m_commonBuf(TAlloc(pAlloc))
	{

	}

	CBPTreeContext::~CBPTreeContext()
	{

	}

	size_t CBPTreeContext::GetCommonBufSize() const
	{
		return m_commonBuf.size();
	}

	byte_t* CBPTreeContext::GetCommonBuf()
	{
		return &m_commonBuf[0];
	}

	void CBPTreeContext::ResizeCommonBuf(size_t size)
	{
		if (size > m_commonBuf.size())
			m_commonBuf.resize(size);
	}

}

