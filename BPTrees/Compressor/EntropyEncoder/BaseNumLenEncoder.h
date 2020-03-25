#pragma once

#include "../CompressUtils.h"
#include "../../../utils/BitUtils.h"

namespace bptreedb
{
	class CBaseNumLenEncoder
	{
		static const uint32_t _nMaxBitsLens = 64;
		public: 

			CBaseNumLenEncoder();
			~CBaseNumLenEncoder();

			void Reset();
			uint32_t EstimateCompressSize() const;

			template <class TSymbol>
			void AddCalcSymbol(const TSymbol& symbol)
			{
				uint32_t nBitsLen = 0;
				if (symbol < 2)
					nBitsLen = (uint32_t)symbol;
				else
					nBitsLen = utils::log2(symbol);

				AddCalcBitsLen(nBitsLen);

			}


		protected:
			void AddCalcBitsLen(uint32_t nBitsLen);
			double GetCodeBitSize() const;

		protected:

			uint16_t m_nFlags{ 0 };
			uint32_t m_nBitRowSize{ 0 };
			uint32_t m_BitsLensFreq[_nMaxBitsLens + 1];
			uint32_t m_FreqPrev[_nMaxBitsLens + 1 + 1];
			uint32_t m_nCount{ 0 };
			uint32_t m_nDiffsLen{ 0 };
			eCompressDataType m_nTypeFreq{ ectUint32 };
	};


}