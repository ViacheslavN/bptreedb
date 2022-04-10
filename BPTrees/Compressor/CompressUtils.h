#pragma once

#include "../../../CommonLib/stream/stream.h"

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
		ectUInt32 = 2,
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
		case ectUInt32:
			return 4;
		case ectUInt64:
			return 8;
		}
		return 0;
	}

	template<class TValue>
	eCompressDataType GetCompressType(TValue nValue)
	{
		if (nValue < 0xFF + 1)
			return ectByte;
		else if (nValue - 1 < 0xFFFF)
			return ectUInt16;
		else if (nValue - 1 < 0xFFFFFFFF)
			return ectUInt32;

		return ectUInt64;
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
		case ectUInt32:
			return nDiffsLen * sizeof(uint32_t);
			break;
		case ectUInt64:
			return nDiffsLen * sizeof(uint64_t);
			break;
		}

		return nDiffsLen;
	}

	template<class TValue>
	void WriteCompressValue(eCompressDataType nType, TValue value, CommonLib::IWriteStream* pStream)
	{
		switch (nType)
		{
		case ectByte:
			pStream->Write((byte_t)value);
			break;
		case ectUInt16:
			pStream->Write((uint16_t)value);
			break;
		case ectUInt32:
			pStream->Write((uint32_t)value);
			break;
		case ectUInt64:
			pStream->Write((uint64_t)value);
			break;
		}
	}

	template<class TValue>
	void ReadCompressValue(eCompressDataType nType, TValue& value, CommonLib::IReadStream* pStream)
	{
		switch (nType)
		{
		case ectByte:
			value = (TValue)pStream->ReadByte();
			break;
		case ectUInt16:
			value = (TValue)pStream->Readint16();
			break;
		case ectUInt32:
			value = (TValue)pStream->ReadIntu32();
			break;
		case ectUInt64:
			value = (TValue)pStream->ReadIntu64();
			break;
		}
	}


	template<class TValue>
	TValue ReadCompressValue(eCompressDataType nType, CommonLib::IReadStream* pStream)
	{
		switch (nType)
		{
		case ectByte:
			return (TValue)pStream->ReadByte();
			break;
		case ectUInt16:
			return (TValue)pStream->Readintu16();
			break;
		case ectUInt32:
			return (TValue)pStream->ReadIntu32();
			break;
		case ectUInt64:
			return (TValue)pStream->ReadIntu64();
			break;
		}

		return 0;
	}

}