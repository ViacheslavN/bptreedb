#include "pch.h"
#include "BitStream.h"

CBitWriter::CBitWriter()
{

}



uint32_t CBitWriter::GetByteForBits(uint32_t nBits)
{
	uint32_t nBitSize = (nBits + 7) / 8;

	return nBitSize + sizeof(uint64_t);
}

void CBitWriter::AttachBuffer(byte_t* buffer, uint32_t size)
{
	m_buffer = buffer;
	m_size = size;
	m_bit_pos = 0;
}

void WriteBits(uint64_t val, uint32_t counts)
{

}


CBitWriter::~CBitWriter()
{

}


static uint64_t BIT_MASKS[] = {
	0x0,  //0
	0x1, //1
	0x3, //2
	0x7, //3
	0xF,//4
	0x1F,//5
	0x3F, //6
	0x7F, //7
	0xFF,//8
	0x1FF,//9 
	0x3FF,//10 
	0x7FF,//11
	0xFFF,//12
	0x1FFF,//13
	0x3FFF,//14
	0x7FFF,//15
	0xFFFF,//16 
	0x1FFFF,//17
	0x3FFFF,//18
	0x7FFFF,//19
	0xFFFFF,//20
	0x1FFFFF,//21 
	0x3FFFFF,//22
	0x7FFFFF,//23
	0xFFFFFF,//24
	0x1FFFFFF,//25
	0x3FFFFFF,//26 
	0x7FFFFFF,//27
	0xFFFFFFF,//28 
	0x1FFFFFFF,//29
	0x3FFFFFFF,//30
	0x7FFFFFFF,//31
	0xFFFFFFFF//32
};

void CBitWriter::WriteBits(uint64_t val, uint32_t bit_cnt)
{
	
	if (bit_cnt > 31)
	{
		_WriteBits(val & BIT_MASKS[31], 31);
		bit_cnt -= 31;
		val >>= (31);
	}

	_WriteBits(val, bit_cnt);
}


void CBitWriter::_WriteBits(uint64_t val, uint32_t bit_cnt)
{

	uint32_t byte_offset = m_bit_pos >> 5;
	byte_offset <<= 2;
	*(uint64_t*)(m_buffer + byte_offset) |= (val & BIT_MASKS[bit_cnt]) << (m_bit_pos & 0x1f);
	m_bit_pos += bit_cnt;
}


CBitReader::CBitReader( )
{
 
}

void CBitReader::AttachBuffer(byte_t* buffer, uint32_t size)
{
	m_buffer = buffer;
	m_size = size;
	m_bit_pos = 0;
}

CBitReader::~CBitReader()
{

}

static uint64_t BIT_MASKS_READ[] = {
	0x0,  //0
	0xFFFFFFFFFFFFFFFE, //1
	0xFFFFFFFFFFFFFFFC, //2
	0xFFFFFFFFFFFFFFF8, //3
	0xFFFFFFFFFFFFFFF0,//4
	0xFFFFFFFFFFFFFFE0,//5
	0xFFFFFFFFFFFFFFC0, //6
	0xFFFFFFFFFFFFFF80, //7
	0xFFFFFFFFFFFFFF00,//8
	0xFFFFFFFFFFFFFE00,//9 
	0xFFFFFFFFFFFFFC00,//10 
	0xFFFFFFFFFFFFF800,//11
	0xFFFFFFFFFFFFF000,//12
	0xFFFFFFFFFFFFE000,//13
	0xFFFFFFFFFFFFC000,//14
	0xFFFFFFFFFFFF8000,//15
	0xFFFFFFFFFFFF0000,//16 
	0xFFFFFFFFFFFE0000,//17
	0xFFFFFFFFFFFC0000,//18
	0xFFFFFFFFFFF80000,//19
	0xFFFFFFFFFFF00000,//20
	0xFFFFFFFFFFE00000,//21 
	0xFFFFFFFFFFC00000,//22
	0xFFFFFFFFFF800000,//23
	0xFFFFFFFFFF000000,//24
	0xFFFFFFFFFE000000,//25
	0xFFFFFFFFFC000000,//26 
	0xFFFFFFFFF8000000,//27
	0xFFFFFFFFF0000000,//28 
	0xFFFFFFFFE0000000,//29
	0xFFFFFFFFC0000000,//30
	0xFFFFFFFF80000000,//31
	0xFFFFFFFF00000000//32
};



void CBitReader::ReadBits(uint64_t& val, uint32_t bit_cnt)
{
	if (bit_cnt > 31)
	{
		uint64_t val1 = 0;
		uint64_t val2 = 0;
		_ReadBits(val1, 31);
		_ReadBits(val2, bit_cnt - 31);

		val = val1 | val2 << 31;

	 }
	else
	_ReadBits(val, bit_cnt);
}


void CBitReader::_ReadBits(uint64_t& val, uint32_t bit_cnt)
{
	uint32_t byte_offset = m_bit_pos >> 5;
	byte_offset <<= 2;

	val = *(uint64_t*)(m_buffer + byte_offset);
	val >>= m_bit_pos & 0x1f;
	m_bit_pos += bit_cnt;


	uint64_t mask = 0xFFFF;
	val &= BIT_MASKS[bit_cnt];
}