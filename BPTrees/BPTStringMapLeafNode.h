#pragma once

#include "BPTreeMapLeafNode.h"
#include "StringVal.h"

namespace bptreedb
{
	template<typename _TValue, class _TCompressor>
	class TBPTreeStringMapLeafNode : public BPTreeLeafNodeMap<StringValue, _TValue, _TCompressor>
	{

	public:

		typedef BPTreeLeafNodeMap<StringValue, _TValue, _TCompressor> TBase;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TAlloc TAlloc;
		typedef typename TBase::TKeyMemSet TKeyMemSet;
		//typedef typename TBase::TValueAlloc;
		typedef typename TBase::TValueMemSet TValueMemSet;

		TBPTreeStringMapLeafNode(TAllocsSetPtr pAllocsSet, bool bMulti, uint32_t nPageSize, TCompressorParamsBasePtr pParams) : TBase(pAllocsSet, bMulti, nPageSize, pParams)
		{
	//		m_pAllocsSet = pAllocsSet;
		}

		~TBPTreeStringMapLeafNode()
		{

		/*	CommonLib::IAllocPtr pAlloc = m_pAllocsSet->GetAlloc(eStringAlloc);

			for (size_t i = 0, sz = this->m_KeyMemSet.size(); i < sz; ++i)
			{
				pAlloc->Free(this->m_KeyMemSet[i].m_utf8);
			}

			this->m_KeyMemSet.clear();*/
		}


	private:
	//	TAllocsSetPtr m_pAllocsSet;

	};



}