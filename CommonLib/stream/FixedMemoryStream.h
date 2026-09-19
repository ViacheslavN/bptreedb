#pragma once

#include "stream.h"


namespace CommonLib
{
	class IAlloc;

	class CFxMemoryWriteStream : public TMemoryStreamBase<IMemoryWriteStream>
	{

		CFxMemoryWriteStream(const CFxMemoryWriteStream& stream);
		CFxMemoryWriteStream& operator=(const CFxMemoryWriteStream& stream);

	public:
		typedef TMemoryStreamBase<IMemoryWriteStream> TBase;

		CFxMemoryWriteStream();
		~CFxMemoryWriteStream();
			

		virtual std::streamsize WriteBytes(const byte_t* buffer, size_t size);
		virtual std::streamsize WriteInverse(const byte_t* buffer, size_t size);
		virtual bool IsEnoughSpace(size_t size) const;
	};

	typedef std::shared_ptr<CFxMemoryWriteStream> CFxMemoryWriteStreamPtr;
}