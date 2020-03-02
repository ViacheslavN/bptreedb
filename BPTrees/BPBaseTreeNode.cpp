#include "stdafx.h"
#include "BPBaseTreeNode.h"

namespace bptreedb
{

	IBPTreeNode::IBPTreeNode() : m_nFlag(0), m_nParent(-1), m_bMinSplit(false), m_nFoundIndex(-1), m_nAddr(-1)
	{}

	IBPTreeNode::~IBPTreeNode() {}

	uint32_t IBPTreeNode::GetFlags() const
	{
		return m_nFlag;
	}

	void IBPTreeNode::SetFlags(int nFlag, bool bSet)
	{
		if (bSet)
			m_nFlag |= nFlag;
		else
			m_nFlag &= ~nFlag;
	}

	void IBPTreeNode::SetParent(IBPTreeNodePtr pNode, int32_t nFoundIndex)
	{
		m_pParent = pNode;
		m_nFoundIndex = nFoundIndex;

		if (pNode.get())
			m_nParent = pNode->GetAddr();
		else
			m_nParent = -1;
	}

	int64_t IBPTreeNode::GetParentAddr() const
	{ 
		return m_nParent;
	}

	int64_t IBPTreeNode::GetFoundIndex() const
	{
		return m_nFoundIndex; 
	}

	void IBPTreeNode::SetFoundIndex(int32_t nFoundIndex)
	{ 
		m_nFoundIndex = nFoundIndex;
	}

	uint64_t IBPTreeNode::GetAddr() const
	{
		return m_nAddr;
	}

	TParentNodePtr IBPTreeNode::GetParentNodePtr()
	{
		return m_pParent.lock();
	}

}