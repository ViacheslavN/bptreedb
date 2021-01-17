#pragma once
namespace randomGen
{

	class CBitMap
	{
	public:
		CBitMap(uint32_t nSizeInByte);

		uint32_t GetBitSize() const;

		void SetBit(uint32_t nBit, bool bBit);
		bool GetBit(uint32_t nBit) const;

	private:
		std::vector<byte_t> m_bitsBuf;
	};

}