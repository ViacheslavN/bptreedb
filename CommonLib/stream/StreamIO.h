#pragma once

#include "stream.h"
#include "io.h"

namespace CommonLib
{
	class CReadStreamIO : public IReadStream
	{

	public:
		CReadStreamIO(io::TReadPtr ptrRead);
		virtual ~CReadStreamIO();


		virtual std::streamsize ReadBytes(byte_t* buffer, size_t size);
		virtual std::streamsize ReadInverse(byte_t* buffer, size_t size);
 
 
	private:
		io::TReadPtr m_ptrRead;
	};


	class CWriteStreamIO : public IWriteStream
	{

	public:
		CWriteStreamIO(io::TWritePtr ptrWrite);
		virtual ~CWriteStreamIO();		

		virtual std::streamsize WriteBytes(const byte_t* buffer, size_t size);
		virtual std::streamsize WriteInverse(const byte_t* buffer, size_t size);


	private:
		io::TWritePtr m_ptrWrite;
	};
}