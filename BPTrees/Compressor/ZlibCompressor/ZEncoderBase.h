#pragma once
#include "../CompressorParams.h"
#include "../../../../CommonLib/CommonLib.h"
#include "../../../../CommonLib/exception/exc_base.h"
#include "../../../../CommonLib/alloc/alloc.h"
#include "../../../../CommonLib/alloc/simpleAlloc.h"
#include "../../../../CommonLib/alloc/stl_alloc.h"

namespace bptreedb
{

	/*class CZCompParams
	{
		public:
			enum ECodeType
			{
				SingleValue = 0,
				UsingContextBuffer = 1,
				UsingValueBuffer = 2
			};

			uint16_t m_compressLevel{ 9 };
			uint16_t m_compressRate{ 5 };
			uint16_t m_codeType{ UsingValueBuffer };


			virtual void Load(CommonLib::IReadStream *pStream)
			{
				m_compressLevel = pStream->Readint16();
				m_compressRate = pStream->Readint16();
				m_codeType = pStream->Readint16();
			}

			virtual void Save(CommonLib::IWriteStream *pStream)
			{
				pStream->Write(m_compressLevel);
				pStream->Write(m_compressRate);
				pStream->Write(m_codeType);
			}
	};
	*/

	template <class _TValue, class _TEncoder, class _TDecoder>
	class TZEncoder
	{
	public:

		typedef _TValue TValue;
		typedef CommonLib::STLAllocator<TValue> TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;
		typedef _TEncoder  TEncoder;
		typedef _TDecoder  TDecoder;

		enum ECodeType
		{
			SingleValue = 0,
			UsingContextBuffer = 1,
			UsingValueBuffer = 2
		};


		TZEncoder(CommonLib::IAllocPtr& pAlloc, TCompressorParamsBasePtr pParamsBase, ECompressParams type) : m_nCount(0),  m_pAlloc(pAlloc)
		{	
			 
			TCompressorParamsPtr pParams = pParamsBase->GetCompressParams(type);

			if (pParams.get() != nullptr)
			{
				m_compressLevel = pParams->GetIntParam("compressLevel", m_compressLevel);
				m_compressRate = pParams->GetIntParam("compressRate", m_compressRate);
				m_codeType = pParams->GetIntParam("codeType ", m_codeType);
			}

		}

		~TZEncoder()
		{

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
			return (m_nCount * sizeof(TValue)) / m_compressRate;
		}

		void BeginEncoding(const TValueMemSet& vecValues)
		{

		}

		uint32_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream, uint32_t maxCompSize, CBPTreeContext *pContext)
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

				TEncoder zStream(m_compressLevel);
				zStream.AttachOut((Bytef*)pMemStream->Buffer() + pStream->Pos(), streamSize);

