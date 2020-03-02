#pragma once

#include "../../CommonLib/stream/MemoryStream.h"

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

	class IBPTreeNode;
	typedef std::shared_ptr<IBPTreeNode> IBPTreeNodePtr;
	typedef std::weak_ptr<IBPTreeNode> TParentNodePtr;

	class IBPTreeNode
	{
	public:
		IBPTreeNode();
		virtual ~IBPTreeNode();

		virtual bool IsLeaf() const = 0;
		virtual void Load(CommonLib::IReadStream* pStream) = 0;
		virtual uint32_t Save(CommonLib::IWriteStream* pStream) = 0;

		virtual uint32_t Size() const = 0;
		virtual uint32_t HeadSize() const = 0;
		virtual uint32_t RowSize() const = 0;
		virtual	uint32_t TupleSize() const = 0;
		virtual bool IsNeedSplit() const = 0;

		virtual uint32_t GetFlags() const;
		virtual uint64_t GetAddr() const;

		virtual void SetFlags(int nFlag, bool bSet);
		virtual void Clear() = 0;

		void SetParent(IBPTreeNodePtr pNode, int32_t nFoundIndex = -1);

		int64_t GetParentAddr() const;
		int64_t GetFoundIndex() const;
		void SetFoundIndex(int32_t nFoundIndex);
		TParentNodePtr GetParentNodePtr();

	protected:
		int64_t m_nAddr;
		uint32_t m_nFlag;	
		TParentNodePtr m_pParent;
		int32_t m_nFoundIndex;
		int64_t m_nParent;
		bool m_bMinSplit;
	};
}