#pragma once
#include "Compressor/CompressorParams.h"
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"
#include "../../CommonLib/alloc/stl_alloc.h"
#include "AllocsSet.h"

namespace bptreedb
{
	template <class _TValue>
	class TEmptyValueEncoder
	{
	public:

		typedef _TValue TValue;
		typedef CommonLib::STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;


		TEmptyValueEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type) : m_nCount(0)
		{

		}

		~TEmptyValueEncoder()
		{

		}

		void AddSymbol(uint32_t nSize, int nIndex, const TValue& value, const TValueMemSet& vecValues)
		{
			m_nCount++;
		}

		void RemoveSymbol(uint32_t nSize, int nIndex, const TValue& value, const TValueMemSet& vecValues)
		{
			m_nCount--;
		}

		void UpdateSymbol(int nIndex, TValue& newValue, const TValue& oldValue, const TValueMemSet& vecValues)
		{

		}
		
		uint32_t GetCompressSize() const
		{
			return m_nCount * sizeof(TValue);
		}

		void BeginEncoding(const TValueMemSet& vecValues)
		{

		}

		uint32_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream, uint32_t maxCompSize, CBPTreeContext *pContext)
		{
			if (m_nCount != vecValues.size())
				throw CommonLib::CExcBase("Empty encoder wrong size, count: %1, values size: %2", m_nCount, vecValues.size());

			for (uint32_t i = 0, sz = (uint32_t)vecValues.size(); i < sz; ++i)
			{
				if (!pStream->WriteSafe(vecValues[i]))
					return i;
			}

			return 0;
		}

		void Decode(uint32_t nSize, TValueMemSet& vecValues, CommonLib::IReadStream *pStream, uint32_t nCompSize, CBPTreeContext *pContext)
		{
			TValue value;
			for (uint32_t i = 0, sz = nSize; i < sz; ++i)
			{
				pStream->Read(value);
				vecValues.push_back(value);
			}

			m_nCount = nSize;
		}

		void Clear()
		{
			m_nCount = 0;
		}

		uint32_t Count() const
		{
			return m_nCount;
		}

	protected:
		uint32_t m_nCount;

	};
}
