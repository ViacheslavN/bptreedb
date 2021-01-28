#pragma once
#include "pch.h"
#include "BitStream.h"

namespace testencoding
{
	class TestEncoder : public bptreedb::CBaseNumLenEncoder
	{
	public:

		enum Operations
		{
			eEncoding,
			eRangeEncode,
			eRangeDecode,
			eBitsWrite,
			eBitsRead
		};

		TestEncoder(CommonLib::TPrefCounterPtr pPerf);

		void BeginEncoding(CommonLib::IWriteStream *pStream);
		bool FinishEncoding(CommonLib::IWriteStream *pStream);

		void BeginDecoding(CommonLib::IReadStream *pStream);
		void FinishDecoding(CommonLib::IReadStream *pStream);

		template <class TSymbol>
		bool EncodeSymbol(TSymbol symbol)
		{

			uint16_t nBitLen = 0;
			if (symbol < 2)
				nBitLen = (uint16_t)symbol;
			else  nBitLen = bptreedb::utils::log2(symbol) + 1;

			{
				CommonLib::CPrefCounterHolder holder(m_pPerf, eRangeEncode);
				if (!m_encoder.EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount))
					return false;
			}

			if (nBitLen > 1)
			{
				CommonLib::CPrefCounterHolder holder(m_pPerf, eBitsWrite);
				m_bitWStream.WriteBits(symbol, nBitLen - 1);
			}

			return true;
		}


		template <class TSymbol>
		void DecodeSymbol(TSymbol& symbol)
		{
			symbol = 0;
			uint32_t freq = (uint32_t)m_decoder.GetFreq(m_nCount);
			int32_t nBitLen = utils::upper_bound(m_FreqPrev, _nMaxBitsLens + 1, freq);
			if (nBitLen != 0)
				nBitLen--;

			m_decoder.DecodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount);

			if (nBitLen > 1)
			{
				m_bitRStream.ReadBits(symbol, nBitLen - 1);
				symbol |= ((TSymbol)1 << (nBitLen - 1));
			}
			else
				symbol = nBitLen;
		}

	private:
		CommonLib::TPrefCounterPtr m_pPerf;
	//	CommonLib::CFxBitWriteStream m_bitWStream;
	//	CommonLib::CFxBitReadStream m_bitRStream;

		CBitReader m_bitRStream;
		CBitWriter m_bitWStream;

		CommonLib::TRangeEncoder64 m_encoder;
		CommonLib::TRangeDecoder64 m_decoder;
	};
	
}
