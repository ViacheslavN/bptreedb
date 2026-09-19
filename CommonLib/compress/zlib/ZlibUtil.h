#pragma once

#include "../../stream/io.h"

namespace CommonLib
{

	class CZLibUtil
	{
	public:

		enum ECompressLevel
		{
			NoCompress = 0,
			BestSpeed = 1,
			BestCompression = 9,
			DefaultCompression = -1
		};

		static void CompressData(ECompressLevel level, io::TReadPtr pSrc, io::TWritePtr pDst);
		static void CompressData(ECompressLevel level, const byte_t* pData, size_t size, io::TWritePtr& pDst);

		static void DecompressData(io::TReadPtr pSrc, io::TWritePtr pDst);
	};


}