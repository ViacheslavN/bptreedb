#pragma once

#include "Compressor/CompressorParams.h"
#include "../../commonlib/CommonLib.h"
#include "../../commonlib/exception/exc_base.h"
#include "../../commonlib/alloc/alloc.h"
#include "../../commonlib/alloc/simpleAlloc.h"
#include "../../commonlib/alloc/stl_alloc.h"

namespace bptreedb
{
	   
	template<class _TValue, class _TSignValue, class _TEncoder, class _TCompressorParams = CompressorParamsBaseImp>
	class TBaseValueDiffEncoder
	{
	public:

		typedef _TValue TValue;
		typedef STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TCompressorParams TCompressorParams;
		typedef _TEncoder TEncoder;
		typedef /*typename TDefSign<TValue>::TSignType*/_TSignValue TSignValue;


		TBaseValueDiffEncoder(uint32 nPageSize, CommonLib::IAllocPtr pAlloc, CompressorParamsBaseImp *pParams) :
			m_encoder(nPageSize, pAlloc, pParams)
		{}

		~TBaseValueDiffEncoder()
		{}
		
		template<typename _Transactions  >
		void  Init(TCompressorParamsPtr& pParams, _Transactions *pTran)
		{

		}

		virtual void Write(const TValue& value, CommonLib::IWriteStream *pStream) = 0;
		virtual void Read(TValue& value, CommonLib::IReadStream *pStream) = 0;
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

		uint32_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			try
			{
			
				if (m_nCount == vecValues.size() - 1)
					throw CommonLib::CExcBase("BaseDiffEncoder  wrong size, count: %1, values size: %2", m_nCount, vecValues.size());

				Write(vecValues[0], pStream);
				m_encoder.BeginEncoding(pStream);
		
				for (size_t i = 1; i < vecValues.size(); ++i)
				{
					if (!m_encoder.EncodeSymbol(TSignValue(vecValues[i] - vecValues[i - 1])))
						break;
				}

				return m_encoder.FinishEncoding(pStream);
			}
			catch (std::exception& exc_src)
			{
				CommonLib::CExcBase::RegenExcT("BaseDiffEncoder failed encode", exc_src);
			}
		}
		void Decode(uint32_t nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			TValue val;
			//pStream->read(val);
			Read(val, pStream);
			m_encoder.BeginDecoding(pStream);

			assert(m_encoder.count() == nCount - 1);
			vecValues.push_back(val);

			TSignValue sym;
			for (size_t i = 1; i < nCount; ++i)
			{
				m_encoder.decodeSymbol(sym);
				vecValues.push_back(sym + (TSignValue)vecValues[i - 1]);
			}

			m_encoder.FinishDecoding();
		}
		void clear()
		{
			m_encoder.clear();
		}
	protected:
		TEncoder m_encoder;
	};


	template<class _TValue, class _TSignValue, class _TEncoder, class _TCompressorParams = CompressorParamsBaseImp>
	class TValueDiffEncoder : public TBaseValueDiffEncoder<_TValue, _TSignValue, _TEncoder, _TCompressorParams>
	{
	public:
		typedef TBaseValueDiffEncoder<_TValue, _TSignValue, _TEncoder, _TCompressorParams> TBase;

		TValueDiffEncoder(uint32 nPageSize, CommonLib::alloc_t *pAlloc, CompressorParamsBaseImp *pParams) : TBase(nPageSize, pAlloc, pParams)
		{}

		virtual void Write(const TValue& value, CommonLib::IWriteStream *pStream)
		{
			pStream->write(value);
		}

		virtual void Read(TValue& value, CommonLib::IReadStream *pStream)
		{
			pStream->read(value);
		}
		virtual uint32 GetValueSize() const
		{
			return sizeof(TValue);
		}
	};
}
