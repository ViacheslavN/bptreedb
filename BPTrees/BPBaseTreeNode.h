#pragma once

#include "../../CommonLib/stream/MemoryStream.h"
#include "../../CommonLib/alloc/stl_alloc.h"
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


	class CBPTreeContext
	{
	public:
		CBPTreeContext(CommonLib::IAllocPtr pAlloc);
		~CBPTreeContext();

		size_t GetCommonBufSize() const;
		byte_t* GetCommonBuf();
		void ResizeCommonBuf(size_t size);

	private:
		CommonLib::IAllocPtr m_pAlloc;
		typedef CommonLib::STLAllocator<byte_t> TAlloc;
		typedef std::vector<byte_t, TAlloc> TVecBuffer;
		TVecBuffer m_commonBuf;
	};


	class IBPTreeNode
	{
	public:
		IBPTreeNode();
		virtual ~IBPTreeNode();


		virtual void Load(CommonLib::IReadStream* pStream, CBPTreeContext *pContext) = 0;
		virtual uint32_t Save(CommonLib::IWriteStream* pStream, CBPTreeContext *pContext) = 0;

		virtual uint32_t Size() const = 0;
		virtual uint32_t HeadSize() const = 0;
		virtual uint32_t RowSize() const = 0;
		virtual	uint32_t TupleSize() const = 0;
		virtual bool IsNeedSplit() const = 0;

		virtual uint32_t GetFlags() const;
		virtual uint32_t Count() const = 0;
		//virtual uint64_t GetAddr() const;

		virtual void SetFlags(uint32_t nFlag, bool bSet);
		virtual void Clear() = 0;



	protected:
		int64_t m_nAddr;
		uint32_t m_nFlag;
		TParentNodePtr m_pParent;
		int32_t m_nFoundIndex;
		int64_t m_nParent;
		bool m_bMinSplit;
	};

}