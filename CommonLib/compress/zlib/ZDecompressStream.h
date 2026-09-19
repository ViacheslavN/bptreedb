#pragma once

#include "ZStream.h"

namespace CommonLib
{
	class CZDecompressStream : public CZStream
	{
	public:
		CZDecompressStream();
		~CZDecompressStream();

		void Decompress();
		int Inflate(int flush);
	};
}