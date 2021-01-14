#include "stdafx.h"
#include "ZLibStringEncoder.h"


namespace bptreedb
{
	CZlibStringComp::CZlibStringComp(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParamsBase, ECompressParams type)
	{

		m_pAlloc = pAllocsSet->GetAlloc(eStringAlloc);

		TCompressorParamsPtr pParams = pParamsBase->GetCompressParams(type);

		if (pParams.get() != nullptr)
		{
			m_compressLevel = pParams->GetIntParam("compressLevel", m_compressLevel);
			m_compressRate = pParams->GetIntParam("compressRate", m_compressRate);
			m_nMaxLen = pParams->GetIntParam("MaxLen", m_nMaxLen);
			m_nDecBufferSize = pParams->GetIntParam("DecBuffer", m_nDecBufferSize);
		}
	}

	CZlibStringComp::~CZlibStringComp()
	{

	}

	void CZlibStringComp::AddSymbol(uint32_t nSize, int nIndex, const StringValue& value, const TValueMemSet& vecValues)
	{
		m_nCount += 1;
		m_nRowSize += value.Length();
	}

	void CZlibStringComp::RemoveSymbol(uint32_t nSize, int nIndex, const StringValue& value, const TValueMemSet& vecValues)
	{
		m_nCount -= 1;
		m_nRowSize -= value.Length();
	}

	void CZlibStringComp::UpdateSymbol(int nIndex, StringValue& newValue, const StringValue& oldValue, const TValueMemSet& vecValues)
	{
		m_nRowSize -= oldValue.Length();
		m_nRowSize -= newValue.Length();
	}

	uint32_t CZlibStringComp::GetCompressSize() const
	{
		return m_nRowSize / m_compressRate;
	}

	void CZlibStringComp::BeginEncoding(const TValueMemSet& vecValues)
	{

	}

	uint32_t CZlibStringComp::Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream, uint32_t maxCompSize, CBPTreeContext *pContext)
	{
		try
		{
			if (m_nCount != vecValues.size())
				throw CommonLib::CExcBase("CZlibStringComp encoder wrong size, count: %1, values size: %2", m_nCount, vecValues.size());

			CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
			if (!pMemStream)
				throw CommonLib::CExcBase(L"IStream isn't memstream");

			uint32_t streamSize = uint32_t(pStream->Size() - pStream->Pos());
			streamSize = streamSize > maxCompSize ? maxCompSize : streamSize;

			CommonLib::CZCompressStream zStream(m_compressLevel);
			zStream.AttachOut((Bytef*)pMemStream->Buffer() + pStream->Pos(), streamSize);

			for (uint32_t i = 0, sz = (uint32_t)vecValues.size(); i < sz; ++i)
			{
				zStream.AttachIn((Bytef*)vecValues[i].CStr(), (uInt)vecValues[i].Length());

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

		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExcT("[CZlibStringComp] failed to encode", exc);
			throw;
		}

		return 0;
	}

	void CZlibStringComp::Decode(uint32_t nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream, uint32_t nCompSize, CBPTreeContext *pContext)
	{
		try
		{
			CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
			if (!pMemStream)
				throw CommonLib::CExcBase(L"IStream isn't memstream");

			CommonLib::CZDecompressStream zStream;
			zStream.AttachIn(pMemStream->Buffer() + pStream->Pos(), nCompSize);

			pContext->ResizeCommonBuf(m_nDecBufferSize);
			byte_t *pCompBuf = pContext->GetCommonBuf();

			uint32_t readCount = 0;
			std::vector<byte_t> stringPart;

			while (zStream.GetAvailIn() != 0)
			{
				zStream.AttachOut(pCompBuf, m_nDecBufferSize);
				zStream.Decompress();

				if (zStream.GetAvailOut() == 0)
				{
					ProcessDecodeBuffer(pCompBuf, m_nDecBufferSize, stringPart, readCount, nCount, vecValues);
				}
			}

			if (zStream.GetAvailOut() != 0)
			{
				ProcessDecodeBuffer(pCompBuf, m_nDecBufferSize - zStream.GetAvailOut(), stringPart, readCount, nCount, vecValues);
			}


			m_nCount = nCount;
			pStream->Seek(nCompSize, CommonLib::soFromCurrent);

		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExcT("[CZlibStringComp] failed to decode", exc);
			throw;
		}
	}

	void CZlibStringComp::ProcessDecodeBuffer(const byte_t* buffer, uint32_t bufSize, std::vector<byte_t>& strPart, uint32_t& readCount, uint32_t nCount, TValueMemSet& vecValues)
	{
		uint32_t beginpos = 0;
		for (uint32_t i = 0; i < bufSize; ++i)
		{
			if (buffer[i] == '\0')
			{

				readCount += 1;
				if (readCount > nCount)
					throw CommonLib::CExcBase("wrong read size");


				size_t len =  i + (uint32_t)strPart.size() + 1  - beginpos;;

				StringValue value((uint32_t)len, m_pAlloc);
	
				byte_t *pStrBuf = value.Str();
				uint32_t copySize = value.Length();

				if (!strPart.empty())
				{
					memcpy(pStrBuf, strPart.data(), strPart.size());
					pStrBuf += (uint32_t)strPart.size();
					copySize -= (uint32_t)strPart.size();
					strPart.clear();
				}

				memcpy(pStrBuf, buffer + beginpos, copySize);

				vecValues.push_back(value);

				beginpos = i + 1;

			}
		}

		if (beginpos < bufSize)
		{
			strPart.resize(bufSize - beginpos);
			memcpy(strPart.data(), buffer + beginpos, strPart.size());
		}
	}

	void CZlibStringComp::Clear()
	{
		m_nCount = 0;
		m_nRowSize = 0;
	}

	uint32_t CZlibStringComp::Count() const
	{
		return m_nCount;
	}
}
