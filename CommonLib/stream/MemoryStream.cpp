#include "stdafx.h"
#include "../alloc/alloc.h"

#include "MemoryStream.h"

namespace CommonLib
{
	CWriteMemoryStream::CWriteMemoryStream(std::shared_ptr<IAlloc> ptrAlloc) : TBase(ptrAlloc.get() == nullptr ? IAlloc::CreateSimpleAlloc() : ptrAlloc)
	{
	}

	CWriteMemoryStream::~CWriteMemoryStream()
	{

	}

	std::streamsize CWriteMemoryStream::WriteBytes(const byte_t* buffer, size_t size)
	{
		try
		{
			if (size > 0)
			{
				ResizeWithCapacity(size);
				::memcpy(Buffer() + m_nPos, buffer, size);
				m_nPos += size;
			}

			return size;
		}
		catch (CExcBase& exc)
		{
			exc.AddMsgT(L"Failed write bytes, size: {0}", size);
			throw;
		}
	}

	std::streamsize CWriteMemoryStream::WriteInverse(const byte_t* buffer, size_t size)
	{
		try
		{
			ResizeWithCapacity(size);
			byte_t *pBuffer = Buffer() + m_nPos;
			for (size_t i = 0; i < size; i++)
				pBuffer[size - i - 1] = buffer[i];

			m_nPos += size;
			return size;
		}
		catch (CExcBase& exc)
		{
			exc.AddMsgT(L"Failed write bytes inverse, size: {0}", size);
			throw;
		}
	}

	void  CWriteMemoryStream::Resize(size_t nSize)
	{
		try
		{
			if (Size() >= nSize)
				return;

			if (m_ptrBuffer->IsAttachedBuffer())
				throw CExcBase("Buffer is attached");
		 
			IMemStreamBufferPtr pBuffer = m_ptrBuffer->CreateBuffer();
			pBuffer->Create((uint32_t)nSize);

			if (Buffer())
			{
				memcpy(pBuffer->GetData(), Buffer(), Size());				 
			}

			m_ptrBuffer = pBuffer;

			if (m_nPos > Size())
				m_nPos = Size();
		}
		catch (CExcBase& exc)
		{
			exc.AddMsgT(L"Failed resize buffer, size: {0}", nSize);
			throw;
		}
	
	}
	
	void  CWriteMemoryStream::ResizeWithCapacity(size_t nSize)
	{

		try
		{
			size_t newSize = Size();
			while (m_nPos + nSize > newSize)
				newSize = size_t(newSize * 1.5) + 1;

			if (newSize > Size())
			{
				// only growing is impossible on an attached buffer; writes that fit are fine
				if (m_ptrBuffer->IsAttachedBuffer())
					throw CExcBase("Buffer is attached");

				IMemStreamBufferPtr pBuffer = m_ptrBuffer->CreateBuffer();
				pBuffer->Create((uint32_t)newSize);

				if (Buffer())
				{
					memcpy(pBuffer->GetData(), Buffer(), m_nPos);
				}
				m_ptrBuffer = pBuffer;
			}
		}
		catch (CExcBase& exc)
		{
			exc.AddMsgT("Failed resize with capacity, size {0}", nSize);
			throw;
		}

	}


	///////////////////////////////////////////
	CReadMemoryStream::CReadMemoryStream()
	{}

	CReadMemoryStream::~CReadMemoryStream()
	{}

	std::streamsize  CReadMemoryStream::ReadBytes(byte_t* dst, size_t size)
	{
		if ((this->m_nPos + size) > Size())
			throw CExcBase(L"ReadMemoryStream: out of range pos: {0}, read size: {1}", m_nPos, size);

		::memcpy(dst, Buffer() + this->m_nPos, size);
		this->m_nPos += size;

		return size;
 
	}

	std::streamsize  CReadMemoryStream::ReadInverse(byte_t* buffer, size_t size)
	{
		if ((this->m_nPos + size) > Size())
			throw CExcBase(L"ReadMemoryStream: out of range pos: {0}, read size: {1}", m_nPos, size);

		const byte_t *pData = Buffer() + this->m_nPos;
		for (size_t i = 0; i < size; i++)
			buffer[i] = pData[size - i - 1];

		this->m_nPos += size;

		return size;
	}

	 

}