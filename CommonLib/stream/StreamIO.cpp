#include "stdafx.h"
#include "StreamIO.h"

namespace CommonLib
{
	CReadStreamIO::CReadStreamIO(io::TReadPtr ptrRead) : m_ptrRead(ptrRead), m_nPos(0)
	{}

	size_t CReadStreamIO::Size() const
	{
		return m_nPos;
	}

	size_t CReadStreamIO::Pos() const
	{
		return m_nPos;
	}

	CReadStreamIO::~CReadStreamIO()
	{

	}

	std::streamsize CReadStreamIO::ReadBytes(byte_t* buffer, size_t size)
	{
		std::streamsize n = m_ptrRead->Read(buffer, size);
		if (n > 0)
			m_nPos += (size_t)n;
		return n;
	}

	std::streamsize CReadStreamIO::ReadInverse(byte_t* buffer, size_t size)
	{
		std::streamsize n = ReadBytes(buffer, size);
		std::reverse(buffer, buffer + (n > 0 ? (size_t)n : 0));
		return n;
	}	 


	CWriteStreamIO::CWriteStreamIO(io::TWritePtr ptrWrite) : m_ptrWrite(ptrWrite), m_nPos(0)
	{

	}

	size_t CWriteStreamIO::Size() const
	{
		return m_nPos;
	}

	size_t CWriteStreamIO::Pos() const
	{
		return m_nPos;
	}

	CWriteStreamIO::~CWriteStreamIO()
	{

	}

	std::streamsize CWriteStreamIO::WriteBytes(const byte_t* buffer, size_t size)
	{
		std::streamsize n = m_ptrWrite->Write(buffer, size);
		if (n > 0)
			m_nPos += (size_t)n;
		return n;
	}

	std::streamsize CWriteStreamIO::WriteInverse(const byte_t* buffer, size_t size)
	{
		std::vector<byte_t> reversed(buffer, buffer + size);
		std::reverse(reversed.begin(), reversed.end());
		return WriteBytes(reversed.data(), size);
	}
	 

}