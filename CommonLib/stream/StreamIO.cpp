#include "stdafx.h"
#include "StreamIO.h"

namespace CommonLib
{
	CReadStreamIO::CReadStreamIO(io::TReadPtr ptrRead) : m_ptrRead(ptrRead)
	{}

	CReadStreamIO::~CReadStreamIO()
	{

	}

	std::streamsize CReadStreamIO::ReadBytes(byte_t* buffer, size_t size)
	{
		return m_ptrRead->Read(buffer, size);
	}

	std::streamsize CReadStreamIO::ReadInverse(byte_t* buffer, size_t size)
	{
		return m_ptrRead->Read(buffer, size);
	}	 


	CWriteStreamIO::CWriteStreamIO(io::TWritePtr ptrWrite) : m_ptrWrite(ptrWrite)
	{

	}

	CWriteStreamIO::~CWriteStreamIO()
	{

	}

	std::streamsize CWriteStreamIO::WriteBytes(const byte_t* buffer, size_t size)
	{
		return m_ptrWrite->Write(buffer, size);
	}

	std::streamsize CWriteStreamIO::WriteInverse(const byte_t* buffer, size_t size)
	{
		return m_ptrWrite->Write(buffer, size);
	}
	 

}