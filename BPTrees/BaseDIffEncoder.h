#pragma once

#include "Compressor/CompressorParams.h"
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"
#include "../../CommonLib/alloc/stl_alloc.h"

namespace bptreedb
{
	   
	template<class _TValue, class _TSignValue, class _TEncoder>
	class TBaseValueDiffEncoder
	{
	public:

		typedef _TValue TValue;
		typedef _TSignValue TSignValue;
		typedef CommonLib::STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TEncoder TEncoder;
		

		TBaseValueDiffEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type) :
			m_encoder( pAllocsSet, pParams, type)
		{}

		~TBaseValueDiffEncoder()
		{}		


		virtual void Write(const TValue& value, CommonLib::IWriteStream *pStream, CBPTreeContext *pContext) = 0;
		virtual void Read(TValue& value, CommonLib::IReadStream *pStream, CBPTreeContext *pContext) = 0;
		virtual uint32_t GetValueSize() const = 0;


		void AddSymbol(uint32_t nSize, uint32_t nIndex, const TValue& nValue, const TValueMemSet& vecValues)
		{
			if (nSize > 1)
			{

				if (nIndex == 0)
				{
					AddDiffSymbol(vecValues[nIndex + 1] - nValue);
				}
				else
				{
					TValue nPrev = vecValues[nIndex - 1];
					if (nIndex == nSize - 1)
					{
						AddDiffSymbol(nValue - nPrev);
					}
					else
					{
						TValue nNext = vecValues[nIndex + 1];
						TValue nOldSymbol = nNext - nPrev;

						RemoveDiffSymbol(nOldSymbol);


						AddDiffSymbol(nValue - nPrev);
						AddDiffSymbol(nNext - nValue);
					}
				}
			}
		}

		void AddDiffSymbol(TSignValue nValue)
		{
			m_encoder.AddSymbol(nValue);
		}

		void RemoveSymbol(uint32_t nSize, int nIndex, const TValue& nValue, const TValueMemSet& vecValues)
		{
			if (vecValues.size() > 1)
			{
				if (nIndex == 0)
				{
					RemoveDiffSymbol(vecValues[nIndex + 1] - nValue);
				}
				else
				{

					if (nIndex == vecValues.size() - 1)
					{

						RemoveDiffSymbol(nValue - vecValues[nIndex - 1]);
					}
					else
					{
						TValue nPrev = vecValues[nIndex - 1];
						TValue nNext = vecValues[nIndex + 1];
						TValue nNewSymbol = nNext - nPrev;

						AddDiffSymbol(nNewSymbol);

						RemoveDiffSymbol(nValue - nPrev);
						RemoveDiffSymbol(nNext - nValue);
					}
				}
			}

		}
		void RemoveDiffSymbol(TSignValue nValue)
		{
			m_encoder.RemoveSymbol(nValue);
		}

		void UpdateSymbol(uint32_t nIndex, TValue& newValue, const TValue& OldValue, const TValueMemSet& vecValues)
		{
			RemoveSymbol((uint32_t)vecValues.size(), nIndex, OldValue, vecValues);
			AddSymbol((uint32_t)vecValues.size(), nIndex, newValue, vecValues);
		}

		uint32_t GetCompressSize() const
		{
			return m_encoder.GetCompressSize() + GetValueSize();
		}

		void BeginEncoding(const TValueMemSet& vecValues)
		{

		}

		uint32_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream, uint32_t maxCompSize, CBPTreeContext *pContext)
		{

			size_t i = 1;
			try
			{

				Write(vecValues[0], pStream, pContext);
				m_encoder.BeginEncoding(pStream);
		
				for (; i < vecValues.size(); ++i)
				{
					if (!m_encoder.EncodeSymbol(TSignValue(vecValues[i] - vecValues[i - 1])))
						return (uint32_t)vecValues.size() / 2;
				}

				if (!m_encoder.FinishEncoding(pStream))
					return (uint32_t)vecValues.size() / 2;
			}
			catch (std::exception& exc_src)
			{
				CommonLib::CExcBase::RegenExcT("[BaseDiffEncoder]  failed encode idx: %1, count: %2", i, vecValues.size(), exc_src);
				throw;
			}

			return 0;
		}

		void Decode(uint32_t nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream, uint32_t nCompSize, CBPTreeContext *pContext)
		{

			size_t i = 1;
			try
			{
				TValue val;
				Read(val, pStream, pContext);
				m_encoder.BeginDecoding(pStream);
				vecValues.push_back(val);

				TSignValue sym;
				for (; i < nCount; ++i)
				{
					m_encoder.DecodeSymbol(sym);
					vecValues.push_back(sym + vecValues[i - 1]);
				}

				m_encoder.FinishDecoding(pStream);
			}
			catch (std::exception& exc_src)
			{
				CommonLib::CExcBase::RegenExcT("[BaseDiffEncoder]  failed decode idx: %1, count: %2 ", i, nCount, exc_src);
				throw;
			}
		}

		void Clear()
		{
			m_encoder.Clear();
		}

	protected:
		TEncoder m_encoder;
	};


	template<class _TValue, class _TSignValue, class _TEncoder>
	class TValueDiffEncoder : public TBaseValueDiffEncoder<_TValue, _TSignValue, _TEncoder>
	{
	public:
		typedef TBaseValueDiffEncoder<_TValue, _TSignValue, _TEncoder> TBase;

		typedef typename TBase::TValue TValue;

		TValueDiffEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type) : TBase(pAllocsSet, pParams, type)
		{}

		virtual void Write(const TValue& value, CommonLib::IWriteStream *pStream, CBPTreeContext *pContext)
		{
			pStream->Write(value);
		}

		virtual void Read(TValue& value, CommonLib::IReadStream *pStream, CBPTreeContext *pContext)
		{
			pStream->Read(value);
		}
		virtual uint32_t GetValueSize() const
		{
			return sizeof(TValue);
		}
	};
}
