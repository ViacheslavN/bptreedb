#include "pch.h"
#include "TestEncoder.h"



namespace testencoding
{
	TestEncoder::TestEncoder(CommonLib::TPrefCounterPtr pPerf) : m_pPerf(pPerf)
	{}


	void TestEncoder::BeginEncoding(CommonLib::IWriteStream *pStream)
	{
		WriteHeader(pStream);

		uint32_t nBitSize = (m_nBitRowSize + 7) / 8;
		CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
		if (!pMemStream)
			throw CommonLib::CExcBase(L"IStream isn't memstream");

		m_bitWStream.AttachBuffer(pMemStream->Buffer() + pStream->Pos(), nBitSize);
		pStream->Seek(nBitSize, CommonLib::soFromCurrent);

		m_encoder.SetStream(pStream);
	}

	bool TestEncoder::FinishEncoding(CommonLib::IWriteStream *pStream)
	{
		return m_encoder.EncodeFinish();
	}

	void TestEncoder::BeginDecoding(CommonLib::IReadStream *pStream)
	{
		ReadHeader(pStream);

		uint32_t nBitSize = (m_nBitRowSize + 7) / 8;
		CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
		if (!pMemStream)
			throw CommonLib::CExcBase(L"IStream isn't memstream");

		m_bitRStream.AttachBuffer(pMemStream->Buffer() + pStream->Pos(), nBitSize);
		pStream->Seek(nBitSize, CommonLib::soFromCurrent);

		m_decoder.SetStream(pStream);
		m_decoder.StartDecode();
	}

	void TestEncoder::FinishDecoding(CommonLib::IReadStream *pStream)
	{

	}
}