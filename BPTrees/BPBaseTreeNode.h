#pragma once

#include "../../CommonLib/stream/MemoryStream.h"
#include "../../CommonLib/alloc/stl_alloc.h"
#include "BPTreeContext.h"


namespace bptreedb
{
	enum eBPTreeNodePageFlags
	{
		ROOT_NODE = 1,
		CHANGE_NODE = 2,
		BUSY_NODE = 4,
		REMOVE_NODE = 8,
		CHECK_REM_NODE = 16,
		LOCK_FROM_REMOVE = 32
	};


	#define  LESS_INDEX  -1
	#define  EMPTY_PAGE_ADDR  -1
 

	class IBPTreeNode;
	typedef std::shared_ptr<IBPTreeNode> IBPTreeNodePtr;
	typedef std::weak_ptr<IBPTreeNode> TParentNodePtr;


	

	class IBPTreeNode
	{
	public:
		IBPTreeNode();
		virtual ~IBPTreeNode();


		virtual void Load(CommonLib::IMemoryReadStream* pStream, CBPTreeContext *pContext) = 0;
		virtual uint32_t Save(CommonLib::IMemoryWriteStream* pStream, CBPTreeContext *pContext) = 0;

		virtual uint32_t Size() const = 0;
		virtual uint32_t HeadSize() const = 0;
		virtual uint32_t RowSize() const = 0;
		virtual	uint32_t TupleSize() const = 0;
		virtual bool IsNeedSplit() const = 0;
		virtual bool IsHalfEmpty() const = 0;


		virtual uint32_t GetFlags() const;
		virtual uint32_t Count() const = 0;
		//virtual uint64_t GetAddr() const;

		virtual void SetFlags(uint32_t nFlag, bool bSet);
		virtual void Clear() = 0;



	protected:
		int64_t m_nAddr;
		uint32_t m_nFlag;
		bool m_bMinSplit;
	};

}