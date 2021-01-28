#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"

namespace bptreedb
{
	namespace utils
	{

		template<class TValue>
		int32_t lower_bound(const TValue* pData, uint32_t nSize, const TValue& key)
		{
			if (nSize == 0)
				return -1;

			int32_t nFirst = 0;
			int32_t nIndex = 0;
			int32_t nStep = 0;
			int32_t nCount = nSize;

			while (nCount > 0)
			{
				nIndex = nFirst;
				nStep = nCount >> 1;
				nIndex += nStep;
				if (pData[nIndex] < key)
				{
					nFirst = ++nIndex;
					nCount -= (nStep + 1);
				}
				else nCount = nStep;
			}
			return nFirst;
		}


		template<class TValue>
		int32_t upper_bound(const TValue* pData, uint32_t nSize, const TValue& key)
		{

			if (nSize == 0)
				return -1;

			int32_t nFirst = 0;
			int32_t nIndex = 0;
			int32_t nStep = 0;
			int32_t nCount = nSize;

			while (nCount > 0)
			{
				nIndex = nFirst;
				nStep = nCount >> 1;
				nIndex += nStep;
				if (!(key < pData[nIndex]))
				{
					nFirst = ++nIndex;
					nCount -= (nStep + 1);
				}
				else nCount = nStep;
			}
			return nFirst;
		}

		template<class TValue >
		int32_t binary_search(const TValue* pData, uint32_t nSize, const TValue& key)
		{
			if (nSize == 0)
				return -1;

			int32_t nIndex = lower_bound(pData, nSize, key);
			if (nIndex != nSize && key == pData[nIndex])
				return nIndex;

			return -1;
		}
	}
}