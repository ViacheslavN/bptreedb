#pragma once
#include "BaseNumLenEncoder.h"
#include "../CompressorParams.h"
#include "../../../../CommonLib/CommonLib.h"
#include "../../../../CommonLib/exception/exc_base.h"
#include "../../../../CommonLib/alloc/alloc.h"
#include "../../../../CommonLib/alloc/simpleAlloc.h"
#include "../../../../CommonLib/alloc/stl_alloc.h"
#include "../../../../CommonLib/compress/EntropyCoders/RangeCoder.h"

namespace bptreedb
{
	class CUnsignedNumLenEncoderAC : public CBaseNumLenEncoder
	{
	public:
		CUnsignedNumLenEncoderAC(CommonLib::IAllocPtr& pAlloc, TCompressorParamsBasePtr pParams, ECompressParams type);
		~CUnsignedNumLenEncoderAC();

	private:
 
	};
}