#pragma once
namespace CommonLib
{
//	namespace win
//	{

		class CCycleCounter
		{
		public:
			CCycleCounter()
			{
				Start();
			}

			void Start()
			{
				LARGE_INTEGER value;
				QueryPerformanceCounter(&value);
				m_start = uint64_t(value.QuadPart);
			}

			uint64_t  GetCycles()
			{
				LARGE_INTEGER value;
				QueryPerformanceCounter(&value);

				uint64_t intervalInSec = (value.QuadPart - m_start);
				m_start = uint64_t(value.QuadPart);

				return intervalInSec;

			}

			static uint64_t GetCurrentCycles()
			{
				LARGE_INTEGER value;
				QueryPerformanceCounter(&value);
				return uint64_t(value.QuadPart);
			}

			static uint64_t GetFrequency()
			{
				LARGE_INTEGER value;
				QueryPerformanceFrequency(&value);
				return uint64_t(value.QuadPart);
			}


		private:
			uint64_t m_start{ 0 };
		};



		class CBasePerformanceTimer
		{
		public:
			CBasePerformanceTimer();
			~CBasePerformanceTimer();

			double GetLastedSec();
			double GetLastedMSec();
			double GetLastedUSec();
			double GetLastedNSec();

			uint64_t  GetCycles();
		protected:
			double MeasureInterval();

			uint64_t m_frequency{ 0 };
			uint64_t m_start{ 0 };
		};


		class CPerformanceTimer : public CBasePerformanceTimer
		{
			public:
				CPerformanceTimer();
				~CPerformanceTimer();
		};


		class CManualPerformanceTimer : public CBasePerformanceTimer
		{
		public:
			CManualPerformanceTimer();
			~CManualPerformanceTimer();

			void Start();

		};
//	}
}