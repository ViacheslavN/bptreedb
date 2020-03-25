#include "stdafx.h"
#include "BaseNumLenEncoder.h"
#include "../../../utils/MathUtils.h"

namespace bptreedb
{

	CBaseNumLenEncoder::CBaseNumLenEncoder()
	{

	}

	CBaseNumLenEncoder::~CBaseNumLenEncoder()
	{

	}

	void CBaseNumLenEncoder::Reset()
	{
		m_nFlags = 0;
		m_nBitRowSize = 0;
		m_nCount = 0;
		m_nTypeFreq = ectUint32;
		memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
		memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
	}

	void CBaseNumLenEncoder::AddCalcBitsLen(uint32_t nBitsLen)
	{
		if(nBitsLen > 1)
			m_nBitRowSize += (nBitsLen - 1);

		m_nCount += 1;

		if (!m_BitsLensFreq[nBitsLen])
			m_nDiffsLen++;

		m_BitsLensFreq[nBitsLen] += 1;

		if (m_nTypeFreq != ectUint32)
		{
			if (m_BitsLensFreq[nBitsLen] > 255)
				m_nTypeFreq = ectUInt16;

			if (m_BitsLensFreq[nBitsLen] > 65535)
				m_nTypeFreq = ectUint32;
		}
	}

	double CBaseNumLenEncoder::GetCodeBitSize() const
	{
		double dBitRowSize = 0;
		if (m_nDiffsLen > 1)
		{
			for (uint32_t i = 0; i < _nMaxBitsLens + 1; ++i)
			{
				if (m_BitsLensFreq[i] == 0)
					continue;

				double dFreq = m_BitsLensFreq[i];
				double dLog2 = utils::Log2((double)m_nCount / dFreq);
				dBitRowSize += (dFreq* dLog2);

			}
		}

		return dBitRowSize;
	}

	uint32_t CBaseNumLenEncoder::EstimateCompressSize() const
	{
		double dRowBitsLen = GetCodeBitSize();
		uint32_t nByteSize = uint32_t((dRowBitsLen + 7) / 8);

		return nByteSize + 1 + (_nMaxBitsLens + 1 + 7) / 8 + GetLenForDiffLen(m_nTypeFreq, m_nDiffsLen) + (m_nBitRowSize + 7) / 8;
	}
}