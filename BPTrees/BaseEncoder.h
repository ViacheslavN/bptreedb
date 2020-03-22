#pragma once

#include "Compressor/CompressorParams.h"
#include "../../commonlib/CommonLib.h"
#include "../../commonlib/exception/exc_base.h"
#include "../../commonlib/alloc/alloc.h"
#include "../../commonlib/alloc/simpleAlloc.h"
#include "../../commonlib/alloc/stl_alloc.h"

namespace bptreedb
{

	template<class _TValue, class _TEncoder, class _TCompressorParams = CompressorParamsBase>
	class TBaseValueEncoder
	{
	public:

		typedef _TValue TValue;
		typedef STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TCompressorParams TCompressorParams;
		typedef _TEncoder TEncoder;
		typedef std::shared_ptr<TCompressorParams> TCompressorParamsPtr;


		TBaseValueEncoder(uint32_t nPageSize, CommonLib::IAllocPtr& pAlloc, TCompressorParamsPtr pParams) :
			m_encoder(pAlloc, pParams)
		{}

		~TBaseValueEncoder()
		{}

	
		void  Init(TCompressorParamsPtr pParams)
		{
			m_encoder.Init(pParams);
		}

		void AddSymbol(uint32_t nSize, int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			m_encoder.AddSymbol(vecValues[nIndex]);
			m_nCount += 1;
		}

		void RemoveSymbol(uint32_t nSize, int nIndex, TValue nValue, const TValueMemSet& vecValues)
		{
			m_encoder.RemoveSymbol(vecValues[nIndex]);
			m_nCount -= 1;
		}

		uint32_t GetCompressSize() const
		{
			return m_encoder.GetCompressSize();
		}

		void BeginEncoding(const TValueMemSet& vecValues)
		{

		}

		uint64_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream, uint32_t maxCompSize)
		{
			try
			{
				if (m_nCount == vecValues.size())
					throw CommonLib::CExcBase("BaseValueEncoder  wrong size, count: %1, values size: %2", m_nCount, vecValues.size());

				m_encoder.Clear();
				m_encoder.BeginEncoding(pStream, maxCompSize);

				for (size_t i = 0; i < vecValues.size(); ++i)
				{
					if(!m_encoder.EncodeSymbol(vecValues[i]))
						return vecValues.size() - i;
				}

				if (!m_encoder.FinishEncoding(pStream))
					return vecValues.size() - 2; //heuristic
			}
			catch (std::exception& exc_src)
			{
				CommonLib::CExcBase::RegenExcT("Base encode failed encode");
			}
			
		}

		void Decode(uint64_t nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			try
			{
				if (m_nCount == vecValues.size())
					throw CommonLib::CExcBase("BaseValueEncoder  wrong size, count: %1, values size: %2", m_nCount, vecValues.size());

				m_encoder.BeginDecoding(pStream);
		
				for (size_t i = 0; i < nCount; ++i)
				{
					vecValues.push_back(m_encoder.DecodeSymbol(pStream));
				}

				m_encoder.FinishDecoding();
			}
			catch (std::exception& exc_src)
			{
				CommonLib::CExcBase::RegenExcT("Base encode failed decode");
			}
		}

		void Clear()
		{
			m_encoder.Clear();
		}

	private:
		TEncoder m_encoder;
		uint32_t m_nCount{ 0 };
	};
}

