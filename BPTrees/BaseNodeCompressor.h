#pragma once

#include "Compressor/CompressorParams.h"
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"
#include "../../CommonLib/alloc/stl_alloc.h"
#include "EmptyEncoder.h"

namespace bptreedb
{

	template<class _TKey, class _TValue,  class _TKeyEncoder = TEmptyValueEncoder<_TKey>, class _TValueEncoder = TEmptyValueEncoder<_TValue> >
			class TBaseNodeCompressor
		{
		public:
			typedef _TKey TKey;
			typedef _TValue TValue;
			typedef _TKeyEncoder TKeyEncoder;
			typedef _TValueEncoder TValueEncoder;

			typedef CommonLib::STLAllocator<TKey> TKeyAlloc;
			typedef CommonLib::STLAllocator<TValue> TValueAlloc;
			typedef std::vector<TKey, TKeyAlloc> TKeyMemSet;
			typedef std::vector<TValue, TValueAlloc> TValueMemSet;


			TBaseNodeCompressor(uint32_t nPageSize, CommonLib::IAllocPtr& pAlloc, TCompressorParamsBasePtr pParams, ECompressNodeType type) : m_nCount(0),
				m_nPageSize(nPageSize), 
				m_KeyEncoder( pAlloc, pParams, type == eInnerNode ? eInnerKey : eLeafKey),
				m_ValueEncoder( pAlloc, pParams, type == eInnerNode ? eInnerValue : eLeafValue)
			{}

			
			virtual ~TBaseNodeCompressor() {}

			virtual void Load(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::IReadStream* pStream, CBPTreeContext *pContext)
			{
				try
				{
					if (pStream == nullptr)
						throw CommonLib::CExcBase("BaseNodeCompressor  read stream is zero");

					CommonLib::CReadMemoryStream KeyStream;
					CommonLib::CReadMemoryStream ValueStream;

					m_nCount = pStream->ReadIntu32();
					if (!m_nCount)
						return;

					if (m_nCount > 1000000)
						throw CommonLib::CExcBase(L"Wrong size %1", m_nCount);

					vecKeys.reserve(m_nCount);
					vecValues.reserve(m_nCount);

					uint32_t nKeySize = pStream->ReadIntu32();
					uint32_t nValueSize = pStream->ReadIntu32();

					CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream); // TO DO fix
					if (!pMemStream)
						throw CommonLib::CExcBase(L"IStream isn't memstream");

					KeyStream.AttachBuffer(pMemStream->Buffer()+ pStream->Pos(), nKeySize);
					ValueStream.AttachBuffer(pMemStream->Buffer() + pStream->Pos() + nKeySize, nValueSize);

					m_KeyEncoder.Decode(m_nCount, vecKeys, &KeyStream, nKeySize, pContext);
					m_ValueEncoder.Decode(m_nCount, vecValues, &ValueStream, nValueSize, pContext);

				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseNodeCompressor failed to read leaf node", exc_src);
				}
			}

			virtual uint32_t Write(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::IWriteStream* pStream, CBPTreeContext *pContext)
			{
				try
				{
					if (pStream == nullptr)
						throw CommonLib::CExcBase("BaseNodeCompressor  write stream is zero");

					uint32_t nSize = (uint32_t)vecKeys.size();
					if (m_nCount != nSize)
						throw CommonLib::CExcBase("BaseNodeCompressor  wrong size, count: %1, values size: %2", m_nCount, vecValues.size());

					pStream->Write(nSize);

					if (!nSize)
						return 0;

					uint32_t maxCompSize = (m_nPageSize - HeadSize()) / 2;

	
					m_KeyEncoder.BeginEncoding(vecKeys);
					m_ValueEncoder.BeginEncoding(vecValues);					
					
					size_t sizePos = pStream->Pos();

					uint32_t keySize = 0;
					uint32_t valueSize = 0;
					pStream->Write(keySize);
					pStream->Write(valueSize);

					size_t keyStartPos = pStream->Pos();
					uint32_t keys = m_KeyEncoder.Encode(vecKeys, pStream, maxCompSize, pContext);
					if (keys != 0)
						return keys;

					size_t valueStartPos = pStream->Pos();
					keySize = (uint32_t)(valueStartPos - keyStartPos);

					maxCompSize = uint32_t(pStream->Size() - valueStartPos);
					uint32_t values = m_ValueEncoder.Encode(vecValues, pStream, maxCompSize, pContext);
					if (values != 0)
						return values;

					size_t endPos =  pStream->Pos();
					valueSize = (uint32_t)(endPos - valueStartPos);

					pStream->Seek(sizePos, CommonLib::soFromBegin);
					pStream->Write(keySize);
					pStream->Write(valueSize);
					pStream->Seek(endPos, CommonLib::soFromBegin);

					return 0;

				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseNodeCompressor failed to write leaf node", exc_src);
					throw;
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
				return  sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
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
				m_KeyEncoder.Clear();
				m_ValueEncoder.Clear();
			}

		protected:

			uint32_t m_nCount;
			TKeyEncoder m_KeyEncoder;
			TValueEncoder m_ValueEncoder;

			uint32_t m_nPageSize;

		};
}

