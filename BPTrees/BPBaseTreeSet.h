#pragma once
#include "BPBaseTreeNode.h"
#include "../utils/CacheLRU_2Q.h"
#include "../utils/ReadStreamPage.h"
#include "../utils/WriteStreamPage.h"
#include "BPTreeNodeHolder.h"

namespace bptreedb
{
	template <	class _TKey, class _TComp, class _TStorage,
		class _TInnerNode,
		class _TLeafNode,
		class _TNodeHolder = TBPNodeHolder<_TInnerNode, _TLeafNode>
	>
	class TBPlusTreeSetBase
	{
	public:
		typedef _TKey      TKey;
		typedef _TComp	   TComp;
		typedef int64_t     TLink;
		typedef _TStorage  TStorage;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode	TLeafNode;
		typedef typename TInnerNode::TCompressor  TInnerCompess;
		typedef typename TLeafNode::TCompressor   TLeafCompess;
		typedef _TNodeHolder TBPTreeNode;
		typedef std::shared_ptr<TBPTreeNode> TBPTreeNodePtr;

		typedef typename TInnerNode::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TLeafNode::TLeafCompressorParams TLeafCompressorParams;

		typedef std::shared_ptr< TInnerCompressorParams> TInnerCompressorParamsPtr;
		typedef std::shared_ptr< TLeafCompressorParams> TLeafCompressorParamsPtr;


		TBPlusTreeSetBase(int64_t nPageBTreeInfo, std::shared_ptr<TStorage> pStorage, CommonLib::IAllocPtr pAlloc, uint32_t nChacheSize, uint32_t nNodesPageSize, bool bMulti = false) :
			m_nPageBTreeInfo(nPageBTreeInfo), m_pStorage(pStorage), m_pAlloc(pAlloc), m_nChacheSize(nChacheSize)
			, m_nRootAddr(-1)
			, m_bMulti(bMulti)
			, m_NodeCache(pAlloc)
			, m_nPageInnerCompInfo(-1)
			, m_nPageLeafPageCompInfo(-1)
			, m_bMinSplit(false)
			, m_nNodePageSize(nNodesPageSize)
			, m_bLockRemoveItemFromCache(false)
		{

		}

		~TBPlusTreeSetBase()
		{

		}

		/*common*/
		void InnitTree(TInnerCompressorParamsPtr innerParams, TLeafCompressorParamsPtr leafParams);
		bool IsTreeInit();
		void Flush();
		TLink GetPageBTreeInfo() const
		{
			return m_nPageBTreeInfo;
		}

		/*insert*/
		void insert(TKey& key);

		/*search*/
		template<class TIterator, class TComparator>
		TIterator find(const TComparator& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true);

		template<class TIterator>
		TIterator begin();

		template<class TIterator>
		TIterator last();

		template<class TIterator, class TComparator>
		TIterator upper_bound(const TComparator& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true);

		template<class TIterator, class TComparator>
		TIterator lower_bound(const TComparator& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true);


	protected:

		void LoadTree();

		//common
		TBPTreeNodePtr GetNode(int64_t nAddr);
		TBPTreeNodePtr GetNodeAndCheckParent(int64_t nAddr);
		TBPTreeNodePtr LoadNodeFromStorage(int64_t nAddr);
		TBPTreeNodePtr CreateNode(int64_t nAddr, bool isLeaf, bool addToChache);
		TBPTreeNodePtr NewNode(bool isLeaf, bool addToChache);
		void AddToCache(TBPTreeNodePtr& node);
	
		void SetParentForNextNode(TBPTreeNodePtr& pNode, TBPTreeNodePtr& pNextNode);
		void SetParentForPrevNode(TBPTreeNodePtr& pNode, TBPTreeNodePtr& pPrevNode);
		TBPTreeNodePtr GetParentNode(TBPTreeNodePtr& pNode);
		TBPTreeNodePtr FindAndSetParent(TBPTreeNodePtr& pNode);
		void DropNode(TBPTreeNodePtr& pNode);
		void SaveNode(TBPTreeNodePtr& pNode);
	

		//insert
		TBPTreeNodePtr findLeafNodeForInsert(const TKey& key);
		void CheckLeafNode(TBPTreeNodePtr &pNode);
		void TransformRootToInner();
		void SplitLeafNode(TBPTreeNodePtr &pNode, TBPTreeNodePtr &pNewNode, TBPTreeNodePtr &pParentNode);
		void SplitRootInnerNode();
		void SetParentInChildCacheOnly(TBPTreeNodePtr& pNode);
		void SplitInnerNode(TBPTreeNodePtr&pNode, TBPTreeNodePtr& pParentNode);

	protected:
		
		TComp		 m_comp;
		TBPTreeNodePtr m_pRoot;
		TLink m_nRootAddr;
		TLink m_nPageBTreeInfo;
		uint32_t m_nNodePageSize;
		CommonLib::IAllocPtr m_pAlloc;
		std::shared_ptr<TStorage> m_pStorage;
		uint32_t m_nChacheSize;
		uint64_t m_nPageInnerCompInfo;
		uint64_t m_nPageLeafPageCompInfo;
		bool m_bMulti;
		bool m_bMinSplit;
		bool m_bLockRemoveItemFromCache;


		TInnerCompressorParamsPtr m_InnerCompressParams;
		TLeafCompressorParamsPtr  m_LeafCompressParams;

		struct TBPTreeFreeChecker
		{
			bool IsFree(TBPTreeNodePtr& pObj)
			{
				return  true;//pObj.use_count() == 1;
			}
		};

		typedef utils::TCacheLRU_2Q<TLink, TBPTreeNodePtr, TBPTreeFreeChecker> TNodesCache;
		TNodesCache m_NodeCache;
	};

#define  BPSETBASE_TEMPLATE_PARAMS 	template <	class _TKey, class _TComp, class _TStorage,	class _TInnerNode,	class _TLeafNode, class _TNodeHolder>
#define  BPSETBASE_DECLARATION  TBPlusTreeSetBase<_TKey, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder>
#define  BPSETBASE_TYPENAME_DECLARATION  typename TBPlusTreeSetBase<_TKey, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder>


#include "BPBaseTreeSetCommon.h"
#include "BPBaseTreeSetInsert.h"
#include "BPBaseTreeSearch.h"

}