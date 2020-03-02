#pragma once

#include "Compressor/CompressorParams.h"
#include "../../commonlib/CommonLib.h"
#include "../../commonlib/exception/exc_base.h"
#include "../../commonlib/alloc/alloc.h"
#include "../../commonlib/alloc/simpleAlloc.h"
#include "../../commonlib/alloc/stl_alloc.h"
#include "EmptyEncoder.h"

namespace bptreedb
{

	template<class _TKey, class _TValue,  class _TKeyEncoder = TEmptyValueEncoder<_TKey>, class _TValueEncoder = TEmptyValueEncoder<_TValue>
		, class _TCompressorParams = CompressorParamsBaseImp>
			class TBaseNodeCompressor
		{
		public:
			typedef _TKey TKey;
			typedef _TValue TValue;
			typedef _TKeyEncoder TKeyEncoder;
			typedef _TValueEncoder TValueEncoder;

			typedef _TCompressorParams TCompressorParams;
			typedef _TCompressorParams TInnerCompressorParams; //TO DO temporary
			typedef STLAllocator<TKey> TKeyAlloc;
			typedef STLAllocator<TValue> TValueAlloc;
			typedef std::vector<TKey, TKeyAlloc> TKeyMemSet;
			typedef std::vector<TValue, TValueAlloc> TValueMemSet;


			TBaseNodeCompressor(uint32 nPageSize, CommonLib::IAllocPtr& pAlloc, TCompressorParams *pParams = nullptr) : m_nCount(0),
				m_nPageSize(nPageSize), m_KeyEncoder(nPageSize, pAlloc, pParams), m_ValueEncoder(nPageSize, pAlloc, pParams)
			{}

			template<typename _Transactions  >
			static TCompressorParams *LoadCompressorParams(_Transactions *pTran)
			{
				return new TCompressorParams();
			}

			template<typename _Transactions  >
			void  Init(TCompressorParams *pParams, _Transactions *pTran)
			{

				m_KeyEncoder.Init(pParams, pTran);
				m_ValueEncoder.Init(pParams, pTran);
			}

			virtual ~TBaseNodeCompressor() {}

			virtual void Load(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::IReadStream* pStream)
			{
				try
				{
					if (pStream == nullptr)
						throw CommonLib::CExcBase("BaseNodeCompressor  read stream is zero");

					CommonLib::CFxMemoryWriteStream KeyStream;
					CommonLib::CFxMemoryWriteStream ValueStream;

					m_nCount = pStream->ReadIntu32();
					if (!m_nCount)
						return;

					vecKeys.reserve(m_nCount);
					vecValues.reserve(m_nCount);

					uint32_t nKeySize = pStream->ReadIntu32();
					uint32_t nValueSize = pStream->ReadIntu32();

					KeyStream.AttachBuffer(pStream->Buffer() + pStream->Pos(), nKeySize);
					ValueStream.AttachBuffer(pStream->Buffer() + pStream->Pos() + nKeySize, nValueSize);

					m_KeyEncoder.Decode(m_nCount, vecKeys, &KeyStream);
					m_ValueEncoder.Decode(m_nCount, vecValues, &ValueStream);

				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseNodeCompressor failed to read leaf node", exc_src);
				}
			}

			virtual void Write(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::IWriteStream* pStream)
			{
				try
				{
					if (pStream == nullptr)
						throw CommonLib::CExcBase("BaseNodeCompressor  write stream is zero");

					uint32_t nSize = (uint32)vecKeys.size();
					if (m_nCount != nSize)
						throw CommonLib::CExcBase("BaseNodeCompressor  wrong size, count: %1, values size: %2", m_nCount, vecValues.size());

					pStream->Write(nSize);

					if (!nSize)
						return;

					CommonLib::CFxMemoryWriteStream ValueStream;
					CommonLib::CFxMemoryWriteStream KeyStream;

					m_KeyEncoder.BeginEncoding(vecKeys);
					m_ValueEncoder.BeginEncoding(vecValues);

					uint32_t nKeySize = m_KeyEncoder.GetCompressSize();
					uint32_t nValueSize = m_ValueEncoder.GetCompressSize();
					
					pStream->Write(nKeySize);
					pStream->Write(nValueSize);

					KeyStream.AttachBuffer(pStream->Buffer() + pStream->Pos(), nKeySize);
					ValueStream.AttachBuffer(pStream->Buffer() + pStream->Pos() + nKeySize, nValueSize);

					pStream->Seek(pStream->Pos() + nKeySize + nValueSize, CommonLib::soFromBegin);

					m_KeyEncoder.Encode(vecKeys, &KeyStream);
					m_ValueEncoder.Encode(vecValues, &ValueStream);
				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseNodeCompressor failed to write leaf node", exc_src);
				}
			}

