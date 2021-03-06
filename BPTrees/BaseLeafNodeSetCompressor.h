#pragma once

#include "Compressor/CompressorParams.h"
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"
#include "../../CommonLib/alloc/stl_alloc.h"
#include "EmptyEncoder.h"
#include "AllocsSet.h"

namespace bptreedb
{
	template<class _TKey, class _TKeyEncoder = TEmptyValueEncoder<_TKey> >
			class TBaseLeafNodeSetCompressor
		{
		public:
			typedef _TKey TKey;
			typedef _TKeyEncoder TKeyEncoder;
			typedef CommonLib::STLAllocator<TKey> TKeyAlloc;
			typedef std::vector<TKey, TKeyAlloc> TKeyMemSet;


			TBaseLeafNodeSetCompressor(uint32_t nPageSize, TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr  pParams, ECompressNodeType type) : m_nCount(0),
				m_nPageSize(nPageSize),
				m_KeyEncoder(pAllocsSet, pParams, eLeafKey)
			{}


			virtual ~TBaseLeafNodeSetCompressor() {}

			virtual void Load(TKeyMemSet& vecKeys, CommonLib::IReadStream* pStream, CBPTreeContext *pContext)
			{
				try
				{
					if (pStream == nullptr)
						throw CommonLib::CExcBase("BaseNodeCompressor  read stream is zero");

					CommonLib::CReadMemoryStream KeyStream;
					m_nCount = pStream->ReadIntu32();
					if (!m_nCount)
						return;

					if(m_nCount > m_nPageSize * 8)
						throw CommonLib::CExcBase(L"Wrong size %1", m_nCount);

					vecKeys.reserve(m_nCount);
					uint32_t nKeySize = pStream->ReadIntu32();

					CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
					if (!pMemStream)
						throw CommonLib::CExcBase(L"IStream isn't memstream");
					
					KeyStream.AttachBuffer(pMemStream->Buffer() + pStream->Pos(), nKeySize);
					m_KeyEncoder.Decode(m_nCount, vecKeys, &KeyStream, nKeySize, pContext);
				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseNodeCompressor failed to read leaf node", exc_src);
				}
			}

			virtual uint32_t Write(TKeyMemSet& vecKeys,  CommonLib::IWriteStream* pStream, CBPTreeContext *pContext)
			{
				try
				{
					if (pStream == nullptr)
						throw CommonLib::CExcBase("BaseNodeCompressor  write stream is zero");

					uint32_t nSize = (uint32_t)vecKeys.size();
					if (m_nCount != nSize)
						throw CommonLib::CExcBase("BaseNodeCompressor  wrong size, count: %1, values size: %2", m_nCount, vecKeys.size());

					pStream->Write(nSize);

					if (!nSize)
						return 0;

					uint32_t compSize = (m_nPageSize - HeadSize());

					m_KeyEncoder.BeginEncoding(vecKeys);
					uint32_t nKeySize = 0;

					size_t sizePos = pStream->Pos();
					pStream->Write(nKeySize);

					size_t keyPosStart = pStream->Pos();
					uint32_t keys = m_KeyEncoder.Encode(vecKeys, pStream, compSize, pContext);
					if (keys != 0)
						return keys;

					size_t endPos = pStream->Pos();
					nKeySize = (uint32_t)(endPos - keyPosStart);

					pStream->Seek(sizePos, CommonLib::soFromBegin);
					pStream->Write(nKeySize);
 
					pStream->Seek(endPos, CommonLib::soFromBegin);

					return 0;
				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("[BaseNodeCompressor] failed to write leaf node", exc_src);
					throw;
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

			virtual void Remove(uint32_t nIndex, const TKey& key, const TKeyMemSet& vecKeys)
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
				return  sizeof(uint32_t) + sizeof(uint32_t);
			}

			uint32_t RowSize() const
			{
				return m_KeyEncoder.GetCompressSize();
			}

			uint32_t TupleSize() const
			{
				return  sizeof(TKey);
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

			bool PossibleUnion(TBaseLeafNodeSetCompressor& pCompressor) const
			{
				if ((m_nCount + pCompressor.m_nCount) > m_nPageSize * 8) //max bits for elem
					return false;

				return (RowSize() + pCompressor.RowSize()) < (m_nPageSize - HeadSize());
			}

			bool PossibleAlignment(TBaseLeafNodeSetCompressor& pCompressor) const
			{
				uint32_t nNoCompSize = m_nCount * sizeof(TKey);
				return nNoCompSize < (m_nPageSize - HeadSize());
			}

			bool IsHalfEmpty() const
			{
				uint32_t nNoCompSize = m_nCount * sizeof(TKey);
				return nNoCompSize < (m_nPageSize - HeadSize()) / 2;
			}

			void Clear()
			{
				m_nCount = 0;
				m_KeyEncoder.Clear();
			}

		protected:

			uint32_t m_nCount;
			TKeyEncoder m_KeyEncoder;
			uint32_t m_nPageSize;

		};
}
