#pragma once
#pragma once
#include "../CompressorParams.h"
#include "../../../../CommonLib/CommonLib.h"
#include "../../../../CommonLib/exception/exc_base.h"
#include "../../../../CommonLib/alloc/alloc.h"
#include "../../../../CommonLib/alloc/simpleAlloc.h"
#include "../../../../CommonLib/alloc/stl_alloc.h"
#include "../../StringVal.h"
#include "../../BPTreeContext.h"
#include "../../AllocsSet.h"


namespace bptreedb
{
	class CEmptyStringEncoder
	{
	public:
		

		typedef CommonLib::STLAllocator<StringValue> TAlloc;
		typedef std::vector<StringValue, TAlloc> TValueMemSet;


		CEmptyStringEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParamsBase, ECompressParams type);
		~CEmptyStringEncoder();

		void AddSymbol(uint32_t nSize, int nIndex, const StringValue& value, const TValueMemSet& vecValues);
		void RemoveSymbol(uint32_t nSize, int nIndex, const StringValue& value, const TValueMemSet& vecValues);
		void UpdateSymbol(int nIndex, StringValue& newValue, const StringValue& oldValue, const TValueMemSet& vecValues);
		uint32_t GetCompressSize() const;
		void BeginEncoding(const TValueMemSet& vecValues);
		uint32_t Encode(const TValueMemSet& vecValues, CommonLib::IMemoryWriteStream *pStream, uint32_t maxCompSize, CBPTreeContext *pContext);
		void Decode(uint32_t nCount, TValueMemSet& vecValues, CommonLib::IMemoryReadStream *pStream, uint32_t nCompSize, CBPTreeContext *pContext);
		
		void Clear();
		uint32_t Count() const;
	private:

		uint32_t m_nCount{ 0 };
		uint32_t m_nRowSize{ 0 };

		CommonLib::IAllocPtr m_pAlloc;
	};

}