#include "stdafx.h"
#include "simpleAlloc.h"
#include "../exception/exc_base.h"

static const size_t  _mem_check_symbol_ = (size_t)-1;


namespace CommonLib
{


std::shared_ptr<IAlloc> IAlloc::CreateSimpleAlloc(bool debug, uint64_t maxblockAllocSize)
{
	return std::shared_ptr<IAlloc>(new CSimpleAlloc(debug, maxblockAllocSize));
}

CSimpleAlloc::CSimpleAlloc(bool debug, uint64_t maxAllocSize) : IAlloc(debug, maxAllocSize)
{}

CSimpleAlloc::~CSimpleAlloc()
{}

void* CSimpleAlloc::Alloc(size_t size)
{
	if (size > m_maxBlockAllocSize)
	{
		throw CExcBase("Failed alloc: size {0}1 more than maximum {1}", size, m_maxBlockAllocSize);
	}

	if (size == 0)
	{
		throw CExcBase("Failed alloc: size is zero");
	}

	size_t addSize = 0;
	size_t offset = 0;

	if (IsDebugMode())
	{
		addSize = 3 * sizeof(size_t);
		offset = 2 *sizeof(size_t);
	}

	uint8_t *data = (uint8_t*) ::malloc(size + addSize);

	if (data == nullptr)
	{
		throw CExcBase("Failed alloc, size: {0}", size);
	}

	if (IsDebugMode())
	{
		memcpy(data, &_mem_check_symbol_, sizeof(size_t));
		memcpy(data + sizeof(size_t), &size, sizeof(size_t));
		
		size_t checkSum = _mem_check_symbol_ - size;
		memcpy(data + 2* sizeof(size_t) + size, &checkSum, sizeof(size_t));

		if (m_sizeMax < size)
			m_sizeMax = size;

		m_totalAlloc += size;
		m_totalBalanceAllocMemory += size;
		m_allocCnt++;

	}
	
	return data + offset;
}

void  CSimpleAlloc::Free(void* ptr)
{
	if(ptr == nullptr)
	{
		throw CExcBase("Failed free alloc, ptr is zero");
	}

	byte_t* pBuf = (byte_t*)ptr;

	if (IsDebugMode())
	{
		size_t memSymbol = 0;
		size_t size = 0;
		size_t checkSum = 0;

		pBuf -= 2 * sizeof(size_t);

		memcpy(&memSymbol, pBuf, sizeof(size_t));
		memcpy(&size, pBuf + sizeof(size_t), sizeof(size_t));

		if(memSymbol != _mem_check_symbol_)
			throw CExcBase("Failed free, error check first symbol");

		if (size == 0 || size > m_maxBlockAllocSize)
			throw CExcBase("Failed free, wrong size {0}", size);

	
		memcpy(&checkSum, pBuf + 2*sizeof(size_t) + size, sizeof(size_t));

		if ((_mem_check_symbol_ - size) != checkSum)
			throw CExcBase("Failed free, wrong checksum, size {0}, checksum {1}", size, checkSum);

		m_totalFree += size;
		m_totalBalanceAllocMemory -= size;
		m_freeCnt++;
	}

	::free(pBuf);
}

int64_t CSimpleAlloc::GetCurrentMemoryBalance() const
{
	return m_totalBalanceAllocMemory;
}

uint64_t CSimpleAlloc::GetAllocCount() const
{
	return m_allocCnt;
}

uint64_t CSimpleAlloc::GetFreeCount() const
{
	return m_freeCnt;
}

uint64_t CSimpleAlloc::GetTotalAlloc() const
{
	return m_totalAlloc;
}

uint64_t CSimpleAlloc::GetTotalFree() const
{
	return m_totalFree;
}

uint64_t CSimpleAlloc::GetSizeMax() const
{
	return m_sizeMax;
}

}
