#pragma once
 
#include "../../../../CommonLib/CommonLib.h"
#include "../../../../CommonLib/exception/exc_base.h"
#include "../../../../CommonLib/alloc/alloc.h"
#include "../../../../CommonLib/alloc/simpleAlloc.h"
#include "../../../../CommonLib/alloc/stl_alloc.h"
#include "../../../../CommonLib/compress/zstd/ZstdCompressionStream.h"
#include "../../../../CommonLib/compress/zstd/ZstdDecompressionStream.h"
#include "ZEncoderDiffBase.h"
#include "../../AllocsSet.h"


namespace bptreedb
{


	template <class _TValue>
	class TZstdEncoder : public TZEncoder<_TValue,  CommonLib::CZstdCompressionStream, CommonLib::CZstdDecompressionStream>
	{
	public:

		typedef TZEncoder<_TValue, CommonLib::CZstdCompressionStream, CommonLib::CZstdDecompressionStream> TBase;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TValueMemSet TValueMemSet;
 
		TZstdEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type) : TBase(pAllocsSet, pParams, type)
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


		TZstdEncoderDiff(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type) : TBase(pAllocsSet, pParams, type)
		{}

		~TZstdEncoderDiff()
		{

		}
	};



}
