#pragma once
#include <math.h>

namespace bptreedb
{
	namespace utils
	{
		static  double Ln_2 = 0.69314718055994529;

		static inline double Log2(double dVal)
		{
			return log(dVal) / Ln_2;
		}

		static inline uint64_t UI64_Add(uint64_t x, uint64_t y, bool carryin, bool* carryout)
		{
			uint64_t tmp = y + carryin;
			uint64_t result = x + tmp;
			*carryout = (tmp < y) || (result < x);
			return result;
		}
	}
}