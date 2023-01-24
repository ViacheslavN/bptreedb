#include "stdafx.h"
#include "UnsignedNumLenEncoder.h"

namespace bptreedb
{

	CUnsignedNumLenEncoder::CUnsignedNumLenEncoder(TAllocsSetPtr ptrAllocsSet, TCompressorParamsBasePtr ptrParams, ECompressParams type)
	{
		 TCompressorParamsPtr ptrCompParams = ptrParams->GetCompressParams(type);
		 m_estimateErr = ptrCompParams->GetIntParam("error", 1);
	}

	CUnsignedNumLenEncoder::~CUnsignedNumLenEncoder()
	{

	}

	bool CUnsignedNumLenEncoder::BeginEncoding(CommonLib::IMemoryWriteStream *pStream)
	{
		WriteHeader(pStream);

		uint32_t nBitSize = CBitBase::GetByteForBits(m_nBitRowSize);

		m_bitWStream.AttachBuffer(pStream->Buffer() + pStream->Pos());
		if (!pStream->SeekSafe(nBitSize, CommonLib::soFromCurrent))
			return false;

		m_encoder.SetStream(pStream);

		return true;
 
	}

	bool CUnsignedNumLenEncoder::FinishEncoding(CommonLib::IMemoryWriteStream *pStream)
	{

		return m_encoder.EncodeFinish();
	}

	void CUnsignedNumLenEncoder::BeginDecoding(CommonLib::IMemoryReadStream *pStream)
	{
		ReadHeader(pStream);

		uint32_t nBitSize = CBitBase::GetByteForBits(m_nBitRowSize);

		m_bitRStream.AttachBuffer(pStream->Buffer() + pStream->Pos());
		pStream->Seek(nBitSize, CommonLib::soFromCurrent);

		m_decoder.SetStream(pStream);
		m_decoder.StartDecode();
	}

	void CUnsignedNumLenEncoder::FinishDecoding(CommonLib::IMemoryReadStream *pStream)
	{

	}

	uint32_t CUnsignedNumLenEncoder::GetCompressSize() const
	{
		uint32_t size = CBaseNumLenEncoder::GetCompressSize() + m_encoder.GetAdditionalSize();
		return size;
	}
}