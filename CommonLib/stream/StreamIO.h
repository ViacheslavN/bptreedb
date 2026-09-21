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

		// The underlying device has no notion of a total size, so both report
		// the number of bytes read through this stream so far.
		virtual size_t Size() const;
		virtual size_t Pos() const;

		virtual std::streamsize ReadBytes(byte_t* buffer, size_t size);
		virtual std::streamsize ReadInverse(byte_t* buffer, size_t size);
 
 
	private:
		io::TReadPtr m_ptrRead;
		size_t m_nPos;
	};


	class CWriteStreamIO : public IWriteStream
	{

	public:
		CWriteStreamIO(io::TWritePtr ptrWrite);
		virtual ~CWriteStreamIO();		

		// Both report the number of bytes written through this stream so far.
		virtual size_t Size() const;
		virtual size_t Pos() const;

		virtual std::streamsize WriteBytes(const byte_t* buffer, size_t size);
		virtual std::streamsize WriteInverse(const byte_t* buffer, size_t size);


	private:
		io::TWritePtr m_ptrWrite;
		size_t m_nPos;
	};
}