#pragma once

#include "../CompressUtils.h"
#include "../../../utils/BitUtils.h"

namespace bptreedb
{
	class CBaseNumLenEncoder
	{

		public: 
			static const uint32_t _nMaxBitsLens = 64;

			CBaseNumLenEncoder();
			~CBaseNumLenEncoder();

			void Clear();
 
			template <class TSymbol>
			void AddSymbol(const TSymbol& symbol)
			{
				try
				{
					uint32_t nBitsLen = 0;
					if (symbol < 2)
						nBitsLen = (uint32_t)symbol;
					else
					{
						nBitsLen = utils::log2(symbol);
						m_nBitRowSize += (nBitsLen);

						nBitsLen += 1; // for 0,1
					}
					m_nCount++;

					AddCalcBitsLen(nBitsLen);
				}
				catch (std::exception& exc)
				{
					CommonLib::CExcBase::RegenExcT("BaseNumLen failed to add bitslen ", exc);
				}
			}


			template <class TSymbol>
			void RemoveSymbol(const TSymbol& symbol)
			{
				try
				{
					uint32_t nBitsLen = 0;
					if (symbol < 2)
						nBitsLen = (uint32_t)symbol;
					else
					{
						nBitsLen = utils::log2(symbol);

						if (m_nBitRowSize < (nBitsLen))
							throw CommonLib::CExcBase("m_nBitRowSize < 0");

						m_nBitRowSize -= (nBitsLen);
						nBitsLen += 1;// for 0,1
					}

					RemoveCalcBitsLen(nBitsLen);
				}
				catch (std::exception& exc)
				{
					CommonLib::CExcBase::RegenExcT("BaseNumLen failed to remove bitslen", exc);
				}
			}

			uint32_t GetCompressSize() const;

		protected:
			void AddCalcBitsLen(uint32_t nBitsLen);
			void RemoveCalcBitsLen(uint32_t nBitsLen);

			double GetCodeBitSize() const;

			uint32_t GetEncodeBitsSize() const;
			uint32_t GetHeaderSize() const;
			uint32_t GetRowBitsSize() const;

			uint32_t GetMaxBitLen() const;
			void WriteHeader(CommonLib::IWriteStream* pStream);
			void ReadHeader(CommonLib::IReadStream* pStream);			

		protected:

			uint16_t m_nFlags{ 0 };
			uint32_t m_nBitRowSize{ 0 };
			uint32_t m_BitsLensFreq[_nMaxBitsLens + 1];
			uint32_t m_FreqPrev[_nMaxBitsLens + 1 + 1];
			uint32_t m_nCount{ 0 };
			uint32_t m_nDiffsLen{ 0 };
			eCompressDataType m_nTypeFreq{ ectUInt32 };

			bool m_bOnlineRec{ false };
			double m_dBitRowSize{ 0.0 };

			uint32_t m_estimateErr{ 1 };
	};


}