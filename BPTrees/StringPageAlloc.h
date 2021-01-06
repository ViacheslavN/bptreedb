#pragma once
#include "../../CommonLib/alloc/stl_alloc.h"

namespace bptreedb
{
 


	class CStringPageAlloc : public CommonLib::IAlloc
	{
	public:
		CStringPageAlloc(uint32_t nMaxLen, CommonLib::IAllocPtr pAlloc);
		~CStringPageAlloc();

		virtual void *Alloc(size_t size);
		virtual void Free(void *ptr);

		virtual int64_t GetCurrentMemoryBalance() const;
		virtual uint64_t GetAllocCount() const;
		virtual uint64_t GetFreeCount() const;
		virtual uint64_t GetTotalAlloc() const;
		virtual uint64_t GetTotalFree() const;
		virtual uint64_t GetSizeMax() const;



	private:
		uint32_t m_maxLen;
		CommonLib::IAllocPtr m_pAlloc;

		typedef CommonLib::STLAllocator<byte_t> TAlloc;
		typedef std::vector<byte_t, TAlloc> TVecBuffer;
	};

	typedef std::shared_ptr<CStringPageAlloc> TStringPageAllocPtr;
}