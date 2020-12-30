#pragma once

#include "../../CommonLib/stream/MemoryStream.h"
#include "Compressor/CompressorParams.h"

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

 

	public:
		TBPNodeHolder(CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32_t nPageSize, bool bLeaf, int64_t nAddr, TCompressorParamsBasePtr pCompressParams) :
			m_IsLeaf(bLeaf),
			m_nAddr(nAddr),
			m_nParentAddr(-1),
			m_nFoundIndex(-1)
		{

			if (m_IsLeaf)
			{
				m_pLeafNode.reset(new TLeafNode(pAlloc, bMulti, nPageSize, pCompressParams));
				m_pCurNode = m_pLeafNode.get();
			}
			else
			{
				m_pInnerNode.reset(new TInnerNode(pAlloc, bMulti, nPageSize, pCompressParams));
				m_pCurNode = m_pInnerNode.get();
			}

		}
 

		void Load(CommonLib::IReadStream* pStream, CBPTreeContext *pContext)
		{
			m_pCurNode->Load(pStream, pContext);
		}

	/*	void InitCopmressor(TInnerCompressorParamsPtr& innerComp, TLeafCompressorParamsPtr& leafComp)
		{
			if (IsLeaf())
				m_pLeafNode->Init(leafComp);
			else
				m_pInnerNode->Init(innerComp);
		}
		*/
		uint32_t Save(CommonLib::IWriteStream *pStream, CBPTreeContext *pContext)
		{
			try
			{
				pStream->Write(m_IsLeaf);
				return m_pCurNode->Save(pStream, pContext);
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("TBPNodeHolder failed to save addr %1 leaf: %2", m_nAddr, m_IsLeaf, exc);
				throw;
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

		int32_t GetFoundIndex() const
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

		void SetFlags(uint32_t flag, bool bSet)
		{
			m_pCurNode->SetFlags(flag, bSet);
		}

		uint32_t GetFlags() const
		{
			return m_pCurNode->GetFlags();
		}

		uint32_t Count() const
		{
			return m_pCurNode->Count();
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

		TLink GetNext() const
		{
			if (IsLeaf())
				return m_pLeafNode->m_nNext;

			throw CommonLib::CExcBase("BTNode holder Next()  Node addr %1 isn't a leaf node", m_nAddr);
		}

		TLink GetPrev() const
		{
			if (IsLeaf())
				return m_pLeafNode->m_nPrev;

			throw CommonLib::CExcBase("BTNode holder Prev()  Node addr %1 isn't a leaf node", m_nAddr);
		}

		void SetNext(TLink next)
		{
			if (IsLeaf())
				 m_pLeafNode->m_nNext = next;
			else
				throw CommonLib::CExcBase("BTNode holder SetNext()  Node addr %1 isn't a leaf node", m_nAddr);
		}

		void SetPrev(TLink prev)
		{
			if (IsLeaf())
				m_pLeafNode->m_nPrev = prev;
			else
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
		TLink FindNodeRemove(const TComp& comp, const TKey& key, int32_t& nIndex)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder findNodeInsert()  Node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->FindNodeInsert(comp, key, nIndex);
		}


		template<class TComp>
		void InsertInLeaf(TComp& comp, const TKey& key, int nInsertLeafIndex = -1)
		{
			if (!IsLeaf())
				throw CommonLib::CExcBase("BTNode holder insertInLeaf()  Node addr %1 isn't a leaf node", m_nAddr);

			m_pLeafNode->insert(comp, key, nInsertLeafIndex);
		}

		template<class TComp>
		int InsertInInnerNode(TComp& comp, const TKey& key, TLink nLink)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder InsertInInnerNode()  Node addr %1 isn't an inner node", m_nAddr);

			return m_pInnerNode->Insert(comp, key, nLink);
		}
		

		uint32_t SplitIn(std::shared_ptr<TBPNodeHolder>  pNewNode, int32_t count, TKey* pSplitKey)
		{
			if (IsLeaf())
				return m_pLeafNode->SplitIn(pNewNode->m_pLeafNode.get(), count, pSplitKey);

			m_pInnerNode->SplitIn(pNewNode->m_pInnerNode.get(), count, pSplitKey);
			return 0;
		}

		uint32_t SplitIn(std::shared_ptr<TBPNodeHolder> pLeftNode, std::shared_ptr<TBPNodeHolder> pRightNode, TKey* pSplitKey)
		{
			if (IsLeaf())
				return m_pLeafNode->SplitIn(pLeftNode->m_pLeafNode.get(), pRightNode->m_pLeafNode.get(), pSplitKey);

			m_pInnerNode->SplitIn(pLeftNode->m_pInnerNode.get(), pRightNode->m_pInnerNode.get(), pSplitKey);
			return 0;
		}

		void UnionWithLeafNode(std::shared_ptr<TBPNodeHolder>  pNode, bool bLeft,  int *nCheckIndex = 0)
		{
			if (!IsLeaf())
				throw CommonLib::CExcBase("BTNode holder UnionWithLeafNode()  Node addr %1 isn't a leaf node", m_nAddr);

			m_pLeafNode->UnionWith(pNode->m_pLeafNode.get(), bLeft, nCheckIndex);
		}

		void UnionWithInnerNode(std::shared_ptr<TBPNodeHolder>  pNode, const TKey* lessMin, bool bLeft)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder UnionWithInnerNode()  Node addr %1 isn't a inner node", m_nAddr);

			m_pInnerNode->UnionWith(pNode->m_pInnerNode.get(), lessMin, bLeft);

		}

		bool AlignmentInnerNodeOf(std::shared_ptr<TBPNodeHolder> pNode, const TKey& lessMin, bool bLeft)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase("BTNode holder AlignmentInnerNodeOf()  Node addr %1 isn't a inner node", m_nAddr);

			return m_pInnerNode->AlignmentOf(pNode->m_pInnerNode.get(), lessMin, bLeft);
		}


		bool  AlignmenLeftNodeOf(std::shared_ptr<TBPNodeHolder> pNode, bool bLeft)
		{
			if (!IsLeaf())
				throw CommonLib::CExcBase("BTNode holder AlignmentInnerNodeOf()  Node addr %1 isn't a left node", m_nAddr);

			return m_pLeafNode->AlignmentOf(pNode->m_pLeafNode.get(), bLeft);
		}

		void TransformToInner(CommonLib::IAllocPtr pAlloc, bool bMulti, uint32_t nPageSize, TCompressorParamsBasePtr pCompressParams)
		{
			try
			{
				if (!IsLeaf())
					throw CommonLib::CExcBase(" Node addr %1 isn't a leaf node", m_nAddr);

				m_pInnerNode.reset(new TInnerNode(pAlloc, bMulti, nPageSize, pCompressParams));
			//	m_pInnerNode->Init(innerComp);
				m_IsLeaf = false;
				m_pLeafNode.reset();

				m_pCurNode = m_pInnerNode.get();
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("BTNode holder transformToInner failed", exc);
			}
		}


		void TransformToLeaf(CommonLib::IAllocPtr pAlloc, bool bMulti, uint32_t nPageSize, TCompressorParamsBasePtr pCompressParams)
		{
			try
			{
				if (IsLeaf())
					throw CommonLib::CExcBase(" Node addr %1 isn't a inner node", m_nAddr);

				m_pLeafNode.reset(new TLeafNode(pAlloc, bMulti, nPageSize, pCompressParams));
				//	m_pInnerNode->Init(innerComp);
				m_IsLeaf = true;
				m_pInnerNode.reset();

				m_pCurNode = m_pLeafNode.get();
		 

			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("BTNode holder TransformToLeaf failed", exc);
			}
		}
		
		bool IsNeedSplit() const
		{
		//	if (Count() > 100)
		//		return true;

			return m_pCurNode->IsNeedSplit();
		}

		void Clear()
		{
			m_pCurNode->Clear();
		}

		bool IsHalfEmpty()
		{			
		//	if (Count() < 50)
		//		return true;

			return m_pCurNode->IsHalfEmpty();
		}

		bool PossibleUnion(std::shared_ptr<TBPNodeHolder> pNode) const
		{
			if (IsLeaf()) 
				return m_pLeafNode->PossibleUnion(pNode->m_pLeafNode.get());

			return m_pInnerNode->PossibleUnion(pNode->m_pInnerNode.get());
		}

		bool PossibleAlignment(std::shared_ptr<TBPNodeHolder> pNode) const
		{
			if (IsLeaf())
				return m_pLeafNode->PossibleAlignment(pNode->m_pLeafNode.get());

			return m_pInnerNode->PossibleAlignment(pNode->m_pInnerNode.get());
		}

		void RemoveByIndex(uint32_t nIndex)
		{
			if (IsLeaf())
				m_pLeafNode->RemoveByIndex(nIndex);
			else
				m_pInnerNode->RemoveByIndex(nIndex);
		}

		template<class TComp>
		bool IsKey(TComp& comp, const TKey& key, uint32_t nIndex)
		{
			if (IsLeaf())
				return m_pLeafNode->IsKey(comp, key, nIndex);
			else
				return m_pInnerNode->IsKey(comp, key, nIndex);
		}

		void UpdateKey(int32_t nIndex, const TKey& Key)
		{
			if (IsLeaf())
				throw CommonLib::CExcBase(" Node addr %1 isn't a inner node", m_nAddr);

			return m_pInnerNode->UpdateKey(nIndex, Key);
		}

	protected:
		bool m_IsLeaf;
		int64_t m_nAddr;
		std::weak_ptr<TBPNodeHolder> m_pParent;
		int32_t m_nFoundIndex;
		int64_t m_nParentAddr;

		TInnerNodePtr m_pInnerNode;
		TLeafNodePtr m_pLeafNode;

		IBPTreeNode *m_pCurNode;

	};
}