			virtual bool Insert(int nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				m_nCount++;
				m_KeyEncoder.AddSymbol(m_nCount, nIndex, key, vecKeys);
				m_ValueEncoder.AddSymbol(m_nCount, nIndex, value, vecValues);

				return true;
			}

			virtual bool Add(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{

				for (uint32_t i = 0, sz = (uint32_t)vecKeys.size(); i < sz; ++i)
				{
					m_KeyEncoder.AddSymbol(m_nCount, m_nCount + i, vecKeys[i], vecKeys);
					m_ValueEncoder.AddSymbol(m_nCount, m_nCount + i, vecValues[i], vecValues);
					m_nCount++;
				}
				return true;
			}

			virtual void Recalc(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				Clear();
				for (uint32_t i = 0, sz = (uint32_t)vecKeys.size(); i < sz; ++i)
				{
					m_nCount += 1;
					m_KeyEncoder.AddSymbol(m_nCount, i, vecKeys[i], vecKeys);
					m_ValueEncoder.AddSymbol(m_nCount, i, vecValues[i], vecValues);

				}
			}

			virtual void UpdateValue(uint32_t nIndex, TValue& newValue, const TValue& OldValue, const TValueMemSet& vecValues, const TKeyMemSet& vecKeys)
			{
				//	m_ValueEncoder.RemoveSymbol(m_nCount, nIndex, OldValue, vecValues);
				//	m_ValueEncoder.AddSymbol(m_nCount, nIndex, newValue, vecValues);

				m_ValueEncoder.UpdateSymbol(nIndex, newValue, OldValue, vecValues);


			}

			virtual void UpdateKey(uint32_t nIndex, const TKey& NewKey, const TKey& OldTKey, const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				m_KeyEncoder.RemoveSymbol(m_nCount, nIndex, OldTKey, vecKeys);
				m_KeyEncoder.AddSymbol(m_nCount, nIndex, NewKey, vecKeys);
			}

			virtual void Remove(uint32_t nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				m_nCount--;
				m_KeyEncoder.RemoveSymbol(m_nCount, nIndex, key, vecKeys);
				m_ValueEncoder.RemoveSymbol(m_nCount, nIndex, value, vecValues);
			}

			virtual uint32_t Size() const
			{
				return RowSize() + HeadSize();
			}

			virtual bool IsNeedSplit() const
			{
				if (m_nCount > m_nPageSize * 8) //max bits for elem
					return true;

				return !(m_nPageSize > Size());
			}

			virtual uint32_t Count() const
			{
				return m_nCount;
			}

			uint32_t HeadSize() const
			{
				return  sizeof(uint32) + sizeof(uint32) + sizeof(uint32);
			}

			uint32_t RowSize() const
			{
				return  m_ValueEncoder.GetCompressSize() + m_KeyEncoder.GetCompressSize();
			}

			uint32_t TupleSize() const
			{
				return  (sizeof(TKey) + sizeof(TValue));
			}			

			virtual void RecalcKey(const TKeyMemSet& vecKeys)
			{
				m_nCount = 0;
				m_KeyEncoder.Clear();
				for (uint32_t i = 0, sz = (uint32_t)vecKeys.size(); i < sz; ++i)
				{
					m_nCount++;
					m_KeyEncoder.AddSymbol(m_nCount, i, vecKeys[i], vecKeys);
				}
			}
			
			bool IsHaveUnion(TBaseNodeCompressor& pCompressor) const
			{
				if ((m_nCount + pCompressor.m_nCount) > m_nPageSize * 8) //max bits for elem
					return false;

				return (RowSize() + pCompressor.RowSize()) < (m_nPageSize - HeadSize());
			}

			bool IsHaveAlignment(TBaseNodeCompressor& pCompressor) const
			{
				uint32_t nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TValue));
				return nNoCompSize < (m_nPageSize - HeadSize());
			}

			bool IsHalfEmpty() const
			{
				uint32_t nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TValue));
				return nNoCompSize < (m_nPageSize - HeadSize()) / 2;
			}

			void Clear()
			{
				m_nCount = 0;
				m_KeyEncoder.clear();
				m_ValueEncoder.clear();
			}

		protected:

			uint32_t m_nCount;
			TKeyEncoder m_KeyEncoder;
			TValueEncoder m_ValueEncoder;

			uint32_t m_nPageSize;

		};
}

