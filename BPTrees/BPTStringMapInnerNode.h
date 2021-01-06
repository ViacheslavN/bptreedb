#pragma once
#include "BPTreeInnerNode.h"
#include "StringVal.h"

namespace bptreedb
{
	template<class _TCompressor>
	class TBPTreeStringMapInnerNode : public  BPTreeInnerNode<StringValue, _TCompressor>
	{
		public:
			typedef BPTreeInnerNode<StringValue, _TCompressor> TBase;


			TBPTreeStringMapInnerNode(TAllocsSetPtr pAllocsSet, CommonLib::IAllocPtr pStringAlloc, bool bMulti, uint32_t nPageSize, TCompressorParamsBasePtr pParams) : TBase (pAllocsSet, bMulti, nPageSize, pParams)
			{
				m_pAllocsSet = pAllocsSet;
			}


			virtual ~TBPTreeStringMapInnerNode()
			{

				CommonLib::IAllocPtr pAlloc = m_pAllocsSet->GetAlloc(eStringAlloc);

				for (size_t i = 0, sz = this->m_innerKeyMemSet.size(); i < sz; ++i)
				{
					pAlloc->Free(this->m_innerKeyMemSet[i].m_utf8);
				}

				this->m_innerKeyMemSet.clear();

			}

		protected:
			TAllocsSetPtr m_pAllocsSet;
	};
}