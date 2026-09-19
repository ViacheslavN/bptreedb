#pragma once

#include "stream.h"


namespace CommonLib
{
	class IAlloc;

	class CWriteMemoryStream : public TMemoryStreamBase<IMemoryWriteStream>, public IStream
	{
	private:

		CWriteMemoryStream(const CWriteMemoryStream& stream);
		CWriteMemoryStream& operator=(const CWriteMemoryStream& stream);
	public:
		CWriteMemoryStream(std::shared_ptr<IAlloc> pAlloc = std::shared_ptr<IAlloc>());
		~CWriteMemoryStream();

		typedef TMemoryStreamBase<IMemoryWriteStream> TBase;


		virtual std::streamsize WriteBytes(const byte_t* buffer, size_t size);
		virtual std::streamsize WriteInverse(const byte_t* buffer, size_t size);
		virtual void Resize(size_t nSize);
		virtual bool IsEnoughSpace(size_t size) const { return true; }

	private:
		void  ResizeWithCapacity(size_t nSize);
	};


	class CReadMemoryStream : public TMemoryStreamBase<IMemoryReadStream>, public IStream
	{

	private:

		CReadMemoryStream(const CReadMemoryStream& stream);
		CReadMemoryStream& operator=(const CReadMemoryStream& stream);
	public:
		CReadMemoryStream();
		~CReadMemoryStream();

		typedef TMemoryStreamBase<IMemoryReadStream> TBase;


		virtual std::streamsize  ReadBytes(byte_t* dst, size_t size);
		virtual std::streamsize  ReadInverse(byte_t* buffer, size_t size);
		virtual bool IsEnoughSpace(size_t size) const { return true; }
	};



}