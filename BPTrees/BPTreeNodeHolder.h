#pragma once

#include "../../CommonLib/stream/MemoryStream.h"

namespace bptreedb
{
 

	template <class _TInnerNode, class _TLeafNode>
	class TBPNodeHolder
	{
	public:
		typedef int64_t TLink;
		typedef typename _TInnerNode::TKey TKey;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode TLeafNode;

		typedef std::shared_ptr<TInnerNode> TInnerNodePtr;
		typedef std::shared_ptr<TLeafNode> TLeafNodePtr;


		typedef typename TInnerNode::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TLeafNode::TLeafCompressorParams TLeafCompressorParams;

		typedef std::shared_ptr< TInnerCompressorParams> TInnerCompressorParamsPtr;
		typedef std::shared_ptr< TLeafCompressorParams> TLeafCompressorParamsPtr;

	private:
		TBPNodeHolder(CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32_t nPageSize, bool bLeaf, int64_t nAddr) :
			m_IsLeaf(m_IsLeaf),
			m_nAddr(nAddr),
			m_nParentAddr(-1),
			m_nFoundIndex(-1)
		{

			if (m_IsLeaf)
				m_pLeafNode.reset(new TLeafNode(pAlloc,  bMulti,  nPageSize));
			else
				m_pInnerNode.reset(new TInnerNode(pAlloc, bMulti, nPageSize));

		}
	public:

		static std::shared_ptr<TBPNodeHolder> Load(CommonLib::IReadStream *pStream, CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32_t nPageSize, int64_t nAddr)
		{
			try
			{
				bool isLeaf = pStream->ReadBool();

				std::shared_ptr<TBPNodeHolder> pNode(new TBPNodeHolder(pAlloc, bMulti, nPageSize, isLeaf, nAddr));

				if (isLeaf)
					pNode->m_pLeafNode.Load(pStream);
				else

					pNode->m_pInnerNode.Load(pStream);

				return pNode;

			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("TBPNodeHolder failed to load ",exc);
			}
			
		}

		static std::shared_ptr<TBPNodeHolder> Create(CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32_t nPageSize, int64_t nAddr, bool bLeaf)
		{
			try
			{
				std::shared_ptr<TBPNodeHolder> pNode(new TBPNodeHolder(pAlloc, bMulti, nPageSize, bLeaf, nAddr));
				return pNode;

			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("TBPNodeHolder failed to load ", exc);
			}

		}

		void InitCopmressor(TInnerCompressorParamsPtr& innerComp, TLeafCompressorParamsPtr& leafComp)
		{
			if (IsLeaf())
				m_pLeafNode->Init(leafComp);
			else
				m_pInnerNode->Init(innerComp);
		}
		
		uint32_t Save(CommonLib::IWriteStream *pStream)
		{
			try
			{
				pStream->Write(m_IsLeaf);

				if (IsLeaf())
					return m_pLeafNode.Save(pStream);
				else
					return m_pInnerNode.Save(pStream);
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("TBPNodeHolder failed to save addr %1 leaf: %2", m_nAddr, m_IsLeaf, exc);
			}
		}


		bool IsLeaf() const { return m_IsLeaf; }


		void SetParent(std::shared_ptr<TBPNodeHolder> pNode, int32_t nFoundIndex)
		{
			m_pParent = pNode;
			m_nFoundIndex = nFoundIndex;

			if (pNode.get())
				m_nParentAddr = pNode->GetAddr();
			else
				m_nParentAddr = -1;
		}

		std::shared_ptr<TBPNodeHolder> GetParentNodePtr()
		{ 
			return m_pParent.lock(); 
		}

		int64_t GetParentAddr() const
		{
			return m_nParentAddr;
		}

		int64_t GetFoundIndex() const
		{
			return m_nFoundIndex;
		}

		void SetFoundIndex(int32_t nFoundIndex)
		{
			m_nFoundIndex = nFoundIndex;
		}

		uint64_t GetAddr() const
		{
			return m_nAddr;
		}

		uint32_t GetFlags() const
		{
			if (IsLeaf())
				return m_pLeafNode->GetFlags();
			else
				return m_pInnerNode->GetFlags();
		}

		uint32_t Count() const
		{
			if (IsLeaf())
				return m_pLeafNode->Count();
			else
				return m_pInnerNode->Count();
		}

		const TKey& Key(uint32_t nIndex) const
		{
			if (IsLeaf())
				return m_pLeafNode->Key(nIndex);
			else
				return m_pInnerNode->Key(nIndex);
		}

		TKey& Key(uint32_t nIndex)
		{
			if (IsLeaf())
				return m_pLeafNode->Key(nIndex);
			else
				return m_pInnerNode->Key(nIndex);
		}

		TLink BackLink() const
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("TBPNodeHolder BackLink failed node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->m_innerLinkMemSet.back();
		}

		TLink FrontLink() const
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("TBPNodeHolder FrontLink failed node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->m_innerLinkMemSet.front();
		}

		TLink Link(int32_t nIndex)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("TBPNodeHolder Link failed node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->Link(nIndex);
		}

		TLink Next() const
		{
			if (IsLeaf())
				return m_pLeafNode->m_nNext;

			throw CommonLib::CExcBase("BTNode holder Next()  Node addr %1 isn't a leaf node", m_nAddr);
		}

