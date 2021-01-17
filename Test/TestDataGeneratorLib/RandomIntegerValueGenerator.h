#pragma once

template<class _TValue>
class TRandomInegerGenerator
{
public:
	typedef _TValue TValue;

	TRandomInegerGenerator(uint32_t nMaxCount) {}

	static uint32_t GetHeaderSize() { return 0; }
	uint32_t GetValueSize() const { return sizeof(TValue); }

	void WriteHeader(CommonLib::file::TFilePtr pFile) {}
	void ReadHeader(CommonLib::file::TFilePtr pFile) {}

	void WriteValue(uint32_t idx, CommonLib::file::TFilePtr  pFile)
	{
		//https://preshing.com/20121224/how-to-generate-a-sequence-of-unique-random-integers/
		if (!m_bInit)
		{
			m_bInit = true;
			uint32_t seed = (uint32_t)time(NULL);

			m_index = permuteQPR(permuteQPR(seed) + 0x682f0161);
			m_intermediateOffset = permuteQPR(permuteQPR(seed + 1) + 0x46790905);
		}

		uint32_t rndVal = permuteQPR((permuteQPR(m_index++) + m_intermediateOffset) ^ 0x5bf03635);

		TValue value = (TValue)rndVal;
		pFile->Write((byte_t*)&value, sizeof(value));
	}

	TValue ReadValue(CommonLib::file::TFilePtr  pFile)
	{
		TValue value;
		pFile->Read((byte_t*)&value, sizeof(value));
		return value;
	}
private:
	static const uint32_t prime = 4294967291u;

	static uint32_t permuteQPR(uint32_t x)
	{
		static const unsigned int prime = 4294967291u;
		if (x >= prime)
			return x;  // The 5 integers out of range are mapped to themselves.

		unsigned int residue = ((unsigned long long) x * x) % prime;
		return (x <= prime / 2) ? residue : prime - residue;
	}

	uint32_t m_index{ 0 };
	uint32_t m_intermediateOffset{ 0 };
	bool m_bInit{ false };
};