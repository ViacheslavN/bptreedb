#include "stdafx.h"
#include "UnsignedNumLenEncoder.h"

namespace bptreedb
{

	CUnsignedNumLenEncoder::CUnsignedNumLenEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type)
	{

		 TCompressorParamsPtr pCompParams = pParams->GetCompressParams(type);
		 m_procError = pCompParams->GetIntParam("error", 1);
	}

	CUnsignedNumLenEncoder::~CUnsignedNumLenEncoder()
	{

	}

	void CUnsignedNumLenEncoder::BeginEncoding(CommonLib::IWriteStream *pStream)
	{
		WriteHeader(pStream);

		uint32_t nBitSize = CBitBase::GetByteForBits(m_nBitRowSize);
		CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
		if (!pMemStream)
			throw CommonLib::CExcBase(L"IStream isn't memstream");

		m_bitWStream.AttachBuffer(pMemStream->Buffer() + pStream->Pos(), nBitSize);
		pStream->Seek(nBitSize, CommonLib::soFromCurrent);

		m_encoder.SetStream(pStream);
 
	}

	bool CUnsignedNumLenEncoder::FinishEncoding(CommonLib::IWriteStream *pStream)
	{

		return m_encoder.EncodeFinish();
	}

	void CUnsignedNumLenEncoder::BeginDecoding(CommonLib::IReadStream *pStream)
	{
		ReadHeader(pStream);

		uint32_t nBitSize = CBitBase::GetByteForBits(m_nBitRowSize);
		CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
		if (!pMemStream)
			throw CommonLib::CExcBase(L"IStream isn't memstream");

		m_bitRStream.AttachBuffer(pMemStream->Buffer() + pStream->Pos(), nBitSize);
		pStream->Seek(nBitSize, CommonLib::soFromCurrent);

		m_decoder.SetStream(pStream);
		m_decoder.StartDecode();
	}

	void CUnsignedNumLenEncoder::FinishDecoding(CommonLib::IReadStream *pStream)
	{

	}

	uint32_t CUnsignedNumLenEncoder::GetCompressSize() const
	{
		uint32_t size = CBaseNumLenEncoder::GetCompressSize();
		uint32_t err = (size * m_procError) / 100;

		return size + err;
	}
}