		TLink Prev() const
		{
			if (IsLeaf())
				return m_pLeafNode->m_nPrev;

			throw CommonLib::CExcBase("BTNode holder Prev()  Node addr %1 isn't a leaf node", m_nAddr);
		}

		void SetNext(TLink next)
		{
			if (IsLeaf())
				 m_pLeafNode->m_nNext = next;

			throw CommonLib::CExcBase("BTNode holder SetNext()  Node addr %1 isn't a leaf node", m_nAddr);
		}

		void SetPrev(TLink prev)
		{
			if (IsLeaf())
				m_pLeafNode->m_nPrev = prev;

			throw CommonLib::CExcBase("BTNode holder SetPrev()  Node addr %1 isn't a leaf node", m_nAddr);
		}
		
		template<class TComp>
		int32_t binary_search(const TComp& comp, const TKey& key)
		{
			if (!IsLeaf())
				throw CommonLib::CExcBase("BTNode holder binary_search()  Node addr %1 isn't a leaf node", m_nAddr);

			return m_pLeafNode->binary_search(comp, key);
		}

		template<class TComp>
		TLink inner_lower_bound(const TComp& comp, const TKey& key, int32_t& nIndex)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder inner_lower_bound()  Node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->lower_bound(comp, key, nIndex);
		}

		template<class TComp>
		TLink  inner_upper_bound(const TComp& comp, const TKey& key, int32_t& nIndex)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder inner_upper_bound()  Node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->upper_bound(comp, key, nIndex);
		}

		template<class TComp>
		int32_t leaf_lower_bound(const TComp& comp, const TKey& key)
		{
			if (!IsLeaf())
				throw CommonLib::CExcBase("BTNode holder leaf_lower_bound()  Node addr %1 isn't a leaf node", m_nAddr);

			return m_pLeafNode->lower_bound(comp, key);
		}

		template<class TComp>
		int32_t  leaf_upper_bound(const TComp& comp, const TKey& key)
		{
			if (!IsLeaf())
				throw CommonLib::CExcBase("BTNode holder leaf_upper_bound()  Node addr %1 isn't a leaf node", m_nAddr);

			return m_pLeafNode->upper_bound(comp, key);
		}

		TLink Less()
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder less Node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->m_nLess;
		}

		void SetLess(TLink nLess)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder set less Node addr %1 isn't an inner node", m_nAddr);

			m_pInnerNode->m_nLess = nLess;
		}

		template<class TComp>
		TLink FindNodeInsert(const TComp& comp, const TKey& key, int32_t& nIndex)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder findNodeInsert()  Node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->FindNodeInsert(comp, key, nIndex);
		}


		template<class TComp>
		int InsertInLeaf(TComp& comp, const TKey& key, int nInsertLeafIndex = -1)
		{
			if (!IsLeaf())
				throw CommonLib::CExcBase("BTNode holder insertInLeaf()  Node addr %1 isn't a leaf node", m_nAddr);

			return m_pLeafNode->insert(comp, key, nInsertLeafIndex);
		}

		template<class TComp>
		int InsertInInnerNode(TComp& comp, const TKey& key, TLink nLink)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder InsertInInnerNode()  Node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->insert(comp, key, nLink);
		}
		

		uint32_t SplitIn(std::shared_ptr<TBPNodeHolder>&  pNewNode, TKey* pSplitKey)
		{
			if (IsLeaf())
				return m_pLeafNode.SplitIn(&pNewNode->m_pLeafNode, pSplitKey);

			m_pInnerNode.SplitIn(&pNewNode->m_pInnerNode, pSplitKey);
			return 0;
		}

		uint32_t SplitIn(std::shared_ptr<TBPNodeHolder>& pLeftNode, std::shared_ptr<TBPNodeHolder>& pRightNode, TKey* pSplitKey)
		{
			if (IsLeaf())
				return m_pLeafNode.SplitIn(&pLeftNode->m_pLeafNode, &pRightNode->m_pLeafNode, pSplitKey);

			m_pInnerNode.SplitIn(&pLeftNode->m_pInnerNode, &pRightNode->m_pInnerNode, pSplitKey);
			return 0;
		}

		void TransformToInner(CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32_t nPageSize, TInnerCompressorParamsPtr& innerComp)
		{
			try
			{
				if (!IsLeaf())
					throw CommonLib::CExcBase(" Node addr %1 isn't a leaf node", m_nAddr);

				m_pInnerNode.reset(new TInnerNode(pAlloc, bMulti, nPageSize));
				m_pInnerNode->Init(innerComp);
				m_IsLeaf = true;
				m_pLeafNode.reset();
			}
			catch (std::exception exc)
			{
				CommonLib::CExcBase::RegenExcT("BTNode holder transformToInner failed", exc);
			}
		}

	private:
		bool m_IsLeaf;
		int64_t m_nAddr;
		std::weak_ptr<TBPNodeHolder> m_pParent;
		int32_t m_nFoundIndex;
		int64_t m_nParentAddr;

		TInnerNodePtr m_pInnerNode;
		TLeafNodePtr m_pLeafNode;

	};
}