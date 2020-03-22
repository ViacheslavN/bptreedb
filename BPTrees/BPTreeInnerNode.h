#pragma once

#include "../../commonlib/CommonLib.h"
#include "../../commonlib/exception/exc_base.h"
#include "../../commonlib/alloc/alloc.h"
#include "../../commonlib/alloc/simpleAlloc.h"
#include "../../commonlib/alloc/stl_alloc.h"

#include "BPBaseTreeNode.h"

namespace bptreedb
{


template<typename _TKey, class _TCompressor>
class BPTreeInnerNode : public  IBPTreeNode
{
public:
	typedef _TKey TKey;
	typedef int64_t TLink;
	typedef _TCompressor TCompressor;

	typedef CommonLib::STLAllocator<TKey> TKeyAlloc;
	typedef CommonLib::STLAllocator<TLink> TLinkAlloc;
	typedef std::vector<TKey, TKeyAlloc> TKeyMemSet;
	typedef std::vector<TLink, TLinkAlloc> TLinkMemSet;



	typedef typename _TCompressor::TCompressorParams TInnerCompressorParams;
	typedef std::shared_ptr<TInnerCompressorParams> TInnerCompressorParamsPtr;

	BPTreeInnerNode(CommonLib::IAllocPtr& pAlloc, bool bMulti, uint32_t nPageSize) :
		m_nLess(-1), m_innerKeyMemSet(TKeyAlloc(pAlloc)), m_innerLinkMemSet(TLinkAlloc(pAlloc)), m_bMulti(bMulti),
		m_pAlloc(pAlloc), m_nPageSize(nPageSize), m_Compressor(nPageSize - sizeof(TLink), pAlloc, TInnerCompressorParamsPtr())
	{

	}

	virtual void Init(TInnerCompressorParamsPtr pParams)
	{
		m_Compressor.Init(pParams);
	}

	virtual ~BPTreeInnerNode()
	{

	}

	virtual bool IsLeaf() const { return false; }

	virtual uint32_t Size() const
	{

		return sizeof(TLink) + m_Compressor.Size();
	}

	virtual bool IsNeedSplit() const
	{
		return m_Compressor.IsNeedSplit();
	}

	virtual uint32_t HeadSize() const
	{
		return sizeof(TLink) + m_Compressor.HeadSize();
	}

	virtual uint32_t RowSize() const
	{
		return m_Compressor.RowSize();
	}

	virtual uint32_t Save(CommonLib::IWriteStream *stream)
	{
		try
		{
			stream->Write(m_nLess);
			return m_Compressor.Write(m_innerKeyMemSet, m_innerLinkMemSet, stream);
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExcT("Inner node failed to save", exc);
			throw;
		}
	}

	virtual void Load(CommonLib::IReadStream* stream)
	{
		try
		{
			stream->Read(m_nLess);
			return m_Compressor.Load(m_innerKeyMemSet, m_innerLinkMemSet, stream);
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExcT("Inner node failed to load", exc);
		}
	}
	uint32_t TupleSize() const
	{
		return m_Compressor.TupleSize();
	}

	template<class TComp>
	TLink FindNodeInsert(const TComp& comp, const TKey& key, int32_t& nIndex)
	{
		nIndex = -1;
		if (!m_bMulti)
		{
			return lower_bound(comp, key, nIndex);
		}

		auto it = std::upper_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
		if (it == m_innerKeyMemSet.end())
		{
			nIndex = (int32_t)m_innerKeyMemSet.size() - 1;
			return m_innerLinkMemSet[nIndex];
		}
		if (it == m_innerKeyMemSet.begin()) //less than all keys
		{
			nIndex = -1;
			return m_nLess;
		}
		--it;

		nIndex = (int32_t)std::distance(m_innerKeyMemSet.begin(), it);
		return m_innerLinkMemSet[nIndex];
	}

