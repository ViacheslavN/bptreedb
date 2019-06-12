#pragma once

#include "../../CommonLib/stream/MemoryStream.h"
#include "../../CommonLib/stream/FixedMemoryStream.h"

namespace bptreedb
{
	enum eBPTreeNodePageFlags
	{
		ROOT_NODE = 1,
		CHANGE_NODE = 2,
		BUSY_NODE = 4,
		REMOVE_NODE = 8,
		CHECK_REM_NODE = 16
	};

	class BPBaseTreeNode;
	typedef std::shared_ptr<BPBaseTreeNode> TBPBaseTreeNodePtr;

	class BPBaseTreeNode
	{
	public:
		BPBaseTreeNode() : m_nFlag(0) {}
		virtual ~BPBaseTreeNode(){}

		virtual bool IsLeaf() const = 0;
		virtual  void Load(CommonLib::CReadMemoryStream& stream) = 0;
		virtual  uint32_t Save(CommonLib::CFxMemoryWriteStream& stream) = 0;

		virtual uint32_t Size() const = 0;
		virtual uint32_t HeadSize() const = 0;
		virtual uint32_t RowSize() const = 0;
		virtual	uint32_t TupleSize() const = 0;
		virtual bool IsNeedSplit() const = 0;
		virtual uint32_t GetFlags() const
		{
			return m_nFlag;
		}
		virtual void SetFlags(int nFlag, bool bSet)
		{
			if (bSet)
				m_nFlag |= nFlag;
			else
				m_nFlag &= ~nFlag;
		}
		virtual void clear() = 0;
	public:
		uint32_t m_nFlag;
	};
}