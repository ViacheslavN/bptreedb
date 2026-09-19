#pragma once

#include "ZStream.h"

namespace CommonLib
{
	class CZCompressStream : public CZStream
	{
	public:
		CZCompressStream(int compressionLevel);
		~CZCompressStream();

		void Compress();
		void FinishCompress();
		int Deflate(int flush);

	};
}