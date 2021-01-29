#pragma once

#include "Compressor/CompressorParams.h"
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"
#include "../../CommonLib/alloc/stl_alloc.h"

namespace bptreedb
{

	template<class _TValue, class _TEncoder>
	class TBaseValueEncoder
	{
	public:

		typedef _TValue TValue;
		typedef CommonLib::STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TEncoder TEncoder;


		TBaseValueEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type) :
			m_encoder(pAllocsSet, pParams, type)
		{}

		~TBaseValueEncoder()
		{}

		void AddSymbol(uint32_t nSize, int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			m_encoder.AddSymbol(vecValues[nIndex]);
		}

		void RemoveSymbol(uint32_t nSize, int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			m_encoder.RemoveSymbol(vecValues[nIndex]);
		}

		void UpdateSymbol(uint32_t nIndex, TValue& newValue, const TValue& OldValue, const TValueMemSet& vecValues)
		{
			RemoveSymbol((uint32_t)vecValues.size(), nIndex, OldValue, vecValues);
			AddSymbol((uint32_t)vecValues.size(), nIndex, newValue, vecValues);
		}

		uint32_t GetCompressSize() const
		{
			return m_encoder.GetCompressSize();
		}

		void BeginEncoding(const TValueMemSet& vecValues)
		{

		}

		uint64_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream, uint32_t maxCompSize, CBPTreeContext *pContext)
		{
			size_t i = 0;
			try
			{

				if(!m_encoder.BeginEncoding(pStream))
					return vecValues.size() / 2;

				for (; i < vecValues.size(); ++i)
				{
					if(!m_encoder.EncodeSymbol(vecValues[i]))
						return vecValues.size() /2 ;
				}

				if (!m_encoder.FinishEncoding(pStream))
					return vecValues.size() / 2; //heuristic
			}
			catch (std::exception& exc_src)
			{
				CommonLib::CExcBase::RegenExcT("[BaseValueEncoder] failed encode idx: %1, count: %2", i , vecValues.size(), exc_src);
			}

			return 0;
			
		}

		void Decode(uint64_t nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream, uint32_t nCompSize, CBPTreeContext *pContext)
		{

			size_t i = 0;
			try
			{
				m_encoder.BeginDecoding(pStream);
		
				TValue value;
				for (; i < nCount; ++i)
				{
					m_encoder.DecodeSymbol(value);
					vecValues.push_back(value);
				}

				m_encoder.FinishDecoding(pStream);
			}
			catch (std::exception& exc_src)
			{
				CommonLib::CExcBase::RegenExcT("[BaseValueEncoder] failed decode idx: %1, count: %2", i, nCount, exc_src);
			}
		}

		void Clear()
		{
			m_encoder.Clear();
		}

	private:
		TEncoder m_encoder;
	};
}

