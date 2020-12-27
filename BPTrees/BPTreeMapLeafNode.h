#pragma once

#include "BPTreeLeafNode.h"

namespace bptreedb
{
	template<typename _TKey, typename _TValue,
		class _TCompressor>
	class BPTreeLeafNodeMap : public  BPTreeLeafNodeSetBase<_TKey, _TCompressor>
	{
	public:

		typedef BPTreeLeafNodeSetBase<_TKey, _TCompressor> TBase;
		typedef _TValue TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TAlloc TAlloc;
		typedef typename TBase::TKeyMemSet TKeyMemSet;
		typedef CommonLib::STLAllocator<TValue> TValueAlloc;
		typedef std::vector<TValue, TValueAlloc> TValueMemSet;
  
		BPTreeLeafNodeMap(CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32_t nPageSize, TCompressorParamsBasePtr pParams) : TBase(pAlloc, bMulti, nPageSize, pParams)
		{

		}

		virtual ~BPTreeLeafNodeMap()
		{

		}
		
		virtual  uint32_t Save(CommonLib::IWriteStream* stream, CBPTreeContext *pContext)
		{
			try
			{
				stream->Write(this->m_nNext);
				stream->Write(this->m_nPrev);
				return this->m_Compressor.Write(this->m_KeyMemSet, this->m_ValueMemSet, stream, pContext);
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("Leaf node  failed to save", exc);
				throw;
			}
		}

		virtual void Load(CommonLib::IReadStream* stream, CBPTreeContext *pContext)
		{
			try
			{
				stream->Read(this->m_nNext);
				stream->Read(this->m_nPrev);
				this->m_Compressor.Load(this->m_KeyMemSet, this->m_ValueMemSet, stream, pContext);
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("Leaf node  failed to load", exc);
			}
		}

		virtual void Clear()
		{
			this->m_KeyMemSet.clear();
			this->m_ValueMemSet.clear();
			this->m_Compressor.Clear();
		}


		template<class TComp>
		void insert(TComp& comp, const TKey& key, const TValue& value, int nInsertLeafIndex = -1)
		{
			int32_t nIndex = 0;
			this->insertImp(comp, key, nIndex, nInsertLeafIndex);

			this->m_ValueMemSet.insert(std::next(this->m_ValueMemSet.begin(), nIndex), value);
			this->m_Compressor.Insert(nIndex, this->m_KeyMemSet[nIndex], this->m_ValueMemSet[nIndex], this->m_KeyMemSet, this->m_ValueMemSet);
		}

		const TValue& Value(uint32_t nIndex) const
		{
			return this->m_ValueMemSet[nIndex];
		}

		TValue& Value(uint32_t nIndex)
		{
			return this->m_ValueMemSet[nIndex];
		}

		void RemoveByIndex(int32_t nIndex)
		{
			this->m_Compressor.Remove(nIndex, this->m_KeyMemSet[nIndex], this->m_ValueMemSet[nIndex], this->m_KeyMemSet, this->m_ValueMemSet);
			this->m_KeyMemSet.erase(std::next(this->m_KeyMemSet.begin(), nIndex));
			m_ValueMemSet.erase(std::next(m_ValueMemSet.begin(), nIndex));
		}

		int32_t  SplitIn(BPTreeLeafNodeMap *pNode, int32_t count, TKey* pSplitKey)
		{
			try
			{
				if (count > (this->m_KeyMemSet.size() - 1))
					throw CommonLib::CExcBase("invalid count %1", count);

				TKeyMemSet& newNodeMemSet = pNode->m_KeyMemSet;
				TCompressor& NewNodeComp = pNode->m_Compressor;


				int32_t nBegin = (int32_t)this->m_KeyMemSet.size() - count;

				int32_t nSplitIndex = this->SplitInVec(this->m_KeyMemSet, newNodeMemSet, pSplitKey, nBegin, (int32_t)this->m_KeyMemSet.size());
				this->SplitInVec(this->m_ValueMemSet, pNode->m_ValueMemSet, (TValue*)NULL, nBegin, (int32_t)m_ValueMemSet.size());

				this->m_Compressor.Recalc(this->m_KeyMemSet, this->m_ValueMemSet);
				NewNodeComp.Recalc(newNodeMemSet, this->m_ValueMemSet);
				return nSplitIndex;
				//	}
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[BPTreeLeafNode] failed to SplitIn", exc);
				throw;
			}

		}

		int32_t  SplitIn(BPTreeLeafNodeMap *pLeftNode, BPTreeLeafNodeMap *pRightNode, TKey* pSplitKey)
		{
			try
			{
				TKeyMemSet& leftNodeMemSet = pLeftNode->m_KeyMemSet;
				TValueMemSet& leftValueMemSet = pLeftNode->m_ValueMemSet;
				TCompressor& pleftNodeComp = pLeftNode->m_Compressor;

				TKeyMemSet& rightNodeMemSet = pRightNode->m_KeyMemSet;
				TValueMemSet& rightValueMemSet = pRightNode->m_ValueMemSet;
				TCompressor& pRightNodeComp = pRightNode->m_Compressor;


				uint32_t nSize = (uint32_t)this->m_KeyMemSet.size() / 2;

				if (pSplitKey)
					*pSplitKey = this->m_KeyMemSet[nSize];

				this->SplitInVec(this->m_KeyMemSet, leftNodeMemSet, 0, nSize);
				this->SplitInVec(this->m_KeyMemSet, rightNodeMemSet, nSize, (uint32_t)this->m_KeyMemSet.size() - nSize);


				this->SplitInVec(m_ValueMemSet, leftValueMemSet, 0, nSize);
				this->SplitInVec(m_ValueMemSet, rightValueMemSet, nSize, (uint32_t)m_ValueMemSet.size() - nSize);

				pleftNodeComp.Recalc(leftNodeMemSet, leftValueMemSet);
				pRightNodeComp.Recalc(rightNodeMemSet, rightValueMemSet);

				return nSize;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[BPTreeLeafNode] failed to SplitIn", exc);
				throw;
			}

		}

		bool UnionWith(BPTreeLeafNodeMap* pNode, bool bLeft, int *nCheckIndex = 0)
		{

			this->UnionVec(this->m_KeyMemSet, pNode->m_KeyMemSet, bLeft, nCheckIndex);
			this->UnionVec(this->m_ValueMemSet, pNode->m_ValueMemSet, bLeft);

			this->m_Compressor.Recalc(this->m_KeyMemSet, this->m_ValueMemSet);

			return true;
		}

		bool AlignmentOf(BPTreeLeafNodeMap* pNode, bool bFromLeft)
		{
			if (!this->AlignmentOfVec(this->m_KeyMemSet, pNode->m_KeyMemSet, bFromLeft))
				return false;

			this->AlignmentOfVec(this->m_ValueMemSet, pNode->m_ValueMemSet, bFromLeft);

			pNode->Recalc();
			Recalc();

			return true;
		}

		void Recalc()
		{
			this->m_Compressor.Recalc(this->m_KeyMemSet, this->m_ValueMemSet);
		}

		bool IsUnion(BPTreeLeafNodeMap *pNode) {

			return this->m_Compressor.IsUnion(pNode->m_Compressor);
		}

		bool IsAlignment(BPTreeLeafNodeMap *pNode)
		{
			return this->m_Compressor.IsAlignment(pNode->m_Compressor);
		}


	public:
		TValueMemSet m_ValueMemSet;
	};
}