	template<class TComp>
	TLink FindNodeRemove(const TComp& comp, const TKey& key, int32_t& nIndex)
	{
		nIndex = -1;
		if (!m_bMulti)
		{
			return lower_bound(comp, key, nIndex);
		}

		auto it = std::upper_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
		if (it == m_innerKeyMemSet.end())
			return -1;

		if (it == m_innerKeyMemSet.begin()) //less than all keys
		{
			nIndex = -1;
			return m_nLess;
		}
		--it;
		nIndex = std::distance(m_innerKeyMemSet.begin(), it);
		return m_innerLinkMemSet[nIndex];
	}




	template<class TComp>
	TLink upper_bound(const TComp& comp, const TKey& key, int32_t& nIndex)
	{
		auto it = std::upper_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
		if (it == m_innerKeyMemSet.end())
			return -1;

		if (it == m_innerKeyMemSet.begin()) //less than all keys
		{
			nIndex = -1;
			return m_nLess;
		}

		nIndex = (int32_t)std::distance(m_innerKeyMemSet.begin(), it);
		return m_innerLinkMemSet[nIndex];
	}

	template<class TComp>
	TLink lower_bound(const TComp& comp, const TKey& key, int32_t& nIndex)
	{
		auto it = std::lower_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
		if (it == m_innerKeyMemSet.end())
		{
			nIndex = (int32_t)m_innerKeyMemSet.size() - 1;
			return  m_innerLinkMemSet[nIndex];
		}

		nIndex = (int32_t)std::distance(m_innerKeyMemSet.begin(), it);
		if (comp.EQ(key, m_innerKeyMemSet[nIndex]))
			return m_innerLinkMemSet[nIndex];

		if (nIndex == 0) //меньше всех ключей
		{
			nIndex = -1;
			return m_nLess;
		}
		--nIndex;
		return m_innerLinkMemSet[nIndex];
	}

	template<class TComp>
	int Insert(const TComp& comp, const TKey& key, TLink nLink)
	{
		int32_t nIndex = -1;
		short nType = 0;
		if (m_innerKeyMemSet.empty())
		{
			m_innerKeyMemSet.push_back(key);
			m_innerLinkMemSet.push_back(nLink);
			nIndex = 0;
		}
		else
		{

			if (m_bMulti)
			{
				auto it = std::upper_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
				nIndex = (int32_t)std::distance(m_innerKeyMemSet.begin(), it);
				m_innerKeyMemSet.insert(it, key);
			}
			else
			{
				auto it = std::lower_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
				if (it != m_innerKeyMemSet.end() && comp.EQ(key, (*it)))
					return false;

				nIndex = (int32_t)std::distance(m_innerKeyMemSet.begin(), it);
				m_innerKeyMemSet.insert(it, key);

			}

			m_innerLinkMemSet.insert(m_innerLinkMemSet.begin() + nIndex, nLink);

		}
		bool bRet = m_Compressor.Insert(nIndex, key, nLink, m_innerKeyMemSet, m_innerLinkMemSet);
		return bRet ? nIndex : -1;
	}

	template<class TComp>
	bool Remove(const TComp& comp, const TKey& key)
	{
		uint32_t nIndex = -1;
		bool bFind = false;
		if (m_bMulti)
		{

			nIndex = m_innerKeyMemSet.upper_bound(key, comp);
			if (nIndex && comp.EQ(key, m_innerKeyMemSet[nIndex - 1]))
			{
				bFind = true;
				nIndex -= 1;
			}
		}
		else
		{
			nIndex = m_innerKeyMemSet.lower_bound(key, comp);
		}

		if (!bFind)
		{
			return false;
		}
		return RemoveByIndex(nIndex);
	}


	void RemoveByIndex(int32_t nIndex)
	{
		m_Compressor.remove(nIndex, m_innerKeyMemSet[nIndex], m_innerLinkMemSet[nIndex], m_innerKeyMemSet, m_innerLinkMemSet);
		m_innerKeyMemSet.erase(m_innerKeyMemSet.begin() + nIndex);
		m_innerLinkMemSet.erase(m_innerLinkMemSet.begin() + nIndex);

	}

