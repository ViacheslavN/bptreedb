#pragma once
#include "BitMap.h"



namespace randomGen
{
	class CValueMaps
	{
	public:

		CValueMaps(uint32_t nMaxCount);
		~CValueMaps();


		uint32_t GetFreeValue(uint32_t idx);
	 

	private:

		struct TVecPage
		{

			TVecPage(uint32_t beginIdx, uint32_t endIdx) : m_nBeginIdx(beginIdx), m_nEndIdx(endIdx), m_bitMaps(endIdx - beginIdx)
			{
				m_nFreeValuies = (m_nEndIdx - m_nBeginIdx);
			}


			CBitMap m_bitMaps;
			uint32_t m_nBeginIdx;
			uint32_t m_nEndIdx;
			int32_t m_nFreeValuies;

		};
		typedef std::shared_ptr<TVecPage> TVecPagePtr;
		uint32_t m_nMaxCount;
		uint32_t m_nPageSize;

		std::vector<TVecPagePtr> m_vecPages;
		static const int m_max_pages = 10000;
	};
}