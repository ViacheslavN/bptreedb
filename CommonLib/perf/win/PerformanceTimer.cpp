#include "stdafx.h"
#include "PerformanceTimer.h"

namespace CommonLib
{
//	namespace win
//	{

		CBasePerformanceTimer::CBasePerformanceTimer()
		{
		}

		CBasePerformanceTimer::~CBasePerformanceTimer()
		{

		}

		double CBasePerformanceTimer::MeasureInterval()
		{
			LARGE_INTEGER value;
			QueryPerformanceCounter(&value);

			double intervalInSec = (double(value.QuadPart) - double(m_start)) / double(m_frequency);
			m_start = uint64_t(value.QuadPart);

			return intervalInSec;
		}

		double CBasePerformanceTimer::GetLastedSec()
		{
			return MeasureInterval();
		}

		double CBasePerformanceTimer::GetLastedMSec()
		{
			return MeasureInterval()*1000.0;
		}

		double CBasePerformanceTimer::GetLastedUSec()
		{
			return MeasureInterval() * 1000000.0;
		}

		double CBasePerformanceTimer::GetLastedNSec()
		{
			return MeasureInterval() * 1000000000.0;
		}


		uint64_t  CBasePerformanceTimer::GetCycles()
		{
			LARGE_INTEGER value;
			QueryPerformanceCounter(&value);

			uint64_t intervalInSec  = (value.QuadPart - m_start);
			m_start = uint64_t(value.QuadPart);

			return intervalInSec;

		}



		CPerformanceTimer::CPerformanceTimer()
		{
			LARGE_INTEGER value;
			QueryPerformanceCounter(&value);
			m_start = uint64_t(value.QuadPart);

			QueryPerformanceFrequency(&value);
			m_frequency = uint64_t(value.QuadPart);
		}

		CPerformanceTimer::~CPerformanceTimer()
		{

		}

		CManualPerformanceTimer::CManualPerformanceTimer()
		{
			LARGE_INTEGER value;
			QueryPerformanceFrequency(&value);
			m_frequency = uint64_t(value.QuadPart);
		}


		CManualPerformanceTimer::~CManualPerformanceTimer()
		{

		}

		void CManualPerformanceTimer::Start()
		{
			LARGE_INTEGER value;
			QueryPerformanceCounter(&value);
			m_start = uint64_t(value.QuadPart);

		
		}

//	}
}