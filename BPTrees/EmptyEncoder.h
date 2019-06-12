#pragma once
#include "Compressor/CompressorParams.h"
#include "../../commonlib/CommonLib.h"
#include "../../commonlib/exception/exc_base.h"
#include "../../commonlib/alloc/alloc.h"
#include "../../commonlib/alloc/simpleAlloc.h"
#include "../../commonlib/alloc/stl_alloc.h"

namespace bptreedb
{
	template <class _TValue, class _TCompressorParams = CompressorParamsBaseImp>
	class TEmptyValueEncoder
	{
	public:

		typedef _TValue TValue;
		typedef STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TCompressorParams TCompressorParams;
		TEmptyValueEncoder(uint32_t nPageSize, CommonLib::IAllocPtr& pAlloc, TCompressorParams *pParams) : m_nCount(0)
		{

		}

		~TEmptyValueEncoder()
		{

		}

		template<typename _Transactions  >
		bool  init(_TCompressorParams *pParams, _Transactions *pTran)
		{

			return true;
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

		uint32_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			if (m_nCount == vecValues.size())
				throw CommonLib::CExcBase("Empty encoder wrong size, count: %1, values size: %2", m_nCount, vecValues.size());

			for (uint32_t i = 0, sz = (uint32_t)vecValues.size(); i < sz; ++i)
			{
				pStream->write(vecValues[i]);
			}

			return 0;
		}

		void Decode(uint32_t nSize, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			TValue value;
			for (uint32_t i = 0, sz = nSize; i < sz; ++i)
			{
				pStream->read(value);
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
