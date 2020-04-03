#include "stdafx.h"
#include "BaseNumLenEncoder.h"
#include "../../../utils/MathUtils.h"
#include "../../../../CommonLib/exception/exc_base.h"

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
		m_nTypeFreq = ectByte;
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

		if (m_nTypeFreq != ectUInt32)
		{
			if (m_BitsLensFreq[nBitsLen] > 255)
				m_nTypeFreq = ectUInt16;

			if (m_BitsLensFreq[nBitsLen] > 65535)
				m_nTypeFreq = ectUInt32;
		}
	}

	void CBaseNumLenEncoder::RemoveCalcBitsLen(uint32_t nBitsLen)
	{
		try
		{
			if (nBitsLen > 1)
			{
				if (m_nBitRowSize < (nBitsLen - 1))
					throw CommonLib::CExcBase("m_nBitRowSize < 0");

				m_nBitRowSize -= (nBitsLen - 1);
			}

			if (m_nCount == 0)
				throw CommonLib::CExcBase("m_nCount < 0");

			m_nCount--;

			if(m_BitsLensFreq[nBitsLen] == 0)
				throw CommonLib::CExcBase("m_BitsLensFreq < 0");

			m_BitsLensFreq[nBitsLen] -= 1;

			if (m_BitsLensFreq[nBitsLen] == 65535 || m_BitsLensFreq[nBitsLen] == 255)
			{
				m_nTypeFreq = ectByte;
				for (uint32_t i = 0; i < _nMaxBitsLens; ++i)
				{
					if (m_BitsLensFreq[i] > 65535)
					{
						m_nTypeFreq = ectUInt32;
						break;
					}
					if (m_nTypeFreq != ectUInt32)
					{
						if (m_BitsLensFreq[nBitsLen] > 255)
							m_nTypeFreq = ectUInt16;
					}
				}
			}
			if (!m_BitsLensFreq[nBitsLen])
				m_nDiffsLen--;

		}
		catch (std::exception& exc )
		{
			CommonLib::CExcBase::RegenExcT("BaseNumLen failed to remove bitslen %1", nBitsLen, exc);
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

	uint32_t CBaseNumLenEncoder::GetMaxBitLen() const
	{
		uint32_t nBitLen = 0;
		for (uint32_t i = 0; i < _nMaxBitsLens + 1; ++i)
		{
			if (m_BitsLensFreq[i] != 0)
				nBitLen = i;
		}
		return nBitLen;
	}

	uint32_t CBaseNumLenEncoder::GetEncodeBitsSize() const
	{
		double dRowBitsLen = GetCodeBitSize();
		uint32_t nByteSize = uint32_t((dRowBitsLen + 7) / 8);
		return nByteSize;
	}

	uint32_t CBaseNumLenEncoder::GetHeaderSize() const
	{
		return 1 + (_nMaxBitsLens + 1 + 7) / 8 + GetLenForDiffLen(m_nTypeFreq, m_nDiffsLen);
	}

	uint32_t CBaseNumLenEncoder::GetRowBitsSize() const
	{
		return (m_nBitRowSize + 7) / 8;
	}
	
	uint32_t CBaseNumLenEncoder::EstimateCompressSize() const
	{
		return GetEncodeBitsSize() + GetHeaderSize() + GetRowBitsSize();
	}

	void CBaseNumLenEncoder::WriteHeader(CommonLib::IWriteStream* pStream)
	{
		try
		{
			uint32_t nBitLen = GetMaxBitLen();
			byte_t nFlag = m_nTypeFreq;

			pStream->Write((byte_t)nBitLen);
			pStream->Write(nFlag);

			byte_t lensMask[sizeof(uint64_t) + 1];
			memset(lensMask, 0, sizeof(uint64_t) + 1);

			for (uint32_t i = 0; i < nBitLen + 1; ++i)
			{
				uint32_t nByte = i / 8;
				uint32_t nBit = i - (nByte * 8);
				if (m_BitsLensFreq[i] != 0)
					lensMask[nByte] |= (0x01 << nBit);
			}

			for (uint32_t i = 0; i < (nBitLen) / 8 + 1; ++i)
			{
				pStream->Write((byte_t)lensMask[i]);
			}

			for (uint32_t i = 0; i < nBitLen + 1; ++i)
			{
				if (m_BitsLensFreq[i] == 0)
					continue;

				switch (m_nTypeFreq)
				{
				case ectByte:
					pStream->Write((byte_t)m_BitsLensFreq[i]);
					break;
				case ectUInt16:
					pStream->Write((uint16_t)m_BitsLensFreq[i]);
					break;
				case ectUInt32:
					pStream->Write((uint32_t)m_BitsLensFreq[i]);
					break;
				}

			}
		}
		catch (std::exception &exc)
		{
			CommonLib::CExcBase::RegenExc("BaseNumLen failed to write header", exc);
		}

	}

	void CBaseNumLenEncoder::ReadHeader(CommonLib::IReadStream* pStream)
	{
		try
		{
			Reset();

			uint32_t nBitsLen = (uint32_t)pStream->ReadByte();
			m_nTypeFreq = (eCompressDataType)pStream->ReadByte();


			byte_t lensMask[(_nMaxBitsLens) / 8 + 1];
			memset(lensMask, 0, (_nMaxBitsLens) / 8 + 1);

			for (uint32_t i = 0; i < (nBitsLen) / 8 + 1; ++i)
			{
				lensMask[i] = pStream->ReadByte();
			}

			for (uint32_t i = 0; i < nBitsLen + 1; ++i)
			{

				uint32_t nByte = i / 8;
				uint32_t nBit = i - (nByte * 8);

				if (!(lensMask[nByte] & (0x01 << nBit)))
				{
					m_BitsLensFreq[i] = 0;
					continue;
				}

				m_nDiffsLen += 1;
				switch (m_nTypeFreq)
				{
				case ectByte:
					m_BitsLensFreq[i] = pStream->ReadByte();
					m_nCount += m_BitsLensFreq[i];
					break;
				case ectUInt16:
					m_BitsLensFreq[i] = pStream->Readintu16();
					m_nCount += m_BitsLensFreq[i];
					break;
				case ectUInt32:
					m_BitsLensFreq[i] = pStream->ReadIntu32();
					m_nCount += m_BitsLensFreq[i];
					break;
				}
				if (i > 1)
					m_nBitRowSize += m_BitsLensFreq[i] * (i - 1);
			}

		}
		catch (std::exception &exc)
		{
			CommonLib::CExcBase::RegenExc("BaseNumLen failed to read header", exc);
		}
	}
}