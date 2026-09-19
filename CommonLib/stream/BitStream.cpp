#include "stdafx.h"
#include "BitStream.h"
#include <assert.h>

namespace CommonLib
{

	IReadBitStreamBase::IReadBitStreamBase()
	{

	}
	IReadBitStreamBase::~IReadBitStreamBase() {

	}
	void IReadBitStreamBase::ReadBits(byte_t& nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		_readBits<byte_t>(nBits, nCntBits);
	}
	void IReadBitStreamBase::ReadBits(int8_t& nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		_readBits<int8_t>(nBits, nCntBits);
	}
	void IReadBitStreamBase::ReadBits(uint16_t&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_readBits<uint16_t>(nBits, nCntBits);
	}
	void IReadBitStreamBase::ReadBits(uint32_t&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_readBits<uint32_t>(nBits, nCntBits);
	}
	void IReadBitStreamBase::ReadBits(uint64_t&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_readBits<uint64_t>(nBits, nCntBits);
	}


	void IReadBitStreamBase::ReadBits(int16_t&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_readBits<int16_t>(nBits, nCntBits);
	}
	void IReadBitStreamBase::ReadBits(int32_t&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_readBits<int32_t>(nBits, nCntBits);
	}
	void IReadBitStreamBase::ReadBits(int64_t&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_readBits<int64_t>(nBits, nCntBits);
	}



	IWriteBitStreamBase::IWriteBitStreamBase() {}
	IWriteBitStreamBase::~IWriteBitStreamBase() {}

	void IWriteBitStreamBase::WriteBit(byte_t nBit)
	{

		bool bitValue = (nBit & 0x01) ? true : false;
		WriteBit(bitValue);

	}


	void IWriteBitStreamBase::WriteBits(byte_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		 _writeBits<byte_t>(nBits, nCntBits);
	}

	void IWriteBitStreamBase::WriteBits(uint16_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_writeBits<uint16_t>(nBits, nCntBits);


	}
	void IWriteBitStreamBase::WriteBits(uint32_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_writeBits<uint32_t>(nBits, nCntBits);
	}

	void IWriteBitStreamBase::WriteBits(uint64_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_writeBits<uint64_t>(nBits, nCntBits);
	}

	void IWriteBitStreamBase::WriteBits(int16_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_writeBits<int16_t>(nBits, nCntBits);

	}
	void IWriteBitStreamBase::WriteBits(int32_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_writeBits<int32_t>(nBits, nCntBits);
	}
	void IWriteBitStreamBase::WriteBits(int64_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_writeBits<int64_t>(nBits, nCntBits);
	}




	bool IWriteBitStreamBase::WriteBitSafe(byte_t nBit)
	{

		bool bitValue = (nBit & 0x01) ? true : false;
		return WriteBitSafe(bitValue);

	}

	bool IWriteBitStreamBase::WriteBitsSafe(byte_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		return _writeBitsSafe<byte_t>(nBits, nCntBits);
	}

	bool IWriteBitStreamBase::WriteBitsSafe(uint16_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		return _writeBitsSafe<uint16_t>(nBits, nCntBits);

	
	}
	bool IWriteBitStreamBase::WriteBitsSafe(uint32_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		return _writeBitsSafe<uint32_t>(nBits, nCntBits);
	}

	bool IWriteBitStreamBase::WriteBitsSafe(uint64_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		return _writeBitsSafe<uint64_t>(nBits, nCntBits);
	}

	bool IWriteBitStreamBase::WriteBitsSafe(int16_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		return _writeBitsSafe<int16_t>(nBits, nCntBits);

	}
	bool IWriteBitStreamBase::WriteBitsSafe(int32_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		return _writeBitsSafe<int32_t>(nBits, nCntBits);
	}
	bool IWriteBitStreamBase::WriteBitsSafe(int64_t nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		return _writeBitsSafe<int64_t>(nBits, nCntBits);
	}

}