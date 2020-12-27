#pragma once

#include "BPBaseTreeSet.h"
#include "BPIteratorMap.h"
#include "BPTreeNodeMapHolder.h"

namespace bptreedb
{
	template <	class _TKey, class _TValue, class _TComp, class _TStorage,
		class _TInnerNode,
		class _TLeafNode,
		class _TNodeHolder = TBPMapNodeHolder<_TInnerNode, _TLeafNode>
	>
	
	class TBPMap : public TBPlusTreeSetBase<_TKey, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder>
	{
	public:

		typedef TBPlusTreeSetBase<_TKey, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder> TBase;
		typedef typename _TValue TValue;
		typedef typename TBase::TKey  TKey;
		typedef typename TBase::TComp	   TComp;
		typedef typename TBase::TLink  TLink;
		typedef typename TBase::TStorage  TStorage;
		typedef typename TBase::TInnerNode TInnerNode;
		typedef typename TBase::TLeafNode	TLeafNode;
		typedef typename TBase::TInnerCompess TInnerCompess;
		typedef typename TBase::TLeafCompess TLeafCompess;
		typedef typename TBase::TBPTreeNode TBPTreeNode;
		typedef typename TBase::TBPTreeNodePtr TBPTreeNodePtr;


		template <class _TK, class _TV, class _TNode, class _TBTree>
		friend class TBPMapIterator;

		typedef TBPMapIterator<TKey, TValue, TBPTreeNode, TBase> iterator;



		TBPMap(int64_t nPageBTreeInfo, std::shared_ptr<TStorage> pStorage, CommonLib::IAllocPtr pAlloc, uint32_t nChacheSize, uint32_t nNodesPageSize, bool bMulti = false) :
			TBase(nPageBTreeInfo, pStorage, pAlloc, nChacheSize, nNodesPageSize, bMulti)
		{

		}

		void insert(const TKey& key, const TValue& value)
		{
			try
			{
				CommonLib::CPrefCounterHolder holder(this->m_pBPTreePerfCounter, eInsertValue);

				TBPTreeNodePtr pNode = this->findLeafNodeForInsert(key);
				pNode->InsertInLeaf(this->m_comp, key, value);
				pNode->SetFlags(CHANGE_NODE, true);

				this->CheckLeafNode(pNode);
 
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed insert", exc);
			}
		}
		

		iterator find(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template find<iterator, TComp>(this->m_comp, key, pFromIterator, bFindNext);
		}

		template<class _TCustComp>
		iterator find(_TCustComp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template find<iterator, _TCustComp>(comp, key, pFromIterator, bFindNext);
		}

		iterator upper_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template upper_bound<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}
		iterator lower_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template lower_bound<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}

		template<class _Comp>
		iterator upper_bound(const _Comp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template upper_bound<iterator, _Comp>(comp, key, pFromIterator, bFindNext);
		}

		template<class _Comp>
		iterator lower_bound(const _Comp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template lower_bound<iterator, _Comp>(comp, key, pFromIterator, bFindNext);
		}

	private:

	

	};
}
