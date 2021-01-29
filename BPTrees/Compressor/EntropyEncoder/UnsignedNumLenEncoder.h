#pragma once
#include "BaseNumLenEncoder.h"
#include "../CompressorParams.h"
#include "../../../../CommonLib/CommonLib.h"
#include "../../../../CommonLib/exception/exc_base.h"
#include "../../../../CommonLib/alloc/alloc.h"
#include "../../../../CommonLib/alloc/simpleAlloc.h"
#include "../../../../CommonLib/alloc/stl_alloc.h"
#include "../../../../CommonLib/compress/EntropyCoders/RangeCoder.h"
#include "../../../../CommonLib/compress/EntropyCoders/ArithmeticCoder.h"
#include "../../../../CommonLib/stream/FixedBitStream.h"
#include "../../AllocsSet.h"
#include "../../../utils/algorithm.h"
#include "BitStream.h"


namespace bptreedb
{
 
	class CUnsignedNumLenEncoder: public CBaseNumLenEncoder
	{
	public:
		CUnsignedNumLenEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type);
		~CUnsignedNumLenEncoder();

	
		bool BeginEncoding(CommonLib::IWriteStream *pStream);
		bool FinishEncoding(CommonLib::IWriteStream *pStream);

		void BeginDecoding(CommonLib::IReadStream *pStream);
		void FinishDecoding(CommonLib::IReadStream *pStream);

		template <class TSymbol>
		bool EncodeSymbol(TSymbol symbol)
		{

			uint16_t nBitLen = 0;
			if (symbol < 2)
				nBitLen = (uint16_t)symbol;
			else  nBitLen = utils::log2(symbol) + 1;
 
			if (!m_encoder.EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount))
				return false;

			if (nBitLen > 1)
				m_bitWStream.WriteBits(symbol, nBitLen - 1);

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
				symbol = m_bitRStream.ReadBits(nBitLen - 1);
				symbol |= ((TSymbol)1 << (nBitLen  -1));
			}
			else
				symbol = nBitLen;
		}

		uint32_t GetCompressSize() const;

	private:
		//CommonLib::CFxBitWriteStream m_bitWStream;
		//CommonLib::CFxBitReadStream m_bitRStream;

		CBitWriter m_bitWStream;
		CBitReader m_bitRStream;

		CommonLib::TRangeEncoder64 m_encoder;
		CommonLib::TRangeDecoder64 m_decoder;

		//CommonLib::TACEncoder64 m_encoder;
		//CommonLib::TACDecoder64 m_decoder;

		uint32_t m_procError{0};

	};
}