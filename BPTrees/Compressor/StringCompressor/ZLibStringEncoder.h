#pragma once
#include "../CompressorParams.h"
#include "../../../../CommonLib/CommonLib.h"
#include "../../../../CommonLib/exception/exc_base.h"
#include "../../../../CommonLib/alloc/alloc.h"
#include "../../../../CommonLib/alloc/simpleAlloc.h"
#include "../../../../CommonLib/alloc/stl_alloc.h"
#include "../../../../CommonLib/compress/zlib/ZDecompressStream.h"
#include "../../../../CommonLib/compress/zlib/ZCompressStream.h"

#include "../../StringVal.h"
#include "../../BPTreeContext.h"
#include "../../AllocsSet.h"

namespace bptreedb
{
	class CZlibStringComp
	{
	public:

		enum ECodeType
		{
			SingleValue = 0,
			UsingContextBuffer = 1,
			UsingValueBuffer = 2
		};

		typedef CommonLib::STLAllocator<StringValue> TAlloc;
		typedef std::vector<StringValue, TAlloc> TValueMemSet;


		CZlibStringComp(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParamsBase, ECompressParams type);
		~CZlibStringComp();

		void AddSymbol(uint32_t nSize, int nIndex, const StringValue& value, const TValueMemSet& vecValues);
		void RemoveSymbol(uint32_t nSize, int nIndex, const StringValue& value, const TValueMemSet& vecValues);
		void UpdateSymbol(int nIndex, StringValue& newValue, const StringValue& oldValue, const TValueMemSet& vecValues);
		uint32_t GetCompressSize() const;
		void BeginEncoding(const TValueMemSet& vecValues);
		uint32_t Encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream, uint32_t maxCompSize, CBPTreeContext *pContext);
		void Decode(uint32_t nCount, TValueMemSet& vecValues, CommonLib::IReadStream *pStream, uint32_t nCompSize, CBPTreeContext *pContext);
		void Clear();
		uint32_t Count() const;
	private:
		void ProcessDecodeBuffer(const byte_t* buffer, uint32_t bufSize, std::vector<byte_t>& strPart, uint32_t& readCount, uint32_t count, TValueMemSet& vecValues);

	private:

		uint32_t m_nCount{ 0 };
		uint32_t m_nRowSize{ 0 };
		uint32_t m_nMaxLen{ 256 };
		uint32_t m_nDecBufferSize{ 1024 };

		CommonLib::IAllocPtr m_pAlloc;

		int32_t m_compressLevel{ 9 };
		int32_t m_compressRate{ 5 };
	};

}