				uint32_t count = 0;
				switch (m_codeType)
				{

					case SingleValue:
						count = EncodeSingleValue(vecValues,  zStream);
						break;
					case UsingContextBuffer:
						count = EncodeWithContextBuffer(vecValues, zStream, pContext);
						break;
					case UsingValueBuffer:
						count = EncodeWithValueBuffer(vecValues, zStream);
						break;
					default:
						throw CommonLib::CExcBase("Unknown type %1", m_codeType);
						break;
				}
				if (count != 0)
					return count;

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
				CommonLib::CExcBase::RegenExcT("[ZLibEncoder] failed to encode", exc);
				throw;
			}
		}


		uint32_t EncodeSingleValue(const TValueMemSet& vecValues, TEncoder& zStream)
		{
			try
			{
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
							
				return 0;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[ZLibEncoder] failed to encode by single value", exc);
				throw;
			}
		}


		uint32_t EncodeWithContextBuffer(const TValueMemSet& vecValues, TEncoder& zStream, CBPTreeContext *pContext)
		{
			try
			{
				size_t bufSize = (vecValues.size() + 1) * sizeof(TValue);
				pContext->ResizeCommonBuf(bufSize);
				
				CommonLib::CFxMemoryWriteStream stream;
				stream.AttachBuffer(pContext->GetCommonBuf(), bufSize);
				stream.Seek(0, CommonLib::soFromBegin);

				for (uint32_t i = 0, sz = (uint32_t)vecValues.size(); i < sz; ++i)
				{

					if ((stream.Pos() + sizeof(TValue)) < stream.Size())
					{
						stream.Write(vecValues[i]);
						continue;
					}

					zStream.AttachIn((Bytef*)stream.Buffer(), (uInt)stream.Pos());
					while (zStream.GetAvailIn() != 0)
					{
						if (zStream.GetAvailOut() == 0)
							return (uint32_t)(vecValues.size()) / 2;

						zStream.Compress();

					}

					stream.Seek(0, CommonLib::soFromBegin);
				}

				if (stream.Pos() != 0)
				{
					zStream.AttachIn((Bytef*)stream.Buffer(), (uInt)stream.Pos());
					while (zStream.GetAvailIn() != 0)
					{
						if (zStream.GetAvailOut() == 0)
							return (uint32_t)(vecValues.size()) / 2;

						zStream.Compress();

					}
				}

				return 0;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("ZLibEncoder failed to encode with context", exc);
				throw;
			}
		}


		uint32_t EncodeWithValueBuffer(const TValueMemSet& vecValues, TEncoder& zStream)
		{
			try
			{

				zStream.AttachIn((Bytef*)&vecValues[0], vecValues.size() * sizeof(TValue));
				
				while (zStream.GetAvailIn() != 0)
				{
					if (zStream.GetAvailOut() == 0)
						return (uint32_t)(vecValues.size()) / 2;

					zStream.Compress();

				}

				return 0;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("ZLibEncoder failed to encode by value buffer", exc);
				throw;
			}
		}



		void Decode(uint32_t nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream, uint32_t nCompSize, CBPTreeContext *pContext)
		{
			try
			{
				CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
				if (!pMemStream)
					throw CommonLib::CExcBase(L"IStream isn't memstream");

				TDecoder zStream;
				zStream.AttachIn(pMemStream->Buffer() + pStream->Pos(), nCompSize);
			
				switch (m_codeType)
				{

				case SingleValue:
					DecodeBySingleValue(nCount, vecValues, zStream);
					break;
				case UsingContextBuffer:
					DecodeWithContextBuffer(nCount, vecValues, zStream, pContext);
					break;
				case UsingValueBuffer:
					DecodeWithValueBuffer(nCount, vecValues, zStream);
					break;
				default:
					throw CommonLib::CExcBase("Unknown type %1", m_codeType);
					break;
				}
			

				m_nCount = nCount;
				pStream->Seek(nCompSize, CommonLib::soFromCurrent);
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("ZLibEncoder failed to decode", exc);
			}
		}


		void DecodeBySingleValue(uint32_t nCount, TValueMemSet& vecValues, TDecoder& zStream)
		{
			try
			{
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
					}
				}

				if (readCount != nCount)
					throw CommonLib::CExcBase("Error decompress");
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("ZLibEncoder failed to decode", exc);
			}
		}


		void DecodeWithContextBuffer(uint32_t nCount, TValueMemSet& vecValues,  TDecoder& zStream, CBPTreeContext *pContext)
		{
			try
			{

				uint32_t readCount = 0;
				TValue value;


				size_t bufSize = nCount * sizeof(TValue);
				pContext->ResizeCommonBuf(bufSize);

				CommonLib::CReadMemoryStream stream;
				stream.AttachBuffer(pContext->GetCommonBuf(), bufSize);
				stream.Seek(0, CommonLib::soFromBegin);
				zStream.AttachOut((byte_t*)stream.Buffer(), stream.Size());


				while (zStream.GetAvailIn() != 0)
				{
					zStream.Decompress();
				}
				
				zStream.Decompress();
				stream.Seek(0, CommonLib::soFromBegin);
				for (size_t i = 0; i < nCount; ++i)
				{
					TValue value;
					stream.Read(value);
					vecValues.push_back(value);
				}

				readCount += nCount;
				if (readCount > nCount)
					throw CommonLib::CExcBase("Error decompress");

				vecValues.push_back(value);
				zStream.AttachOut((byte_t*)stream.Buffer(), stream.Size());
			
				if (readCount != nCount)
					throw CommonLib::CExcBase("Error decompress");
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("ZLibEncoder failed to decode with context buffer", exc);
			}
		}

		void DecodeWithValueBuffer(uint32_t nCount, TValueMemSet& vecValues, TDecoder& zStream)
		{
			try
			{
				vecValues.resize(nCount);
				zStream.AttachOut((byte_t*)&vecValues[0], nCount*sizeof(TValue));

				while (zStream.GetAvailIn() != 0)
				{
					zStream.Decompress();
				}
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("ZLibEncoder failed to decode with value buffer", exc);
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
		uint32_t m_nCount{ 0 };
		CommonLib::IAllocPtr m_pAlloc;

		int32_t m_compressLevel{ 9 };
		int32_t m_compressRate{ 5 };
		int32_t m_codeType{ UsingValueBuffer };
	};
}
