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
	}
}