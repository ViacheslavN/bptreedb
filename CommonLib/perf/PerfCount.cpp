#include "stdafx.h"
#include "PerfCount.h"
#ifdef _WIN32
	#include "win/PerformanceTimer.h"
#endif



namespace CommonLib
{
	CPerfCounter::CPerfCounter(uint32_t nParams)
	{
		m_Operations.resize(nParams);
	}

	CPerfCounter::~CPerfCounter()
	{

	}

	void CPerfCounter::Reset()
	{
		for (size_t i = 0, sz = m_Operations.size(); i < sz; ++i)
		{
			m_Operations[i].Reset();
		}
	}

	void CPerfCounter::StartOperation(uint32_t id)
	{
		if ( id >= m_Operations.size())
			return;
#ifdef _WIN32
		m_Operations[id].m_nStartCycle = CCycleCounter::GetCurrentCycles();
#else
		clock_gettime(CLOCK_MONOTONIC_RAW, &m_Operations[id].m_nStartTime);
#endif
	}

	void CPerfCounter::StopOperation(uint32_t id, uint32_t nData)
	{
		if (id >= m_Operations.size())
			return;
#ifdef _WIN32
		uint64_t nCycles = CCycleCounter::GetCurrentCycles();
		m_Operations[id].m_nCycles += (nCycles - m_Operations[id].m_nStartCycle);
#else
		timespec stopTime;
		clock_gettime(CLOCK_MONOTONIC_RAW, &stopTime);


		m_Operations[id].m_nCycles += 1000000000 * ((uint64_t)stopTime.tv_sec - (uint64_t)m_Operations[id].m_nStartTime.tv_sec) + (stopTime.tv_nsec - m_Operations[id].m_nStartTime.tv_nsec);

		m_Operations[id].m_nStartTime = stopTime;
#endif
		m_Operations[id].m_nData += nData;
		m_Operations[id].m_nCount += 1;
	}


	double CPerfCounter::GetTimeOperation(uint32_t id) const
	{
		if (id >= m_Operations.size())
			return 0.;
#ifdef _WIN32
		return (double)m_Operations[id].m_nCycles / (double)CCycleCounter::GetFrequency();
#else
		return (double)m_Operations[id].m_nCycles / 1000000000;
#endif
	}

	uint64_t CPerfCounter::GetDataOperation(uint32_t id) const
	{
		if (id >= m_Operations.size())
			return 0;

		return m_Operations[id].m_nData;
	}

	uint32_t CPerfCounter::GetCountOperation(uint32_t id) const
	{
		if (id >= m_Operations.size())
			return 0;

		return m_Operations[id].m_nCount;
	}
 

}