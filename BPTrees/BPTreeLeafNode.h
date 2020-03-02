#pragma once

#include "../../commonlib/CommonLib.h"
#include "../../commonlib/exception/exc_base.h"
#include "../../commonlib/alloc/alloc.h"
#include "../../commonlib/alloc/simpleAlloc.h"
#include "../../commonlib/alloc/stl_alloc.h"

#include "BPBaseTreeNode.h"

namespace bptreedb
{
	template<typename _TKey,
		class _Transaction, class _TCompressor>
		class BPTreeLeafNodeSetBase : public  IBPTreeNode
	{
	public:

		typedef int64_t TLink;
		typedef _TKey TKey;
		typedef _Transaction Transaction;
		typedef _TCompressor TCompressor;
		typedef CommonLib::STLAllocator<TKey> TAlloc;
		typedef std::vector<TKey, TAlloc> TKeyMemSet;

		typedef typename _TCompressor::TCompressorParams TLeafCompressorParams;

		BPTreeLeafNodeSetBase(CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32_t nPageSize) :
			m_KeyMemSet(TAlloc(pAlloc)), m_Compressor(nPageSize - 2 * sizeof(TLink), pAlloc), m_nNext(-1), m_nPrev(-1), m_bMulti(bMulti),
			m_pAlloc(pAlloc), m_nPageSize(nPageSize)

		{

		}
		~BPTreeLeafNodeSetBase()
		{

		}

		virtual bool Init(TLeafCompressorParams *pParams = NULL, Transaction* pTransaction = NULL)
		{
			return m_Compressor.init(pParams, pTransaction);
		}

		virtual bool IsLeaf() const { return true; }

		virtual uint32_t Size() const
		{

			return  2 * sizeof(TLink) + m_Compressor.size();
		}

		virtual bool IsNeedSplit() const
		{
			return m_Compressor.IsNeedSplit();
		}

		virtual uint32_t HeadSize() const
		{
			return 2 * sizeof(TLink) + m_Compressor.HeadSize();
		}

		virtual uint32_t RowSize() const
		{
			return m_Compressor.RowSize();
		}

		virtual bool IsFree() const
		{
			return !(m_nFlag & (ROOT_NODE | BUSY_NODE));
		}

		template<class TComp>
		void insertImp(TComp& comp, const TKey& key, int32_t& nIndex, int32_t nInsertLeafIndex = -1)
		{
			nIndex = -1;
			if (m_KeyMemSet.empty())
			{
				m_KeyMemSet.push_back(key);
				nIndex = 0;
			}
			else
			{
				if (nInsertLeafIndex != -1)
					nIndex = nInsertLeafIndex;
				else
				{
					if (m_bMulti)
					{
						auto it = std::upper_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
						nIndex = (int32_t)std::distance(m_KeyMemSet.begin(), it);
						m_KeyMemSet.insert(it, key);
					}
					else
					{
						auto it = std::lower_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
						if (it != m_KeyMemSet.end() && comp.EQ(*it, key))
						{
							throw CommonLib::CExcBase("Failed to insert: key exists");
						}

						nIndex = (int32_t)std::distance(m_KeyMemSet.begin(), it);
						m_KeyMemSet.insert(it, key);
					}
				}
			}
		}

		template<class TComp>
		int32_t insert(TComp& comp, const TKey& key, int nInsertLeafIndex = -1)
		{
			int32_t nIndex = 0;
			insertImp(comp, key, nIndex, nInsertLeafIndex);
			m_Compressor.insert(nIndex, m_KeyMemSet[nIndex], m_KeyMemSet);
		}
		
		template<class TComp>
		int32_t upper_bound(TComp& comp, const TKey& key)
		{
			auto it = std::upper_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
			return (int32_t)std::distance(m_KeyMemSet.begin(), it);
		}

		template<class TComp>
		int32_t lower_bound(TComp& comp, const TKey& key)
		{
			auto it = std::lower_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
			return (int32_t)std::distance(m_KeyMemSet.begin(), it);
		}

		template<class TComp>
		int32_t binary_search(TComp& comp, const TKey& key)
		{
			auto it = std::lower_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
			if (it == m_KeyMemSet.end())
				return -1;

			if (comp.EQ(*it, key))
				return (int32_t)std::distance(m_KeyMemSet.begin(), it);

			return -1;
		}

		bool Update(const TKey& key)
		{
			return true;
		}

		template<class TComp>
		void Remove(TComp& comp, const TKey& key)
		{
			uint32_t nIndex = -1;
			if (m_bMulti)
			{

				auto it = std::upper_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
				if (it == m_KeyMemSet.end())
					throw CommonLib::CExcBase("Failed to remove: key not found");

				if (it != m_KeyMemSet.begin())
					--it;
				nIndex = std::distance(m_KeyMemSet.begin(), it);

			}
			else
			{
				auto it = std::lower_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
				if (it == m_KeyMemSet.end())
					throw CommonLib::CExcBase("Failed to remove: key not found");

				nIndex = std::distance(m_KeyMemSet.begin(), it);
			}

			if (!comp.EQ(m_KeyMemSet[nIndex], key))
				throw CommonLib::CExcBase("Failed to remove: key not found");

			RemoveByIndex(nIndex);
		}

