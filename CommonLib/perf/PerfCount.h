#pragma once

namespace CommonLib
{

	class CPerfCounter
	{
	public:

		struct SOperation
		{

			void Reset()
			{
				m_nCycles = 0;
#ifdef _WIN32
				m_nStartCycle = 0;
#else
				m_nStartTime.tv_nsec = 0;
				m_nStartTime.tv_sec = 0;
#endif
				m_nCount = 0;
				m_nData = 0;
			}

			SOperation()
			{
				Reset();
			}

			uint64_t m_nCycles;
#ifdef _WIN32

			uint64_t m_nStartCycle;
#else
			timespec m_nStartTime;
#endif
			uint32_t m_nCount;
			uint64_t m_nData;
		};


		CPerfCounter(uint32_t nParams);
		~CPerfCounter();

		void StartOperation(uint32_t id);
		void StopOperation(uint32_t id, uint32_t nData = 0);
 		void Reset();

		double GetTimeOperation(uint32_t id) const;
		uint64_t GetDataOperation(uint32_t id) const;
		uint32_t GetCountOperation(uint32_t id) const;

	private:
		std::vector<SOperation> m_Operations;
	};

	typedef std::shared_ptr< CPerfCounter> TPrefCounterPtr;


	class CPrefCounterHolder
	{
		public:
			CPrefCounterHolder(TPrefCounterPtr& pCounter, uint32_t id, uint32_t nData = 0) : m_pCounter(pCounter), m_id(id), m_nData(nData)
			{
				if (m_pCounter.get() != nullptr)
					m_pCounter->StartOperation(m_id);
			}

			~CPrefCounterHolder()
			{
				if (m_pCounter.get() != nullptr)
					m_pCounter->StopOperation(m_id, m_nData);
			}

		private:
			TPrefCounterPtr& m_pCounter;
			uint32_t m_id;
			uint32_t m_nData;
	};

}