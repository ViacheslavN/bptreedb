#pragma once

#include "alloc.h"


namespace CommonLib
{

class CSimpleAlloc : public IAlloc
{
public:
	CSimpleAlloc(bool debug = false, uint64_t maxAllocSize = uint64_t(-1));
	virtual ~CSimpleAlloc();

	virtual void* Alloc(size_t size);
	virtual void  Free(void* ptr);


	virtual int64_t GetCurrentMemoryBalance() const;
	virtual uint64_t GetAllocCount() const;
	virtual uint64_t GetFreeCount() const;
	virtual uint64_t GetTotalAlloc() const;
	virtual uint64_t GetTotalFree() const;
	virtual uint64_t GetSizeMax() const;

private:
	int64_t m_totalBalanceAllocMemory { 0 };
	uint64_t m_allocCnt{ 0 };
	uint64_t m_freeCnt{ 0 };
	uint64_t m_totalAlloc{ 0 };
	uint64_t m_totalFree{ 0 };
	uint64_t m_sizeMax{ 0 };

};


}
