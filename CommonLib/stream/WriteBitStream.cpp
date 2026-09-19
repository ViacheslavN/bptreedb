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
					memcpy(pBuffer->GetData(), Buffer(), this->m_nPos);
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
			if (m_nCurrBit > m_nBitBase)
			{
				m_nPos++;
				if (m_nPos == Size())
				{
					if (m_ptrBuffer->IsAttachedBuffer())
						throw CExcBase("Stream is attached");

					uint32_t newSize = uint32_t(Size() * 1.5) + 1;
					IMemStreamBufferPtr pBuffer = m_ptrBuffer->CreateBuffer();
					pBuffer->Create(newSize);
					memset(pBuffer->GetData(), 0, pBuffer->GetSize());
 
					if (Buffer())
					{
						memcpy(pBuffer->GetData(), Buffer(), m_nPos);
					}

					m_ptrBuffer = pBuffer;

				}

				byte_t *pData = Buffer();
				m_nCurrBit = 0;
				pData[m_nPos] = 0;
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
}