		void RemoveByIndex(uint32_t nIndex)
		{
			m_Compressor.Remove(nIndex, m_KeyMemSet[nIndex], m_KeyMemSet);
			m_KeyMemSet.erase(std::next(m_KeyMemSet.begin(), nIndex));

		}

		template<class TVector, class TVecVal>
		uint32_t SplitInVec(TVector& src, TVector& dst, TVecVal* pSplitVal, int32_t _nBegin = -1, int32_t _nEnd = -1)
		{
			uint32_t nBegin = _nBegin == -1 ? (uint32_t)src.size() / 2 : _nBegin;
			uint32_t nEnd = _nEnd == -1 ? (uint32_t)src.size() : _nEnd;

			std::move(std::next(src.begin(), nBegin), std::next(src.begin(), nEnd), std::inserter(dst, dst.begin()));
			src.resize(src.size() - (nEnd - nBegin));

			if (pSplitVal)
				*pSplitVal = dst[0];

			return (uint32_t)nBegin;

		}


		template<class TVector>
		void SplitInVec(TVector& src, TVector& dst, uint32_t nBegin, uint32_t nCount)
		{
			std::move(std::next(src.begin(), nBegin), std::next(src.begin(), nBegin + nCount), std::inserter(dst, dst.begin()));
		}

		template<class TVector, class TVecVal>
		uint32_t SplitOne(TVector& src, TVector& dst, TVecVal* pSplitVal)
		{
			uint32_t nSize = (uint32_t)src.size() - 1;
			dst.push_back(src[nSize]);
			src.resize(nSize);
			if (pSplitVal)
				*pSplitVal = dst[0];

			return (uint32_t)nSize;

		}

		int  SplitIn(BPTreeLeafNodeSetBase *pNode, TKey* pSplitKey)
		{
			TKeyMemSet& newNodeMemSet = pNode->m_KeyMemSet;
			TCompressor& NewNodeComp = pNode->m_Compressor;

			if (m_bMinSplit)
			{
				m_Compressor.remove((uint32_t)m_KeyMemSet.size() - 1, m_KeyMemSet.back(), m_KeyMemSet);
				uint32_t nSplitIndex = SplitOne(m_KeyMemSet, newNodeMemSet, pSplitKey);

				NewNodeComp.insert(0, newNodeMemSet[0], newNodeMemSet);
				return nSplitIndex;
			}
			else
			{
				int nSplitIndex = SplitInVec(m_KeyMemSet, newNodeMemSet, pSplitKey);
				m_Compressor.recalc(m_KeyMemSet);
				NewNodeComp.recalc(newNodeMemSet);
				return nSplitIndex;
			}
		}
		int  SplitIn(BPTreeLeafNodeSetBase *pLeftNode, BPTreeLeafNodeSetBase *pRightNode, TKey* pSplitKey)
		{

			TKeyMemSet& leftNodeMemSet = pLeftNode->m_KeyMemSet;
			TCompressor& pleftNodeComp = pLeftNode->m_Compressor;

			TKeyMemSet& rightNodeMemSet = pRightNode->m_KeyMemSet;
			TCompressor& pRightNodeComp = pRightNode->m_Compressor;


			uint32_t nSize = (uint32_t)m_KeyMemSet.size() / 2;

			if (pSplitKey)
				*pSplitKey = m_KeyMemSet[nSize];

			SplitInVec(m_KeyMemSet, leftNodeMemSet, 0, nSize);
			SplitInVec(m_KeyMemSet, rightNodeMemSet, nSize, (uint32_t)m_KeyMemSet.size() - nSize);

			pleftNodeComp.recalc(leftNodeMemSet);
			pRightNodeComp.recalc(rightNodeMemSet);
			return nSize;

		}

		uint32_t Count() const
		{
			return (uint32_t)m_KeyMemSet.size();
		}

		uint32_t TupleSize() const
		{
			return m_Compressor.TupleSize();
		}

		const TKey& Key(uint32_t nIndex) const
		{
			return m_KeyMemSet[nIndex];
		}

		TKey& Key(uint32_t nIndex)
		{
			return m_KeyMemSet[nIndex];
		}

		template<class TVector>
		void UnionVec(TVector& dstVec, TVector& srcVec, bool bLeft, int32_t *nCheckIndex = 0)
		{
			if (bLeft)
			{
				if (nCheckIndex)
					*nCheckIndex += (int32_t)srcVec.size();

				srcVec.reserve(srcVec.size() + dstVec.size());
				std::move(dstVec.begin(), dstVec.end(), std::inserter(srcVec, srcVec.end()));
				srcVec.swap(dstVec);
			}
			else
			{
				dstVec.reserve(srcVec.size() + dstVec.size());
				std::move(srcVec.begin(), srcVec.end(), std::inserter(dstVec, dstVec.end()));
				srcVec.clear();
			}
		}


