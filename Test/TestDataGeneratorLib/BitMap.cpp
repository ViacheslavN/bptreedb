#include "pch.h"
#include "BitMap.h"

namespace randomGen
{

	CBitMap::CBitMap(uint32_t nSizeInByte)
	{
		m_bitsBuf.resize(nSizeInByte, 0);
	}

	uint32_t CBitMap::GetBitSize() const
	{
		return m_bitsBuf.size() * 8;
	}


	void  CBitMap::SetBit(uint32_t nBit, bool bBit)
	{
		if (!(nBit < GetBitSize()))
			throw CommonLib::CExcBase("BitMap::SetBit out of range %1", nBit);

		uint32_t nByte = nBit / 8;
		uint32_t nBitInByte = nBit % 8;
		byte_t   nMask = 0x01 << nBitInByte;
		if (bBit)
			m_bitsBuf[nByte] |= nMask;
		else
			m_bitsBuf[nByte] &= (~nMask);
	}

	bool  CBitMap::GetBit(uint32_t nBit) const
	{
		if (!(nBit < GetBitSize()))
			throw CommonLib::CExcBase("BitMap::GetBit out of range %1", nBit);

		uint32_t nByte = nBit / 8;
		uint32_t nBitInByte = nBit % 8;
		return ((m_bitsBuf[nByte] >> nBitInByte) & 0x01) == 1;
	}

}
