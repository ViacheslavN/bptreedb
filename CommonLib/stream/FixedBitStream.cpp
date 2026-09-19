#include "stdafx.h"
#include "FixedBitStream.h"
#include "../exception/exc_base.h"
namespace CommonLib
{


	CFxBitWriteStream::CFxBitWriteStream() : TBase(IAllocPtr())
	{}

	CFxBitWriteStream::~CFxBitWriteStream()
	{}

	void CFxBitWriteStream::WriteBit(bool bBit)
	{
	
		byte_t* pBuffer = m_ptrBuffer->GetData();
		if (m_nCurrBit > m_nBitBase)
		{
			if (m_nPos == Size())
				throw CExcBase("FxBitWriteStream write bit out of range ");

			m_nPos++;
			m_nCurrBit = 0;
			pBuffer[m_nPos] = 0;
		}

		if (bBit)
			pBuffer[m_nPos] |= (1 << m_nCurrBit);

		m_nCurrBit++;

	}


	bool CFxBitWriteStream::WriteBitSafe(bool bBit)
	{
		byte_t* pBuffer = m_ptrBuffer->GetData();
		if (m_nCurrBit > m_nBitBase)
		{
			if (m_nPos == Size())
				return false;

			m_nPos++;
			m_nCurrBit = 0;
			pBuffer[m_nPos] = 0;
		}

		if (bBit)
			pBuffer[m_nPos] |= (1 << m_nCurrBit);

		m_nCurrBit++;

		return true;
	}	
	
	CFxBitReadStream::CFxBitReadStream( ) : TBase(IAllocPtr())
	{

	}

	CFxBitReadStream::~CFxBitReadStream() {}

	bool CFxBitReadStream::ReadBit()
	{
		byte_t* pBuffer = m_ptrBuffer->GetData();

		bool bBit = pBuffer[m_nPos] & (1 << m_nCurrBit) ? true : false;
		m_nCurrBit++;

		if (m_nCurrBit > m_nBitBase)
		{
			if (m_nPos == Size())
				throw CExcBase("FxBitReadStream read bit out of range ");

			m_nPos++;
			m_nCurrBit = 0;
		}

		return bBit;
	}

}