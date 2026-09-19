#pragma once
#pragma once
namespace CommonLib
{
	//namespace lin
	//{CLOCK_MONOTONIC_RAW
		class CPerformanceTimer
		{
		public:
			CPerformanceTimer();
			~CPerformanceTimer();

			double GetLastedSec();
			double GetLastedMSec();
			double GetLastedUSec();
			double GetLastedNSec();
		private:
			double MeasureInterval();
			uint64_t GetNs(const struct timespec& val);
		private:
			uint64_t m_startNS{ 0 };
		};
	//}
}