	void SplitIn(BPTreeInnerNode *pNode, int32_t count, TKey* pSplitKey)
	{
		try
		{
			if (count > (m_innerKeyMemSet.size() - 2))
				throw CommonLib::CExcBase("invalid count %1", count);

			TKeyMemSet& newNodeKeySet = pNode->m_innerKeyMemSet;
			TLinkMemSet& newNodeLinkSet = pNode->m_innerLinkMemSet;
			TCompressor& pNewNodeComp = pNode->m_Compressor;

			int32_t nBegin = (int32_t)m_innerKeyMemSet.size() - count;

			std::move(std::next(m_innerKeyMemSet.begin(), nBegin + 1), m_innerKeyMemSet.end(), std::inserter(newNodeKeySet, newNodeKeySet.begin()));
			std::move(std::next(m_innerLinkMemSet.begin(), nBegin + 1), m_innerLinkMemSet.end(), std::inserter(newNodeLinkSet, newNodeLinkSet.begin()));
			
			uint32_t nNewSize = nBegin;

			*pSplitKey = m_innerKeyMemSet[nNewSize];
			pNode->m_nLess = m_innerLinkMemSet[nNewSize];
			
			m_innerKeyMemSet.resize(nNewSize);
			m_innerLinkMemSet.resize(nNewSize);

			m_Compressor.Recalc(m_innerKeyMemSet, m_innerLinkMemSet);
			pNewNodeComp.Recalc(newNodeKeySet, newNodeLinkSet);

		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExcT("InnerNod failed to SplitIn", exc);
		}
	}

	void SplitIn(BPTreeInnerNode *pLeftNode, BPTreeInnerNode *pRightNode, TKey* pSplitKey)
	{
		try
		{
			TKeyMemSet& LeftKeySet = pLeftNode->m_innerKeyMemSet;
			TLinkMemSet& LeftLinkSet = pLeftNode->m_innerLinkMemSet;
			TCompressor& pLeftNodeComp = pLeftNode->m_Compressor;

			TKeyMemSet& RightKeySet = pRightNode->m_innerKeyMemSet;
			TLinkMemSet& RightLinkSet = pRightNode->m_innerLinkMemSet;
			TCompressor& pRightNodeComp = pRightNode->m_Compressor;

			//int nSize = m_innerKeyMemSet.size() / 2;


			uint32_t nSize = m_bMinSplit ? (uint32_t)this->m_innerKeyMemSet.size() - 2 : (uint32_t)this->m_innerKeyMemSet.size() / 2;

			std::move(m_innerKeyMemSet.begin(), m_innerKeyMemSet.begin() + nSize, std::inserter(LeftKeySet, LeftKeySet.begin()));
			std::move(m_innerLinkMemSet.begin(), m_innerLinkMemSet.begin() + nSize, std::inserter(LeftLinkSet, LeftLinkSet.begin()));


			pLeftNode->m_nLess = m_nLess;

			pLeftNodeComp.Recalc(LeftKeySet, LeftLinkSet);

			*pSplitKey = m_innerKeyMemSet[nSize];
			pRightNode->m_nLess = m_innerLinkMemSet[nSize];

			std::move(m_innerKeyMemSet.begin() + nSize + 1, m_innerKeyMemSet.end(), std::inserter(RightKeySet, RightKeySet.begin()));
			std::move(m_innerLinkMemSet.begin() + nSize + 1, m_innerLinkMemSet.end(), std::inserter(RightLinkSet, RightLinkSet.begin()));


			pRightNodeComp.Recalc(RightKeySet, RightLinkSet);


			m_innerKeyMemSet.clear();
			m_innerLinkMemSet.clear();
			m_Compressor.Clear();
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExcT("InnerNod failed to SplitIn", exc);
		}
	}

	virtual uint32_t Count() const
	{
		return (int32_t)m_innerLinkMemSet.size();
	}

	TLink Link(int32_t nIndex)
	{
		if (nIndex < m_innerLinkMemSet.size())
			return m_innerLinkMemSet[nIndex];

		throw CommonLib::CExcBase("out of range for Inner Node get link count %1, index %2", m_innerLinkMemSet.size(), nIndex);
	}

