#include "stdafx.h"
#include "BitStream.h"
#include "WriteBitStream.h"

namespace CommonLib
{
	WriteBitStream::WriteBitStream(IAllocPtr pAlloc) : TBase(pAlloc)
	{
	}
	WriteBitStream::~WriteBitStream()
	{

	}

	void WriteBitStream::Create(size_t nSize)
	{
		try
		{
			TBase::Create(nSize);
			// bits are OR-ed into the buffer, so it has to start out zeroed
			memset(Buffer(), 0, Size());
			m_nCurrBit = 0;
		}
		catch (std::exception& exc)
		{
			CExcBase::RegenExc("WriteBitStream Create", exc);
		}
	}

	void  WriteBitStream::Resize(uint32_t nSize)
	{
		try
		{
			if(m_ptrBuffer->IsAttachedBuffer())
				throw CExcBase("Stream is attached");

			uint32_t newSize = (uint32_t)Size();

			while (m_nPos + nSize > newSize)
				newSize = uint32_t(newSize * 1.5) + 1;

			if (newSize > Size())
			{		 

				IMemStreamBufferPtr pBuffer = m_ptrBuffer->CreateBuffer();
				pBuffer->Create(newSize);
				memset(pBuffer->GetData(), 0, pBuffer->GetSize());					

				if (Buffer())
				{
					// keep the completed bytes AND the byte currently being filled
					memcpy(pBuffer->GetData(), Buffer(), std::min<size_t>(Size(), this->m_nPos + 1));
				}

				m_ptrBuffer = pBuffer;
			}
		}
		catch (std::exception& exc)
		{
			CExcBase::RegenExc("WriteBitStream Resize", exc);
		}
 
	}

	void WriteBitStream::WriteBit(bool bBit)
	{
		try
		{
			if (Size() == 0)
				Resize(1); // nothing allocated yet: start with a zeroed byte

			if (m_nPos >= Size())
				throw CExcBase("WriteBitStream: position is out of range");

			if (m_nCurrBit > m_nBitBase)
			{
				// the current byte is full: make sure the next one exists BEFORE
				// advancing, so a failed write leaves the stream state untouched
				if (m_nPos + 1 >= Size())
				{
					if (m_ptrBuffer->IsAttachedBuffer())
						throw CExcBase("Stream is attached");

					uint32_t newSize = uint32_t(Size() * 1.5) + 1;
					IMemStreamBufferPtr pBuffer = m_ptrBuffer->CreateBuffer();
					pBuffer->Create(newSize);
					memset(pBuffer->GetData(), 0, pBuffer->GetSize());
 
					if (Buffer())
					{
						memcpy(pBuffer->GetData(), Buffer(), m_nPos + 1);
					}

					m_ptrBuffer = pBuffer;
				}

				m_nPos++;
				m_nCurrBit = 0;
				Buffer()[m_nPos] = 0;
			}

			byte_t *pData = Buffer();
			if (bBit)
				pData[m_nPos] |= (1 << m_nCurrBit);

			m_nCurrBit++;
		}
		catch (std::exception& exc)
		{
			CExcBase::RegenExc("WriteBitStream WriteBit", exc);
		}
	}

	bool WriteBitStream::WriteBitSafe(bool bBit)
	{
		try
		{
			WriteBit(bBit);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}