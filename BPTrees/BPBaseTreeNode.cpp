#include "stdafx.h"
#include "BPBaseTreeNode.h"

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
		if(size > m_commonBuf.size())
			m_commonBuf.resize(size);
	}



	IBPTreeNode::IBPTreeNode() : m_nFlag(0), m_nParent(-1), m_bMinSplit(false), m_nFoundIndex(-1), m_nAddr(-1)
	{}

	IBPTreeNode::~IBPTreeNode() {}

	uint32_t IBPTreeNode::GetFlags() const
	{
		return m_nFlag;
	}

	void IBPTreeNode::SetFlags(uint32_t nFlag, bool bSet)
	{
		if (bSet)
			m_nFlag |= nFlag;
		else
			m_nFlag &= ~nFlag;
	}

	

}