#pragma once

#include "../../../commonlib/CommonLib.h"
#include "../../../commonlib/exception/exc_base.h"
#include "../../../commonlib/alloc/alloc.h"
#include "../../../commonlib/alloc/simpleAlloc.h"
#include "../../../commonlib/alloc/stl_alloc.h"
#include "../../../CommonLib/compress/zstd/ZstdCompressionStream.h"
#include "../../../CommonLib/compress/zstd/ZstdDecompressionStream.h"
#include "ZEncoderDiffBase.h"

namespace bptreedb
{


	template <class _TValue>
	class TZstdEncoder : public TZEncoder<_TValue,  CommonLib::CZstdCompressionStream, CommonLib::CZstdDecompressionStream>
	{
	public:

		typedef TZEncoder<_TValue, CommonLib::CZstdCompressionStream, CommonLib::CZstdDecompressionStream> TBase;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TValueMemSet TValueMemSet;
 
		TZstdEncoder(CommonLib::IAllocPtr& pAlloc, TCompressorParamsBasePtr pParams, ECompressParams type) : TBase(pAlloc, pParams, type)
		{}

		~TZstdEncoder()
		{

		}
	};


	template <class _TValue>
	class TZstdEncoderDiff : public ZEncoderDiffBase<_TValue, CommonLib::CZstdCompressionStream, CommonLib::CZstdDecompressionStream>
	{
	public:

		typedef ZEncoderDiffBase<_TValue, CommonLib::CZstdCompressionStream, CommonLib::CZstdDecompressionStream> TBase;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TValueMemSet TValueMemSet;


		TZstdEncoderDiff(CommonLib::IAllocPtr& pAlloc, TCompressorParamsBasePtr pParams, ECompressParams type) : TBase(pAlloc, pParams, type)
		{}

		~TZstdEncoderDiff()
		{

		}
	};



}
