#pragma once
#include "BPIteratorSet.h"

namespace bptreedb
{


	template <class _TKey, class _TValue, class _TBTreeNode, class _TBTree>
	class TBPMapIterator : public TBPSetIterator<_TKey, _TBTreeNode, _TBTree>
	{
	public:

		typedef TBPSetIterator<_TKey, _TBTreeNode, _TBTree> TBase;
		typedef _TValue		TValue;

		typedef typename TBase::TKey      TKey;
		typedef typename TBase::TBTreeNode TBTreeNode;
		typedef typename TBase::TBPTreeNodePtr TBPTreeNodePtr;
		typedef typename TBase::TBTree TBTree;


		TBPMapIterator(TBTree *pTree, TBPTreeNodePtr pCurNode, int32_t nIndex) :
			TBase(pTree, pCurNode, nIndex)
		{
		}

		TBPMapIterator()
		{

		}

		const TValue& Value() const
		{
			return this->m_pCurNode->Value((uint32_t)this->m_nIndex);
		}

		TValue& Value()
		{
			return this->m_pCurNode->Value((uint32_t)this->m_nIndex);
		}
	};

}