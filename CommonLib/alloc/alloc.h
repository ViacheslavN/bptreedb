#pragma once

namespace CommonLib
{

	class IAlloc
	{
	public:

		IAlloc(bool debug = false, uint64_t maxblockAllocSize = uint64_t(-1)) : m_debugMode(debug), m_maxBlockAllocSize(maxblockAllocSize)
		{}
		virtual ~IAlloc(){}

		virtual void *Alloc(size_t size) = 0;
		virtual void Free(void *ptr) = 0;

		virtual bool IsDebugMode() const { return m_debugMode; };
		virtual int64_t GetCurrentMemoryBalance() const = 0;
		virtual uint64_t GetAllocCount() const = 0;
		virtual uint64_t GetFreeCount() const = 0;
		virtual uint64_t GetTotalAlloc() const = 0;
		virtual uint64_t GetTotalFree() const = 0;
		virtual uint64_t GetSizeMax() const = 0;

		static std::shared_ptr<IAlloc> CreateSimpleAlloc(bool debug = false, uint64_t maxblockAllocSize = uint64_t(-1));

	protected:
		bool m_debugMode;
		const uint64_t m_maxBlockAllocSize;
	};

	typedef std::shared_ptr<IAlloc> IAllocPtr;
}