	const TKey& Key(int32_t nIndex) const
	{
		if (nIndex < m_innerKeyMemSet.size())
			return m_innerKeyMemSet[nIndex];

		throw CommonLib::CExcBase("out of range for Inner Node get key count %1, index %2", m_innerKeyMemSet.size(), nIndex);
 
	}

	TKey& Key(int32_t nIndex)
	{
		if (nIndex < m_innerKeyMemSet.size())
			return m_innerKeyMemSet[nIndex];

		throw CommonLib::CExcBase("out of range for Inner Node get key count %1, index %2", m_innerKeyMemSet.size(), nIndex);
	}

	void UpdateLink(int32_t nIndex, TLink nLink)
	{
		m_Compressor.UpdateValue(nIndex, nLink, m_innerLinkMemSet[nIndex], m_innerLinkMemSet, m_innerKeyMemSet);
		m_innerLinkMemSet[nIndex] = nLink;

	}


	void UpdateKey(int32_t nIndex, const TKey& key)
	{
		m_Compressor.UpdateKey(nIndex, key, m_innerKeyMemSet[nIndex], m_innerKeyMemSet, m_innerLinkMemSet);
		m_innerKeyMemSet[nIndex] = key;

	}

	void UnionWith(BPTreeInnerNode* pNode, const TKey* pLessMin, bool bLeft)
	{


		if (bLeft)
		{
			if (pLessMin) //can be if root is empty
			{
				pNode->m_innerKeyMemSet.push_back(*pLessMin);
				pNode->m_innerLinkMemSet.push_back(m_nLess);
			}



			pNode->m_innerKeyMemSet.reserve(pNode->m_innerKeyMemSet.size() + m_innerKeyMemSet.size());
			pNode->m_innerLinkMemSet.reserve(pNode->m_innerLinkMemSet.size() + m_innerLinkMemSet.size());


			std::move(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), std::inserter(pNode->m_innerKeyMemSet, pNode->m_innerKeyMemSet.end()));
			std::move(m_innerLinkMemSet.begin(), m_innerLinkMemSet.end(), std::inserter(pNode->m_innerLinkMemSet, pNode->m_innerLinkMemSet.end()));


			pNode->m_innerLinkMemSet.swap(m_innerLinkMemSet);
			pNode->m_innerKeyMemSet.swap(m_innerKeyMemSet);

			m_nLess = pNode->m_nLess;

			m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);
		}
		else
		{
			if (pLessMin)
			{
				m_innerKeyMemSet.push_back(*pLessMin);
				m_innerLinkMemSet.push_back(pNode->m_nLess);
			}


			m_innerKeyMemSet.reserve(pNode->m_innerKeyMemSet.size() + m_innerKeyMemSet.size());
			m_innerLinkMemSet.reserve(pNode->m_innerLinkMemSet.size() + m_innerLinkMemSet.size());


			std::move(pNode->m_innerKeyMemSet.begin(), pNode->m_innerKeyMemSet.end(), std::inserter(m_innerKeyMemSet, m_innerKeyMemSet.end()));
			std::move(pNode->m_innerLinkMemSet.begin(), pNode->m_innerLinkMemSet.end(), std::inserter(m_innerLinkMemSet, m_innerLinkMemSet.end()));

			m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);
		}
	}

	bool AlignmentOf(BPTreeInnerNode* pNode, const TKey& LessMin, bool bLeft)
	{
		int nCnt = int(((m_innerKeyMemSet.size() + pNode->m_innerKeyMemSet.size())) / 2 - m_innerKeyMemSet.size());

		if (nCnt < 1 && !m_innerKeyMemSet.empty())
			return false;

		uint32_t newSize = (uint32_t)pNode->m_innerLinkMemSet.size() - nCnt;

		if (bLeft)
		{

			uint32_t oldSize = (uint32_t)m_innerKeyMemSet.size();


			m_innerKeyMemSet.reserve(m_innerKeyMemSet.size() + nCnt + 1); //1 for less elem
			m_innerLinkMemSet.reserve(m_innerLinkMemSet.size() + nCnt + 1); //1 for less elem


			std::move(std::next(pNode->m_innerKeyMemSet.begin(), newSize + 1), pNode->m_innerKeyMemSet.end(), std::inserter(m_innerKeyMemSet, m_innerKeyMemSet.end()));
			std::move(std::next(pNode->m_innerLinkMemSet.begin(), newSize + 1), pNode->m_innerLinkMemSet.end(), std::inserter(m_innerLinkMemSet, m_innerLinkMemSet.end()));

			m_innerKeyMemSet.push_back(LessMin);
			m_innerLinkMemSet.push_back(m_nLess);

			std::rotate(m_innerKeyMemSet.begin(), std::next(m_innerKeyMemSet.begin(), oldSize), m_innerKeyMemSet.end());
			std::rotate(m_innerLinkMemSet.begin(), std::next(m_innerLinkMemSet.begin(), oldSize), m_innerLinkMemSet.end());

			m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);

			m_nLess = pNode->m_innerLinkMemSet[newSize];

			pNode->m_innerKeyMemSet.resize(newSize);
			pNode->m_innerLinkMemSet.resize(newSize);

			pNode->m_Compressor.recalc(pNode->m_innerKeyMemSet, pNode->m_innerLinkMemSet);
		}
		else
		{

			m_innerKeyMemSet.reserve(m_innerKeyMemSet.size() + nCnt + 1); //1 for less elem
			m_innerLinkMemSet.reserve(m_innerLinkMemSet.size() + nCnt + 1); //1 for less elem

			m_innerKeyMemSet.push_back(LessMin);
			m_innerLinkMemSet.push_back(pNode->m_nLess);


			std::move(pNode->m_innerKeyMemSet.begin(), std::next(pNode->m_innerKeyMemSet.begin(), nCnt - 1), std::inserter(m_innerKeyMemSet, m_innerKeyMemSet.end()));
			std::move(pNode->m_innerLinkMemSet.begin(), std::next(pNode->m_innerLinkMemSet.begin(), nCnt - 1), std::inserter(m_innerLinkMemSet, m_innerLinkMemSet.end()));


			pNode->m_nLess = pNode->m_innerLinkMemSet[nCnt - 1];



			std::rotate(pNode->m_innerKeyMemSet.begin(), std::next(pNode->m_innerKeyMemSet.begin(), nCnt), pNode->m_innerKeyMemSet.end());
			std::rotate(pNode->m_innerLinkMemSet.begin(), std::next(pNode->m_innerLinkMemSet.begin(), nCnt), pNode->m_innerLinkMemSet.end());
			pNode->m_innerKeyMemSet.resize(newSize);
			pNode->m_innerLinkMemSet.resize(newSize);


			m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);
			pNode->m_Compressor.recalc(pNode->m_innerKeyMemSet, pNode->m_innerLinkMemSet);
		}
		return true;
	}

	template<class TComp>
	bool IsKey(TComp& comp, const TKey& key, uint32_t nIndex)
	{
		return comp.EQ(key, m_innerKeyMemSet[nIndex]);
	}

	void SetMinSplit(bool bMinSplit)
	{
		m_bMinSplit = bMinSplit;
	}

	virtual void Clear()
	{
		m_innerKeyMemSet.clear();
		m_innerLinkMemSet.clear();
		m_Compressor.Clear();
	}

	bool IsHaveUnion(BPTreeInnerNode *pNode)
	{
		return this->m_Compressor.IsHaveUnion(pNode->m_Compressor);
	}

	bool IsHaveAlignment(BPTreeInnerNode *pNode)
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
	TLink m_nLess;
	TKeyMemSet m_innerKeyMemSet;
	TLinkMemSet m_innerLinkMemSet;
	bool m_bMulti;
	TCompressor  m_Compressor;
	CommonLib::IAllocPtr m_pAlloc;
	uint32_t m_nPageSize;
};
}