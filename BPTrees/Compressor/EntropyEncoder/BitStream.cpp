#include "stdafx.h"
#include "BitStream.h"

namespace bptreedb
{
	CBitBase::CBitBase()
	{}

	CBitBase::~CBitBase()
	{}

	void CBitBase::AttachBuffer(byte_t* buffer)
	{
		m_buffer = buffer;
		m_bit_pos = 0;
	}

	uint32_t CBitBase::GetByteForBits(uint32_t nBits)
	{
		uint32_t nBitSize = (nBits + 7) / 8;
		return nBitSize + sizeof(uint64_t);
	}


	static const uint64_t BIT_MASKS[] = {
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


	CBitWriter::CBitWriter()
	{

	}

	CBitWriter::~CBitWriter()
	{

	}

	void CBitWriter::WriteBits(uint64_t val, uint32_t bit_cnt)
	{
		if (bit_cnt > n_max_bits_for_ops)
		{
			_WriteBits(val & BIT_MASKS[n_max_bits_for_ops], n_max_bits_for_ops);
			bit_cnt -= n_max_bits_for_ops;
			val >>= (n_max_bits_for_ops);
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


	uint64_t CBitReader::ReadBits(uint32_t bit_cnt)
	{
		if (bit_cnt > n_max_bits_for_ops)
		{
			uint64_t val1 = _ReadBits(n_max_bits_for_ops);
			uint64_t val2 = _ReadBits(bit_cnt - n_max_bits_for_ops);

			return  val1 | val2 << n_max_bits_for_ops;

		}
		else
			return	_ReadBits( bit_cnt);
	}


	uint64_t CBitReader::_ReadBits(uint32_t bit_cnt)
	{
		uint32_t byte_offset = m_bit_pos >> 5;
		byte_offset <<= 2;

		uint64_t val = *(uint64_t*)(m_buffer + byte_offset);
		val >>= m_bit_pos & 0x1f;
		m_bit_pos += bit_cnt;

		val &= BIT_MASKS[bit_cnt];

		return val;
	}

}