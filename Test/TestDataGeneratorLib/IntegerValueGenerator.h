#pragma once

template<class _TValue>
class TInegerGenerator
{
public:
	typedef _TValue TValue;

	TInegerGenerator(uint32_t nMaxCount){}

	static uint32_t GetHeaderSize() { return 0; }
	uint32_t GetValueSize() const { return sizeof(TValue); }

	void WriteHeader(CommonLib::file::TFilePtr pFile){}
	void ReadHeader(CommonLib::file::TFilePtr pFile) {}

	void WriteValue(uint32_t idx, CommonLib::file::TFilePtr  pFile)
	{
		TValue value = (TValue)idx;
		pFile->Write(&value, sizeof(value));
	}

	TValue ReadValue(CommonLib::file::TFilePtr  pFile)
	{
		TValue value;
		pFile->Read(&value, sizeof(value));
		return value;
	}
};