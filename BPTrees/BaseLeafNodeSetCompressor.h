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
	template<class _TKey, class _TKeyEncoder = TEmptyValueEncoder<_TKey>,
		 class _TCompressorParams = CompressorParamsBaseImp>
			class TBaseLeafNodeSetCompressor
		{
		public:
			typedef _TKey TKey;
			typedef _TKeyEncoder TKeyEncoder;
			typedef _TCompressorParams TCompressorParams;
			typedef STLAllocator<TKey> TKeyAlloc;
			typedef std::vector<TKey, TKeyAlloc> TKeyMemSet;


			TBaseLeafNodeSetCompressor(uint32 nPageSize, CommonLib::IAllocPtr& pAlloc, TCompressorParams *pParams = nullptr) : m_nCount(0),
				m_nPageSize(nPageSize), m_KeyEncoder(nPageSize, pAlloc, pParams)
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

			virtual void Load(TKeyMemSet& vecKeys, CommonLib::IReadStream* pStream)
			{
				try
				{
					if (pStream == nullptr)
						throw CommonLib::CExcBase("BaseNodeCompressor  read stream is zero");

					CommonLib::CFxMemoryWriteStream KeyStream;
					m_nCount = pStream->ReadIntu32();
					if (!m_nCount)
						return;

					vecKeys.reserve(m_nCount);
					uint32_t nKeySize = pStream->ReadIntu32();
					
					KeyStream.AttachBuffer(pStream->Buffer() + pStream->Pos(), nKeySize);
					m_KeyEncoder.Decode(m_nCount, vecKeys, &KeyStream);
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

					CommonLib::CFxMemoryWriteStream KeyStream;

					m_KeyEncoder.BeginEncoding(vecKeys);
					uint32_t nKeySize = m_KeyEncoder.GetCompressSize();


					pStream->Write(nKeySize);

					KeyStream.AttachBuffer(pStream->Buffer() + pStream->Pos(), nKeySize);
					pStream->Seek(pStream->Pos() + nKeySize, CommonLib::soFromBegin);

					m_KeyEncoder.Encode(vecKeys, &KeyStream);
				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseNodeCompressor failed to write leaf node", exc_src);
				}
			}

			virtual void Insert(int nIndex, const TKey& key,const TKeyMemSet& vecKeys)
			{
				m_nCount++;
				m_KeyEncoder.AddSymbol(m_nCount, nIndex, key, vecKeys);
			}

			virtual void Add(const TKeyMemSet& vecKeys)
			{

				for (uint32_t i = 0, sz = (uint32_t)vecKeys.size(); i < sz; ++i)
				{
					m_KeyEncoder.AddSymbol(m_nCount, m_nCount + i, vecKeys[i], vecKeys);
					m_nCount++;
				}
			}

			virtual void Recalc(const TKeyMemSet& vecKeys)
			{
				Clear();
				for (uint32_t i = 0, sz = (uint32_t)vecKeys.size(); i < sz; ++i)
				{
					m_nCount += 1;
					m_KeyEncoder.AddSymbol(m_nCount, i, vecKeys[i], vecKeys);
				}
			}
						

			virtual void UpdateKey(uint32_t nIndex, const TKey& NewKey, const TKey& OldTKey, const TKeyMemSet& vecKeys)
			{
				m_KeyEncoder.RemoveSymbol(m_nCount, nIndex, OldTKey, vecKeys);
				m_KeyEncoder.AddSymbol(m_nCount, nIndex, NewKey, vecKeys);
			}

			virtual void Remove(uint32_t nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vecKeys)
			{
				m_nCount--;
				m_KeyEncoder.RemoveSymbol(m_nCount, nIndex, key, vecKeys);
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
			uint32_t m_nPageSize;

		};
}