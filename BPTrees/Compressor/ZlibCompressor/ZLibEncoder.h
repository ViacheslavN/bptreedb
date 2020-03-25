#pragma once
#include "../CompressorParams.h"
#include "../../../../commonlib/CommonLib.h"
#include "../../../../commonlib/exception/exc_base.h"
#include "../../../../commonlib/alloc/alloc.h"
#include "../../../../commonlib/alloc/simpleAlloc.h"
#include "../../../../commonlib/alloc/stl_alloc.h"
#include "../../../../CommonLib/compress/zlib/ZDecompressStream.h"
#include "../../../../CommonLib/compress/zlib/ZCompressStream.h"
#include "ZEncoderBase.h"
#include "ZEncoderDiffBase.h"

namespace bptreedb
{

	template <class _TValue>
	class TZLibEncoder : public TZEncoder<_TValue, CommonLib::CZCompressStream, CommonLib::CZDecompressStream>
	{
	public:

		typedef TZEncoder<_TValue, CommonLib::CZCompressStream, CommonLib::CZDecompressStream> TBase;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TValueMemSet TValueMemSet;
		typedef typename TBase::TCompParams TCompParams;
		typedef typename TBase::TCompressorParamsPtr TCompressorParamsPtr;


		TZLibEncoder( CommonLib::IAllocPtr& pAlloc, TCompressorParamsPtr pParams) : TBase(pAlloc, pParams)
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
		typedef typename TBase::TCompParams TCompParams;
		typedef typename TBase::TCompressorParamsPtr TCompressorParamsPtr;


		TZLibEncoderDiff(CommonLib::IAllocPtr& pAlloc, TCompressorParamsPtr pParams) : TBase(pAlloc, pParams)
		{}

		~TZLibEncoderDiff()
		{

		}
	};
}
