#pragma once

namespace bptreedb
{

	template<class _TType>
	struct TDefSign { typedef _TType TSignType; };

	template<> struct TDefSign< byte_t > { typedef char TSignType; };
	template<> struct TDefSign< uint16_t > { typedef int16_t TSignType; };
	template<> struct TDefSign< uint32_t > { typedef int32_t TSignType; };
	template<> struct TDefSign< uint64_t > { typedef int64_t TSignType; };

	enum eCompressDataType
	{
		ectByte = 0,
		ectUInt16 = 1,
		ectUint32 = 2,
		ectUInt64 = 3
	};


	static uint32_t GetSizeTypeValue(eCompressDataType type)
	{
		switch (type)
		{
		case ectByte:
			return 1;
		case ectUInt16:
			return 2;
		case ectUint32:
			return 4;
		case ectUInt64:
			return 8;
		}
		return 0;
	}


	static uint32_t GetLenForDiffLen(eCompressDataType nTypeFreq, uint32_t nDiffsLen)
	{
		switch (nTypeFreq)
		{
		case ectByte:
			return nDiffsLen;
			break;
		case ectUInt16:
			return nDiffsLen * sizeof(uint16_t);
			break;
		case ectUint32:
			return nDiffsLen * sizeof(uint32_t);
			break;
		case ectUInt64:
			return nDiffsLen * sizeof(uint64_t);
			break;
		}
			return nDiffsLen;
	}
}