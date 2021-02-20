#pragma once

#include "BPMap.h"
#include "StringPageAlloc.h"
#include "StringVal.h"
#include "StringPageAlloc.h"

namespace bptreedb
{


	template <class _TValue, class _TComp,  class _TStorage,
		class _TInnerNode,
		class _TLeafNode,
		class _TNodeHolder = TBPMapNodeHolder<_TInnerNode, _TLeafNode>
	>

	class TBPSMap : public TBPlusTreeSetBase<StringValue, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder>
	{
	public:

		typedef TBPlusTreeSetBase<StringValue, _TComp, _TStorage, _TInnerNode, _TLeafNode, _TNodeHolder> TBase;
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

		typedef TBPMapIterator<StringValue, TValue, TBPTreeNode, TBase> iterator;

		template <class _TKey, class _TNodeHolder, class _TBTree>
		friend class TBPTreeStatistics;

		typedef TBPTreeStatistics<StringValue, TBPTreeNode, TBase> TreeStatistics;
		typedef std::shared_ptr< TreeStatistics> TreeStatisticsPtr;



		TBPSMap(int64_t nPageBTreeInfo, std::shared_ptr<TStorage> pStorage, CommonLib::IAllocPtr pAlloc, uint32_t nChacheSize, uint32_t nNodesPageSize, uint32_t nMaxStr, bool bMulti, bool bMinSplit) :
			TBase(nPageBTreeInfo, pStorage, pAlloc, nChacheSize, nNodesPageSize, bMulti, bMinSplit),
			m_nMaxStr(nMaxStr)
		{
			 


			m_StringAlloc = this->m_pAllocsSet->GetAlloc(eStringAlloc);
		}

	 

		void insert(const char *pszUtf8, uint32_t strLen, const TValue& value)
		{
			try
			{
				CommonLib::CPrefCounterHolder holder(this->m_pBPTreePerfCounter, eInsertValue);
				if (strLen > m_nMaxStr)
					throw CommonLib::CExcBase("exceeding the maximum size %1", m_nMaxStr);

				StringValue strVal ((const byte_t*)pszUtf8, (uint32_t)strLen, m_StringAlloc);
				TBPTreeNodePtr pNode = this->findLeafNodeForInsert(strVal);


				pNode->InsertInLeaf(this->m_comp, strVal, value);
				pNode->SetFlags(CHANGE_NODE, true);

				this->CheckLeafNode(pNode);
				this->CheckCache();
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[TBPSMap] failed insert", exc);
			}
		}

		virtual TBPTreeNodePtr AllocateNewNode( int64_t nAddr, bool bLeaf)
		{
			try
			{
				TBPTreeNodePtr pNode(new TBPTreeNode(this->m_pAllocsSet, this->m_bMulti, this->GetNodePageSize(), bLeaf, nAddr, this->m_pCompressParams));
				return pNode;

			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("TBPNodeHolder failed to load ", exc);
				throw;
			}

		}

		iterator begin()
		{
			return TBase::template begin<iterator>();
		}

		iterator find(const char *pszUtf8, uint32_t strLen, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			// TO DO wrap in method  create StringValue from pszUtf8
			if (strLen > m_nMaxStr)
				throw CommonLib::CExcBase("exceeding the maximum size %1", m_nMaxStr);

			StringValue key((const byte_t*)pszUtf8, (uint32_t)strLen, m_StringAlloc);
			return TBase::template find<iterator, TComp>(this->m_comp, key, pFromIterator, bFindNext);
		}

		template<class _TCustComp>
		iterator find(_TCustComp& comp, const char *pszUtf8, uint32_t strLen, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			if (strLen > m_nMaxStr)
				throw CommonLib::CExcBase("exceeding the maximum size %1", m_nMaxStr);

			StringValue key((const byte_t*)pszUtf8, (uint32_t)strLen, m_StringAlloc);

			return TBase::template find<iterator, _TCustComp>(comp, key, pFromIterator, bFindNext);
		}

		iterator upper_bound(const char *pszUtf8, uint32_t strLen, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			if (strLen > m_nMaxStr)
				throw CommonLib::CExcBase("exceeding the maximum size %1", m_nMaxStr);

			StringValue key((const byte_t*)pszUtf8, (uint32_t)strLen, m_StringAlloc);

			return TBase::template upper_bound<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}
		iterator lower_bound(const char *pszUtf8, uint32_t strLen, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			//size_t strLen = strnlen_s(pszUtf8, m_nMaxStr + 1);
			if (strLen > m_nMaxStr)
				throw CommonLib::CExcBase("exceeding the maximum size %1", m_nMaxStr);

			StringValue key((const byte_t*)pszUtf8, (uint32_t)strLen, m_StringAlloc);

			return TBase::template lower_bound<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}

		template<class _Comp>
		iterator upper_bound(const _Comp& comp, const char *pszUtf8, uint32_t strLen, iterator *pFromIterator = NULL, bool bFindNext = true)
		{

			if (strLen > m_nMaxStr)
				throw CommonLib::CExcBase("exceeding the maximum size %1", m_nMaxStr);

			StringValue key((const byte_t*)pszUtf8, (uint32_t)strLen, m_StringAlloc);

			return TBase::template upper_bound<iterator, _Comp>(comp, key, pFromIterator, bFindNext);
		}

		template<class _Comp>
		iterator lower_bound(const _Comp& comp, const char *pszUtf8, uint32_t strLen, iterator *pFromIterator = NULL, bool bFindNext = true)
		{

			if (strLen > m_nMaxStr)
				throw CommonLib::CExcBase("exceeding the maximum size %1", m_nMaxStr);

			StringValue key((const byte_t*)pszUtf8, (uint32_t)strLen, m_StringAlloc);

			return TBase::template lower_bound<iterator, _Comp>(comp, key, pFromIterator, bFindNext);
		}

		TreeStatisticsPtr GetStatistics()
		{
			return TreeStatisticsPtr(new TreeStatistics(this));
		}


		private:

			uint32_t m_nMaxStr;
			CommonLib::IAllocPtr m_StringAlloc;

	};
}