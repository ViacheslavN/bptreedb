#pragma once

#include "../CompressUtils.h"
#include "../../../utils/BitUtils.h"

namespace bptreedb
{

	class CBitBase
	{
	public:
		CBitBase();
		~CBitBase();

		void AttachBuffer(byte_t* buffer, uint32_t size);
		static uint32_t GetByteForBits(uint32_t nBits);

	protected:
		byte_t* m_buffer{ 0 };
		uint32_t m_size{ 0 };
		uint32_t m_bit_pos{ 0 };

		const uint32_t n_max_bits_for_ops = 31;

	};

	class CBitWriter : public CBitBase
	{
	public:
		CBitWriter();
		~CBitWriter();

		void WriteBits(uint64_t val, uint32_t bit_cnt);

	


	private:

		void _WriteBits(uint64_t val, uint32_t bit_cnt);
	};


	class CBitReader : public CBitBase
	{
	public:
		CBitReader(){}
		~CBitReader(){}

		uint64_t ReadBits(uint32_t bit_cnt);

	private:
		uint64_t _ReadBits(uint32_t bit_cnt);
	};

}