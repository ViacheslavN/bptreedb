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
		 class _TCompressorParams = CompressorParamsBase>
			class TBaseLeafNodeSetCompressor
		{
		public:
			typedef _TKey TKey;
			typedef _TKeyEncoder TKeyEncoder;
			typedef _TCompressorParams TCompressorParams;
			typedef CommonLib::STLAllocator<TKey> TKeyAlloc;
			typedef std::vector<TKey, TKeyAlloc> TKeyMemSet;
			typedef std::shared_ptr<TCompressorParams> TCompressorParamsPtr;


			TBaseLeafNodeSetCompressor(uint32_t nPageSize, CommonLib::IAllocPtr& pAlloc, TCompressorParamsPtr  pParams ) : m_nCount(0),
				m_nPageSize(nPageSize), m_KeyEncoder(nPageSize, pAlloc, pParams)
			{}

			static TCompressorParamsPtr LoadCompressorParams(CommonLib::IReadStream *pStream)
			{
				try
				{
					TCompressorParamsPtr pParam(new TCompressorParams());

					pParam->Load(pStream);

					return pParam;
				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseLeafNodeSetCompressor failed to load compressor params", exc_src);
				}
			}


			void  Init(TCompressorParamsPtr pParams)
			{
				m_KeyEncoder.Init(pParams);
			}

			virtual ~TBaseLeafNodeSetCompressor() {}

			virtual void Load(TKeyMemSet& vecKeys, CommonLib::IReadStream* pStream)
			{
				try
				{
					if (pStream == nullptr)
						throw CommonLib::CExcBase("BaseNodeCompressor  read stream is zero");

					CommonLib::CReadMemoryStream KeyStream;
					m_nCount = pStream->ReadIntu32();
					if (!m_nCount)
						return;

					vecKeys.reserve(m_nCount);
					uint32_t nKeySize = pStream->ReadIntu32();

					CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
					if (!pMemStream)
						throw CommonLib::CExcBase(L"IStream isn't memstream");
					
					KeyStream.AttachBuffer(pMemStream->Buffer() + pStream->Pos(), nKeySize);
					m_KeyEncoder.Decode(m_nCount, vecKeys, &KeyStream);
				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseNodeCompressor failed to read leaf node", exc_src);
				}
			}

			virtual uint32_t Write(TKeyMemSet& vecKeys,  CommonLib::IWriteStream* pStream)
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

					CommonLib::CFxMemoryWriteStream KeyStream;

					m_KeyEncoder.BeginEncoding(vecKeys);
					uint32_t nKeySize = m_KeyEncoder.GetCompressSize();


					pStream->Write(nKeySize);

					CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
					if (!pMemStream)
						throw CommonLib::CExcBase(L"IStream isn't memstream");

					KeyStream.AttachBuffer(pMemStream->Buffer() + pStream->Pos(), nKeySize);
					pStream->Seek(pStream->Pos() + nKeySize, CommonLib::soFromBegin);

					m_KeyEncoder.Encode(vecKeys, &KeyStream);

					return 0;
				}
				catch (std::exception& exc_src)
				{
					CommonLib::CExcBase::RegenExcT("BaseNodeCompressor failed to write leaf node", exc_src);
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

			bool IsHaveUnion(TBaseLeafNodeSetCompressor& pCompressor) const
			{
				if ((m_nCount + pCompressor.m_nCount) > m_nPageSize * 8) //max bits for elem
					return false;

				return (RowSize() + pCompressor.RowSize()) < (m_nPageSize - HeadSize());
			}

			bool IsHaveAlignment(TBaseLeafNodeSetCompressor& pCompressor) const
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
