#include "pch.h"
#include "ValueMaps.h"

namespace randomGen
{
	CValueMaps::CValueMaps(uint32_t nMaxCount) : m_nMaxCount(nMaxCount)
	{


		uint32_t nStart = 0;
		uint32_t nEnd = 0;
		m_nPageSize = nMaxCount / m_max_pages;

		if (m_nPageSize == 0)
		{
			m_nPageSize = nMaxCount;
			m_vecPages.push_back(TVecPagePtr(new  TVecPage(0, nMaxCount)));
		}
		else
		{
			for (int i = 0; i < m_max_pages; ++i)
			{

				nEnd = nStart + m_nPageSize;
				m_vecPages.push_back(TVecPagePtr(new  TVecPage(nStart, nEnd)));
				nStart += m_nPageSize;
			}
		}
	}

	CValueMaps::~CValueMaps()
	{

	}

	uint32_t CValueMaps::GetFreeValue(uint32_t idx)
	{
		uint32_t numPage = idx / m_nPageSize;
		return 0;
	}
}