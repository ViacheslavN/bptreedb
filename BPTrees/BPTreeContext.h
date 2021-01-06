#pragma once

#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/stl_alloc.h"

namespace bptreedb
{
	class CBPTreeContext
	{
	public:
		CBPTreeContext(CommonLib::IAllocPtr pAlloc);
		~CBPTreeContext();

		size_t GetCommonBufSize() const;
		byte_t* GetCommonBuf();
		void ResizeCommonBuf(size_t size);

	private:
		CommonLib::IAllocPtr m_pAlloc;
		typedef CommonLib::STLAllocator<byte_t> TAlloc;
		typedef std::vector<byte_t, TAlloc> TVecBuffer;
		TVecBuffer m_commonBuf;
	};

}