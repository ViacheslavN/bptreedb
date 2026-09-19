#include "stdafx.h"
#include "PerformanceTimer.h"
#include <time.h>

namespace CommonLib
{
//	namespace lin
//	{
		CPerformanceTimer::CPerformanceTimer()
		{
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			
			m_startNS = GetNs(ts);
		}

		CPerformanceTimer::~CPerformanceTimer()
		{

		}

		uint64_t CPerformanceTimer::GetNs(const struct timespec& val)
		{
			return uint64_t(val.tv_sec) * 1000000000 + uint64_t(val.tv_nsec);
		}

		double CPerformanceTimer::MeasureInterval()
		{
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);

			uint64_t ns = GetNs(ts);
			m_startNS = ns;

			return ns;
		}

		double CPerformanceTimer::GetLastedSec()
		{
			return MeasureInterval()/ 1000000000.0;
		}

		double CPerformanceTimer::GetLastedMSec()
		{
			return MeasureInterval()*1000000.0;
		}

		double CPerformanceTimer::GetLastedUSec()
		{
			return MeasureInterval() * 1000.0;
		}

		double CPerformanceTimer::GetLastedNSec()
		{
			return MeasureInterval();
		}
//	}
}