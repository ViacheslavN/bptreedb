#include "stdafx.h"
#include "ByteArray.h"

namespace CommonLib
{
	namespace Data
	{

		int Compare(const TVecBuffer& first, const TVecBuffer& second)
		{
			if (first.empty() && second.empty())
				return 0;

			if (first.size() < second.size())
				return -1;

			if (first.size() > second.size())
				return 1;

			return memcmp(first.data(), second.data(), first.size());
		}

		bool Equals(const TVecBuffer& first, const TVecBuffer& second)
		{
			return Compare(first, second) == 0;
		}
	}
}