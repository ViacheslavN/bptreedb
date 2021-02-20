#pragma once
#include "BPBaseTreeNode.h"
#include "../utils/CacheLRU_2Q.h"
#include "../utils/ReadStreamPage.h"
#include "../utils/WriteStreamPage.h"
#include "BPTreeNodeHolder.h"
#include "Compressor/CompressorParams.h"
#include "BPTreePerfCounter.h"
#include "AllocsSet.h"


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


 

		TBPlusTreeSetBase(int64_t nPageBTreeInfo, std::shared_ptr<TStorage> pStorage, CommonLib::IAllocPtr pAlloc, uint32_t nCacheize, uint32_t nNodesPageSize, bool bMulti = false, bool bMinSplit = false) :
			m_nPageBTreeInfo(nPageBTreeInfo), m_pStorage(pStorage),  m_nCacheSize(nCacheize)
			, m_nRootAddr(-1)
			, m_bMulti(bMulti)
			, m_NodeCache(pAlloc)
			, m_bMinSplit(bMinSplit)
			, m_nNodePageSize(nNodesPageSize)
			, m_bLockRemoveItemFromCache(false)
			, m_Context(pAlloc)
		{

			m_pAllocsSet.reset(new CAllocsSet(pAlloc));
		}

		~TBPlusTreeSetBase()
		{

		}

		/*common*/
		void InnitTree(TCompressorParamsBasePtr pParams, bool bMinSplit, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype);
		bool IsTreeInit();
		void Flush();
		void SetBPTreePerfCounter(CommonLib::TPrefCounterPtr pBPTreePerfCounter);

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



		/*delete*/
		bool remove(const TKey& key);

		template<class TIterator, class TComparator>
		TIterator remove(const TComparator& comp, const TKey& key);
		//common
		TBPTreeNodePtr Root() 
		{ 
			if (!m_pRoot.get())
				LoadTree();

			return m_pRoot; 
		}
		TBPTreeNodePtr GetNode(int64_t nAddr);
		void CheckCache();
	protected:

		void LoadTree();



	
		//common
		
		TBPTreeNodePtr LoadNodeFromStorage(int64_t nAddr);
		TBPTreeNodePtr CreateNode(int64_t nAddr, bool isLeaf, bool addToChache);
		uint32_t GetNodePageSize() const;


		TBPTreeNodePtr NewNode(bool isLeaf, bool addToChache);
		void AddToCache(TBPTreeNodePtr node);


		TBPTreeNodePtr GetNodeFromCache(int64_t nAddr, bool bNotMove = false);
	

		TBPTreeNodePtr GetParentNode(TBPTreeNodePtr pNode, int32_t* nElementPos = nullptr);
		TBPTreeNodePtr GetParentNodeByKey(TBPTreeNodePtr pNode, const TKey& key, int32_t* nElementPos = nullptr);
		void DropNode(TBPTreeNodePtr& pNode);
		void SaveNode(TBPTreeNodePtr& pNode);
	
		TBPTreeNodePtr GetMinimumNode(TBPTreeNodePtr pNode);

		template <class TNodeHolder>
		std::shared_ptr<TNodeHolder> LoadNode(CommonLib::IReadStream *pStream, TAllocsSetPtr pAllocsSet, bool bMulti, uint32_t nPageSize, int64_t nAddr, TCompressorParamsBasePtr pCompressParams, CBPTreeContext *pContext)
		{
			try
			{
				bool isLeaf = pStream->ReadBool();

				std::shared_ptr<TNodeHolder> pNode(new TNodeHolder(pAllocsSet, bMulti, nPageSize, isLeaf, nAddr, pCompressParams));
				pNode->Load(pStream, pContext);

				return pNode;

			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("TBPNodeHolder failed to load ", exc);
				throw;
			}

		}

 
		virtual TBPTreeNodePtr AllocateNewNode(int64_t nAddr, bool bLeaf) = 0;
		
		//insert
		TBPTreeNodePtr findLeafNodeForInsert(const TKey& key);
		void CheckLeafNode(TBPTreeNodePtr &pNode);
		void TransformRootToInner();
		void SplitLeafNode(TBPTreeNodePtr &pNode, TBPTreeNodePtr &pNewNode, TBPTreeNodePtr &pParentNode, int32_t nCount);
		void SplitRootInnerNode();
		void SplitInnerNode(TBPTreeNodePtr&pNode, TBPTreeNodePtr& pNodeNewRight, TBPTreeNodePtr& pParentNode, int32_t nCount);
 

		//remove
 
		TBPTreeNodePtr FindLeafNodeForDelete(const TKey& key);
		void RemoveFromLeafNode(TBPTreeNodePtr pNode, int32_t nIndex, const TKey& key);
		void RemoveFromInnerNode(TBPTreeNodePtr pNode, const TKey& key);
		void UnionLeafNode(TBPTreeNodePtr pParentNode, TBPTreeNodePtr pLeafNode, TBPTreeNodePtr pDonorNode, int32_t nDonorFoundIndex);
		void DeleteNode(TBPTreeNodePtr pNode);
		void AlignmentLeafNode(TBPTreeNodePtr pParentNode, TBPTreeNodePtr pLeafNode, int32_t nFoundIndex, TBPTreeNodePtr pDonorNode, int32_t nDonorIndex, bool bLeft);
		void UnionInnerNode(TBPTreeNodePtr pParentNode, TBPTreeNodePtr pNode, TBPTreeNodePtr pDonorNode, int32_t nDonorFoundIndex);
		void AlignmentInnerNode(TBPTreeNodePtr pParentNode, TBPTreeNodePtr pNode, int32_t nFoundIndex,  TBPTreeNodePtr pDonorNode, int32_t nDonorFoundIndex, bool bLeft);

	protected:



		TComp	m_comp;
		TBPTreeNodePtr m_pRoot;
		TLink m_nRootAddr{ -1 };
		TLink m_nPageBTreeInfo{ -1 };
		uint32_t m_nNodePageSize{ 0 };
	//	CommonLib::IAllocPtr m_pAlloc;
		TAllocsSetPtr m_pAllocsSet;
		std::shared_ptr<TStorage> m_pStorage;
		uint32_t m_nCacheSize{ 0 };
		bool m_bMulti{ false };
		bool m_bMinSplit{ false };
		bool m_bLockRemoveItemFromCache{ false };
		CommonLib::TPrefCounterPtr m_pBPTreePerfCounter;
		uint32_t m_objectType{ 0 };
		uint32_t m_objectID{ 0 };
		uint32_t m_parentID{ 0 };
		uint32_t m_parentType{ 0 };


		TCompressorParamsBasePtr m_pCompressParams;
		FilePagePtr m_cachePage;

		struct TBPTreeFreeChecker
		{
			bool IsFree(TBPTreeNodePtr& pObj)
			{
				return  !(pObj->GetFlags() & LOCK_FROM_REMOVE);
			}
		};

		typedef utils::TCacheLRU_2Q<TLink, TBPTreeNodePtr, TBPTreeFreeChecker> TNodesCache;
		TNodesCache m_NodeCache;
		CBPTreeContext m_Context;
	};

#define  BPSETBASE_TEMPLATE_PARAMS 	template <	class _TKey, class _TComp, class _TStorage,	class _TInnerNode,	class _TLeafNode, class _TNodeHolder>
#define  BPSETBASE_DECLARATION  TBPlusTreeSetBase<_TKey, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder>
#define  BPSETBASE_TYPENAME_DECLARATION  typename TBPlusTreeSetBase<_TKey, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder>


#include "BPBaseTreeSetCommon.h"
#include "BPBaseTreeSetInsert.h"
#include "BPBaseTreeSearch.h"
#include "BPBaseTreeDelete.h"

}