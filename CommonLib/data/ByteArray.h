#pragma once

#include "../alloc/stl_alloc.h"

namespace CommonLib
{
	namespace Data
	{
		typedef STLAllocator<byte_t> TAlloc;
		typedef std::vector<byte_t, TAlloc> TVecBuffer;

		int Compare(const TVecBuffer& first, const TVecBuffer& second);
		bool Equals(const TVecBuffer& first, const TVecBuffer& second);

		typedef std::shared_ptr<TVecBuffer> TVecBufferPtr;
	
	}

}