#pragma once

#include "BPBaseTreeSet.h"
#include "BPIteratorSet.h"

namespace bptreedb
{
	template <	class _TKey, class _TComp, class _TStorage,
		class _TInnerNode,
		class _TLeafNode,
		class _TNodeHolder = TBPNodeHolder<_TInnerNode, _TLeafNode>
	>
	class TBPSet : public TBPlusTreeSetBase<_TKey, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder>
	{
	public:

		typedef TBPlusTreeSetBase<_TKey, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder> TBase;
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


		template <class _TK, class _TNode, class _TBTree>
		friend class TBPSetIterator;

		typedef TBPSetIterator<TKey, TBPTreeNode, TBase> iterator;



		TBPSet(int64_t nPageBTreeInfo, std::shared_ptr<TStorage> pStorage, CommonLib::IAllocPtr pAlloc, uint32_t nChacheSize, uint32_t nNodesPageSize, bool bMulti = false) :
			TBase(nPageBTreeInfo, pStorage, pAlloc, nChacheSize, nNodesPageSize, bMulti)
		{

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
	};
}