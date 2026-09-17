#pragma once
#include "../CompressorParams.h"
#include "../../../../CommonLib/CommonLib.h"
#include "../../../../CommonLib/exception/exc_base.h"
#include "../../../../CommonLib/alloc/alloc.h"
#include "../../../../CommonLib/alloc/simpleAlloc.h"
#include "../../../../CommonLib/alloc/stl_alloc.h"
#include "../../../../CommonLib/compress/zlib/ZDecompressStream.h"
#include "../../../../CommonLib/compress/zlib/ZCompressStream.h"
#include "ZEncoderBase.h"
#include "ZEncoderDiffBase.h"
#include "../../AllocsSet.h"

namespace bptreedb
{

	template <class _TValue>
	class TZLibEncoder : public TZEncoder<_TValue, CommonLib::CZCompressStream, CommonLib::CZDecompressStream>
	{
	public:

		typedef TZEncoder<_TValue, CommonLib::CZCompressStream, CommonLib::CZDecompressStream> TBase;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TValueMemSet TValueMemSet;



		TZLibEncoder(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type) : TBase(pAllocsSet, pParams, type)
		{}

		~TZLibEncoder()
		{

		}
	};


	template <class _TValue>
	class TZLibEncoderDiff : public ZEncoderDiffBase<_TValue, CommonLib::CZCompressStream, CommonLib::CZDecompressStream>
	{
	public:

		typedef ZEncoderDiffBase<_TValue, CommonLib::CZCompressStream, CommonLib::CZDecompressStream> TBase;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TValueMemSet TValueMemSet;
 

		TZLibEncoderDiff(TAllocsSetPtr pAllocsSet, TCompressorParamsBasePtr pParams, ECompressParams type) : TBase(pAllocsSet, pParams, type)
		{}

		~TZLibEncoderDiff()
		{

		}
	};
}
