#pragma once

namespace CommonLib
{
	namespace synch
	{
		template <class TItem>
		class TSyncQueue
		{
		public:
			TSyncQueue(TItem nullItem = TItem()) : m_bAbort(false), m_nullItem(nullItem)
			{

			}

			void Push(const TItem& ptrItem)
			{
				std::lock_guard<std::recursive_mutex> locker(m_mutex);
				m_queue.push(ptrItem);
				m_condChanged.notify_all();

			}

			void Push(TItem&& ptrItem)
			{
				std::lock_guard<std::recursive_mutex> locker(m_mutex);
				m_queue.push(ptrItem);
				m_condChanged.notify_all();

			}

			TItem Pop()
			{
				std::unique_lock<std::recursive_mutex> locker(m_mutex);
				while (m_queue.empty() && !m_bAbort)
					m_condChanged.wait(locker);

				if (m_queue.empty() || m_bAbort)
					return m_nullItem;

				TItem item = m_queue.front();
				m_queue.pop();

				m_condChanged.notify_all();

				return item;
			}

			size_t Count() const
			{
				std::lock_guard<std::recursive_mutex> locker(m_mutex);
				return m_queue.size();
			}

			void WaitForAtLeast(size_t size) const
			{
				std::unique_lock<std::recursive_mutex> locker(m_mutex);
				while (m_queue.size() < size && !m_bAbort)
					m_condChanged.wait(locker);
			}

			void WaitForAtMost(size_t size) const
			{
				std::lock_guard<std::recursive_mutex> locker(m_mutex);
				while (m_queue.size() > size && !m_bAbort)
					m_condChanged.wait(locker);
			}

			void AbortQueue()
			{
				std::lock_guard<std::recursive_mutex> locker(m_mutex);
				m_bAbort = true;

				m_condChanged.notify_all();
			}

			bool IsAborted()
			{
				std::lock_guard<std::recursive_mutex> locker(m_mutex);
				return m_bAbort;
			}

		private:
			std::queue<TItem>               m_queue;
			bool                            m_bAbort;
			mutable std::recursive_mutex	m_mutex;
			mutable std::condition_variable_any m_condChanged;
			TItem							m_nullItem;
		};

	}
}