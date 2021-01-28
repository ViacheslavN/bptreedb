#pragma once


class CBitWriter
{
public:
	CBitWriter();
	~CBitWriter();

	void WriteBits(uint64_t val, uint32_t bit_cnt);

	void AttachBuffer(byte_t* buffer, uint32_t size);

	static uint32_t GetByteForBits(uint32_t nBits);

private:

	void _WriteBits(uint64_t val, uint32_t bit_cnt);

	byte_t* m_buffer{0};
	uint32_t m_size{0};
	uint32_t m_bit_pos{0};
};
 

class CBitReader
{
public:
	CBitReader();
	~CBitReader();

	void ReadBits(uint64_t& val, uint32_t bit_cnt);

	void AttachBuffer(byte_t* buffer, uint32_t size);
	   	 
private:
	void _ReadBits(uint64_t& val, uint32_t bit_cnt);


	byte_t* m_buffer{ 0 };
	uint32_t m_size{ 0 };
	uint32_t m_bit_pos{ 0 };
};
