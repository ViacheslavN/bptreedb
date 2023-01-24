#include "pch.h"
#include "TestEncoder.h"



namespace testencoding
{
	TestEncoder::TestEncoder(CommonLib::TPrefCounterPtr pPerf) : m_pPerf(pPerf)
	{
	}


	void TestEncoder::BeginEncoding(CommonLib::IMemoryWriteStream *pStream)
	{
		WriteHeader(pStream);

		uint32_t nBitSize = (m_nBitRowSize + 7) / 8;

		m_bitWStream.AttachBuffer(pStream->Buffer() + pStream->Pos(), nBitSize);
		pStream->Seek(nBitSize, CommonLib::soFromCurrent);

		m_encoder.SetStream(pStream);
	}

	bool TestEncoder::FinishEncoding(CommonLib::IMemoryWriteStream *pStream)
	{
		return m_encoder.EncodeFinish();
	}

	void TestEncoder::BeginDecoding(CommonLib::IMemoryReadStream *pStream)
	{
		ReadHeader(pStream);

		uint32_t nBitSize = (m_nBitRowSize + 7) / 8;

		m_bitRStream.AttachBuffer(pStream->Buffer() + pStream->Pos(), nBitSize);
		pStream->Seek(nBitSize, CommonLib::soFromCurrent);

		m_decoder.SetStream(pStream);
		m_decoder.StartDecode();
	}

	void TestEncoder::FinishDecoding(CommonLib::IMemoryReadStream *pStream)
	{

	}

	uint32_t TestEncoder::GetCompressSize() const
	{
		uint32_t size = CBaseNumLenEncoder::GetCompressSize();
		size += m_encoder.GetAdditionalSize();

		return size;
	}
}