#pragma once
#include "../../CommonLib/CommonLib.h"

namespace bptreedb
{
	namespace util
	{
		class CCRC
		{
		public:
			static uint32_t Crc32(const byte_t* buf, uint32_t len);
		};
		
	}
}