		bool UnionWith(BPTreeLeafNodeSetBase* pNode, bool bLeft, int32_t *nCheckIndex = 0)
		{
			UnionVec(m_KeyMemSet, pNode->m_KeyMemSet, bLeft, nCheckIndex);


			this->m_Compressor.recalc(this->m_KeyMemSet);
			return true;
		}


		template<class TVector>
		bool AlignmentOfVec(TVector& dstVec, TVector& srcVec, bool bFromLeft, int *nCheckIndex = 0)
		{

			int32_t nCnt = int32_t(((dstVec.size() + srcVec.size())) / 2 - dstVec.size());
			if (nCnt <= 0)
				return false;

			uint32_t newSize = (uint32_t)srcVec.size() - nCnt;
			dstVec.reserve(dstVec.size() + nCnt);

			if (bFromLeft)
			{
				if (nCheckIndex)
					*nCheckIndex += nCnt;
				uint32_t oldSize = dstVec.size();

				std::move(std::next(srcVec.begin(), newSize), srcVec.end(), std::inserter(dstVec, dstVec.end()));
				std::rotate(dstVec.begin(), std::next(dstVec.begin(), oldSize), dstVec.end());
				srcVec.resize(newSize);
			}
			else
			{
				std::move(srcVec.begin(), std::next(srcVec.begin(), nCnt), std::inserter(dstVec, dstVec.end()));
				std::rotate(srcVec.begin(), std::next(srcVec.begin(), nCnt), srcVec.end());
				srcVec.resize(newSize);
			}
			return true;
		}

		bool AlignmentOf(BPTreeLeafNodeSetBase* pNode, bool bFromLeft)
		{
			auto& nodeMemset = pNode->m_KeyMemSet;
			TCompressor& pNodeComp = pNode->m_Compressor;
			if (!AlignmentOfVec(m_KeyMemSet, nodeMemset, bFromLeft))
				return false;

			pNodeComp.recalc(nodeMemset);
			m_Compressor.recalc(m_KeyMemSet);

			return true;
		}
		template<class TComp>
		bool IsKey(TComp& comp, const TKey& key, uint32 nIndex)
		{
			return comp.EQ(key, m_KeyMemSet[nIndex]);
		}

		void SetMinSplit(bool bMinSplit)
		{
			m_bMinSplit = bMinSplit;
		}

		virtual void clear()
		{
			m_KeyMemSet.clear();
			m_Compressor.clear();
		}

		bool IsHaveUnion(BPTreeLeafNodeSetBase *pNode)
		{

			return this->m_Compressor.IsHaveUnion(pNode->m_Compressor);
		}

		bool IsHaveAlignment(BPTreeLeafNodeSetBase *pNode)
		{
			return this->m_Compressor.IsHaveAlignment(pNode->m_Compressor);
		}

		bool IsHalfEmpty() const
		{
			return this->m_Compressor.IsHalfEmpty();
		}

		virtual  void PreSave()
		{

		}

	public:
		TCompressor  m_Compressor;
		TKeyMemSet m_KeyMemSet;
		TLink m_nNext;
		TLink m_nPrev;
		bool m_bMulti;
		CommonLib::IAllocPtr m_pAlloc;
		bool m_bMinSplit;
		uint32_t m_nPageSize;
	};

	template<typename _TKey,
		class _Transaction, class _TCompressor>
		class BPTreeLeafNode : public  BPTreeLeafNodeSetBase <_TKey, _Transaction, _TCompressor>
	{
	public:

		typedef BPTreeLeafNodeSetBase <_TKey, _Transaction, _TCompressor> TBase;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;


		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TKeyMemSet TLeafMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;

		BPTreeLeafNode(CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32 nPageSize) :
			TBase(pAlloc, bMulti, nPageSize)
		{}

		void Init(TLeafCompressorParams *pParams = NULL, Transaction* pTransaction = NULL)
		{
			this->m_Compressor.Init(pParams, pTransaction);
		}

		virtual  void Save(CommonLib::CFxMemoryWriteStream stream, TBPBaseTreeNodePtr& nextNode)
		{
			stream.Write(this->m_nNext);
			stream.Write(this->m_nPrev);
			this->m_Compressor.Write(this->m_KeyMemSet, stream);
		}

		virtual void Load(CommonLib::CReadMemoryStream& stream)
		{
			stream.Read(this->m_nNext);
			stream.Read(this->m_nPrev);
			this->m_Compressor.Load(this->m_KeyMemSet, stream);
		}

		bool IsUnion(BPTreeLeafNode *pNode) {

			return this->m_Compressor.IsUnion(pNode->m_Compressor);
		}

		bool IsAlignment(BPTreeLeafNode *pNode)
		{
			return this->m_Compressor.IsAlignment(pNode->m_Compressor);
		}
	};
}