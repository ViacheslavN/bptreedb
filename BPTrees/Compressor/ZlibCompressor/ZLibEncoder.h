#pragma once
#include "../CompressorParams.h"
#include "../../../../commonlib/CommonLib.h"
#include "../../../../commonlib/exception/exc_base.h"
#include "../../../../commonlib/alloc/alloc.h"
#include "../../../../commonlib/alloc/simpleAlloc.h"
#include "../../../../commonlib/alloc/stl_alloc.h"
#include "../../../../CommonLib/compress/zlib/ZDecompressStream.h"
#include "../../../../CommonLib/compress/zlib/ZCompressStream.h"

namespace bptreedb
{
	template <class _TValue>
	class TZLibEncoder
	{
	public:

		typedef _TValue TValue;
		typedef CommonLib::STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;

		class CZLibCompParams
		{
			public:
				enum ECompressLevel
				{
					NoCompress = 0,
					BestSpeed = 1,
					BestCompression = 9,
					DefaultCompression = -1
				};

				int32_t m_compressLevel{ BestCompression };
				uint32_t m_compressRate{ 3 };


				virtual void Load(CommonLib::IReadStream *pStream)
				{
					m_compressLevel = (ECompressLevel)pStream->Readint16();
					m_compressRate = pStream->ReadIntu32();
				}

				virtual void Save(CommonLib::IWriteStream *pStream)
				{
					pStream->Write(uint16_t(m_compressLevel));
					pStream->Write(m_compressRate);
				}
		};


		typedef std::shared_ptr<CZLibCompParams> TCompressorParamsPtr;


		TZLibEncoder( CommonLib::IAllocPtr& pAlloc, TCompressorParamsPtr pParams) : m_nCount(0), m_pCompParams(pParams) 
		{
			if (m_pCompParams.get() == nullptr)
				m_pCompParams.reset(new CZLibCompParams());
		}

		~TZLibEncoder()
		{

		}

		void  Init(TCompressorParamsPtr pParams)
		{
			if (pParams.get() != nullptr)
				m_pCompParams = pParams;
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
			return (m_nCount * sizeof(TValue)) / m_pCompParams->m_compressRate;
 		}

		void BeginEncoding(const TValueMemSet& vecValues)
		{

		}

		uint32_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream, uint32_t maxCompSize)
		{
			try
			{
				if (m_nCount != vecValues.size())
					throw CommonLib::CExcBase("Empty encoder wrong size, count: %1, values size: %2", m_nCount, vecValues.size());


				CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
				if (!pMemStream)
					throw CommonLib::CExcBase(L"IStream isn't memstream");

				uint32_t streamSize = uint32_t(pStream->Size() - pStream->Pos());
				streamSize = streamSize > maxCompSize ? maxCompSize : streamSize;

				CommonLib::CZCompressStream zStream(m_pCompParams->m_compressLevel);
				zStream.AttachOut((Bytef*)pMemStream->Buffer() + pStream->Pos(), streamSize);

				for (uint32_t i = 0, sz = (uint32_t)vecValues.size(); i < sz; ++i)
				{
					zStream.AttachIn((Bytef*)&vecValues[i], (uInt)sizeof(TValue));
					while (zStream.GetAvailIn() != 0)
					{
						if (zStream.GetAvailOut() == 0)			
							return (uint32_t)(vecValues.size()) / 2;

						zStream.Compress();

					}
				}

				if (zStream.GetAvailOut() == 0)
					return (uint32_t)(vecValues.size()) / 2;

				zStream.FinishCompress();

				if (zStream.GetAvailOut() == 0)
					return (uint32_t)(vecValues.size()) / 2;

				pStream->Seek(streamSize - zStream.GetAvailOut(), CommonLib::soFromCurrent);
				return 0;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("ZLibEncoder failed to encode", exc);
				throw;
			}
		}

		void Decode(uint32_t nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream, uint32_t nCompSize)
		{
			try
			{

				CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
				if (!pMemStream)
					throw CommonLib::CExcBase(L"IStream isn't memstream");

				CommonLib::CZDecompressStream zStream;

				zStream.AttachIn(pMemStream->Buffer() + pStream->Pos(), nCompSize);
				uint32_t readCount = 0;
				TValue value;

				while (zStream.GetAvailIn() != 0)
				{
					zStream.AttachOut((byte_t*)&value, sizeof(TValue));

					zStream.Decompress();

					if (zStream.GetAvailOut() == 0)
					{
						readCount += 1;
						if (readCount > nCount)
							throw CommonLib::CExcBase("Error decompress");

						vecValues.push_back(value);
						zStream.AttachOut((byte_t*)&value, sizeof(TValue));
					}
				}

				if(readCount != nCount)
					throw CommonLib::CExcBase("Error decompress");

				m_nCount = nCount;
				pStream->Seek(nCompSize, CommonLib::soFromCurrent);
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("ZLibEncoder failed to decode", exc);
			}
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
		uint32_t m_nCount{0};
		TCompressorParamsPtr m_pCompParams;
	};
}
