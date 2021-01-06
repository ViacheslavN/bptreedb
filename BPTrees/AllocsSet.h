#pragma once
#include "../../CommonLib/CommonLib.h"

namespace bptreedb
{

	enum EAllocTypes
	{
		eStringAlloc,
		eBlobAlloc
		
	};

	class CAllocsSet
	{
	public:
		CAllocsSet(CommonLib::IAllocPtr pAlloc);
		~CAllocsSet();


		void AddAlloc(CommonLib::IAllocPtr pAlloc, uint32_t id);
		CommonLib::IAllocPtr GetAlloc(uint32_t id, bool retCommonIfNull = true);
		CommonLib::IAllocPtr GetCommonAlloc();
	private:

		typedef std::map<uint32_t, CommonLib::IAllocPtr> TMapAlloc;
		TMapAlloc m_allocs;

		CommonLib::IAllocPtr m_pCommonAlloc;
	};

	typedef std::shared_ptr<CAllocsSet> TAllocsSetPtr;
}