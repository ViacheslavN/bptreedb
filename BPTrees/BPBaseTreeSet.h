#pragma once
#include "BPBaseTreeNode.h"
#include "../utils/CacheLRU_2Q.h"

namespace bptreedb
{
	template <	class _TKey, class _TComp, class _Transaction,
		class _TInnerNode,
		class _TLeafNode
	>
	class TBPlusTreeSetBase
	{
	public:
		typedef _TKey      TKey;
		typedef _TComp	   TComp;
		typedef int64_t     TLink;
		typedef _Transaction  Transaction;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode	TLeafNode;
		typedef typename TInnerNode::TCompressor  TInnerCompess;
		typedef typename TLeafNode::TCompressor   TLeafCompess;


		TBPlusTreeSetBase(int64 nPageBTreeInfo, std::shared_ptr<_Transaction> pTransaction, CommonLib::IAllocPtr pAlloc, uint32 nChacheSize, uint32 nNodesPageSize, bool bMulti = false) :
			m_nPageBTreeInfo(nPageBTreeInfo), m_pTransaction(pTransaction), m_pAlloc(pAlloc), m_nChacheSize(nChacheSize)
			, m_nRootAddr(-1), m_bMulti(bMulti)
			, m_Cache(pAlloc)
			, m_nPageInnerCompInfo(-1)
			, m_nPageLeafPageCompInfo(-1)
			, m_bMinSplit(false)
			, m_nNodesPageSize(nNodesPageSize)
		{

		}

		~TBPlusTreeSetBase()
		{

		}

	protected:

		IBPTreeNodePtr GetNode(int64_t nAddr);
		IBPTreeNodePtr LoadNodeFromStorage(int64_t nAddr);


	protected:
		
		TComp		 m_comp;
		IBPTreeNodePtr m_pRoot;
		TLink m_nRootAddr;
		TLink m_nPageBTreeInfo;
		uint32_t m_nNodesPageSize;
		CommonLib::IAllocPtr m_pAlloc;
		std::shared_ptr<Transaction> m_pTransaction;
		uint32_t m_nChacheSize;
		uint64_t m_nPageInnerCompInfo;
		uint64_t m_nPageLeafPageCompInfo;

		struct TBPTreeFreeChecker
		{
			bool IsFree(IBPTreeNodePtr& pObj)
			{
				return  pObj.use_count() == 1;
			}
		};

		typedef TCacheLRU_2Q<TLink, IBPTreeNodePtr, TBPTreeFreeChecker> TNodesCache;
		TNodesCache m_NodeCache;
	};

#define  BPSETBASE_TEMPLATE_PARAMS 	template <	class _TKey, class _TComp, class _Transaction,	class _TInnerNode,	class _TLeafNode>
#define  BPSETBASE_DECLARATION  TBPlusTreeSetBase<_TKey, _TComp, _Transaction, _TInnerNode, _TLeafNode>



#include "BPBaseTreeSetCommon.h"

}