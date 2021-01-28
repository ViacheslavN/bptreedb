#include "stdafx.h"
#include "BPBaseTreeNode.h"

namespace bptreedb
{
	IBPTreeNode::IBPTreeNode() : m_nFlag(0), m_nAddr(-1)
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