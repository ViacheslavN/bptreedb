#pragma once

#include "BPTreeNodeHolder.h"
#include "AllocsSet.h"

namespace bptreedb
{
	template <class _TInnerNode, class _TLeafNode>
	class TBPMapNodeHolder : public TBPNodeHolder<_TInnerNode, _TLeafNode>
	{
		public:

			typedef TBPNodeHolder<_TInnerNode, _TLeafNode> TBase;
			typedef typename TBase::TLink TLink;
			typedef typename TBase::TKey TKey;
			typedef typename TBase::TInnerNode TInnerNode;
			typedef typename TBase::TLeafNode TLeafNode;
			typedef typename TLeafNode::TValue  TValue;

			typedef typename TBase::TInnerNodePtr TInnerNodePtr;
			typedef typename TBase::TLeafNodePtr TLeafNodePtr;

	public:
		TBPMapNodeHolder(TAllocsSetPtr pAllocsSet, bool bMulti, uint32_t nPageSize, bool bLeaf, int64_t nAddr, TCompressorParamsBasePtr pCompressParams) : 
			TBase(pAllocsSet, bMulti, nPageSize, bLeaf, nAddr, pCompressParams)
		{}



		template<class TComp>
		void InsertInLeaf(TComp& comp, const TKey& key, const TValue& val, int nInsertInIndex = -1)
		{
			if (!this->IsLeaf())
				throw CommonLib::CExcBase("BTNode holder insertInLeaf()  Node addr %1 isn't a leaf node", this->m_nAddr);

			this->m_pLeafNode->insert(comp, key, val, nInsertInIndex);
		}

		const TValue& Value(uint32_t nIndex) const
		{
			if (!this->IsLeaf())
				throw CommonLib::CExcBase("BTNode holder insertInLeaf()  Node addr %1 isn't a leaf node", this->m_nAddr);

			return this->m_pLeafNode->Value(nIndex);
		}

		TValue& Value(uint32_t nIndex)
		{
			if (!this->IsLeaf())
				throw CommonLib::CExcBase("BTNode holder insertInLeaf()  Node addr %1 isn't a leaf node", this->m_nAddr);

			return this->m_pLeafNode->Value(nIndex);
		}
	};
}