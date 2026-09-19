
#pragma once

#include "stream.h"
#include "../alloc/alloc.h"

namespace CommonLib
{
	class IBitWriteStream
	{
	public:
		IBitWriteStream() {}
		virtual ~IBitWriteStream() {}
		virtual void WriteBit(bool bBit) = 0;
		virtual void WriteBit(byte_t nBit) = 0;
		virtual void WriteBits(byte_t nBits, size_t nCntBits) = 0;
		virtual void WriteBits(uint16_t nBits, size_t nCntBits) = 0;
		virtual void WriteBits(uint32_t  nBits, size_t nCntBits) = 0;
		virtual void WriteBits(uint64_t  nBits, size_t nCntBits) = 0;
	};


	class IBitReadStream
	{
	public:
		IBitReadStream() {}
		virtual ~IBitReadStream() {}
		virtual bool ReadBit() = 0;
		virtual void ReadBits(byte_t & nBits, size_t nCntBits) = 0;
		virtual void ReadBits(uint16_t &  nBits, size_t nCntBits) = 0;
		virtual void ReadBits(uint32_t &  nBits, size_t nCntBits) = 0;
		virtual void ReadBits(uint64_t &  nBits, size_t nCntBits) = 0;
	};



	class IReadBitStreamBase : public IBitReadStream
	{
	public:


		IReadBitStreamBase();
		virtual ~IReadBitStreamBase();
		virtual void ReadBits(byte_t & nBits, size_t nCntBits);
		virtual void ReadBits(int8_t & nBits, size_t nCntBits);
		virtual void ReadBits(uint16_t &  nBits, size_t nCntBits);
		virtual void ReadBits(uint32_t &  nBits, size_t nCntBits);
		virtual void ReadBits(uint64_t &  nBits, size_t nCntBits);
		virtual void ReadBits(int16_t &  nBits, size_t nCntBits);
		virtual void ReadBits(int32_t &  nBits, size_t nCntBits);
		virtual void ReadBits(int64_t &  nBits, size_t nCntBits);

	protected:

		template <class TVal>
		void _readBits(TVal& bits, size_t nCount)
		{
			bits = 0;
			for (size_t i = 0; i < nCount; ++i)
			{
				bool bBit = ReadBit();
				if (bBit)
					bits |= ((TVal)0x01 << i);

			}
		}
	};


	class IWriteBitStreamBase : public IBitWriteStream
	{
	public:


		IWriteBitStreamBase();
		virtual ~IWriteBitStreamBase();
		virtual void WriteBit(bool nBit) = 0;
		virtual void WriteBit(byte_t  nBit);
		virtual void WriteBits(byte_t  nBits, size_t nCntBits);
		virtual void WriteBits(uint16_t  nBits, size_t nCntBits);
		virtual void WriteBits(uint32_t  nBits, size_t nCntBits);
		virtual void WriteBits(uint64_t  nBits, size_t nCntBits);
		virtual void WriteBits(int16_t  nBits, size_t nCntBits);
		virtual void WriteBits(int32_t  nBits, size_t nCntBits);
		virtual void WriteBits(int64_t  nBits, size_t nCntBits);

		virtual bool WriteBitSafe(bool  nBit) = 0;
		virtual bool WriteBitSafe(byte_t  nBit);
		virtual bool WriteBitsSafe(byte_t  nBits, size_t nCntBits);
		virtual bool WriteBitsSafe(uint16_t  nBits, size_t nCntBits);
		virtual bool WriteBitsSafe(uint32_t  nBits, size_t nCntBits);
		virtual bool WriteBitsSafe(uint64_t  nBits, size_t nCntBits);
		virtual bool WriteBitsSafe(int16_t  nBits, size_t nCntBits);
		virtual bool WriteBitsSafe(int32_t  nBits, size_t nCntBits);
		virtual bool WriteBitsSafe(int64_t  nBits, size_t nCntBits);

	protected:

		template <class TVal>
		void _writeBits(TVal bits, size_t nCount)
		{
			for (size_t i = 0; i < nCount; ++i)
			{
				bool bBit = (bits & ((TVal)0x01 << i)) ? true : false;
				WriteBit(bBit);
			}
		}

		template <class TVal>
		bool _writeBitsSafe(TVal bits, size_t nCount)
		{
			for (size_t i = 0; i < nCount; ++i) //TO DO write bytes at frist
			{
				bool bBit = (bits & ((TVal)0x01 << i)) ? true : false;
				if (!WriteBitSafe(bBit))
					return false;
			}

			return true;
		}
	};


	template< class I>
	class TBaseBitMemryStream : public TMemoryStreamBase<I>
	{
	public:
		typedef TMemoryStreamBase<I> TBase;
		TBaseBitMemryStream(CommonLib::IAllocPtr pAlloc) : TBase(pAlloc), m_nCurrBit(0)
		{

		}
		virtual ~TBaseBitMemryStream()
		{

		}

		virtual void Seek(uint32_t position, enSeekOffset offset)
		{
			Seek(position, offset);
			m_nCurrBit = 0;
		}


	protected:
		static const uint32_t m_nBitBase = 7;
		uint32_t m_nCurrBit;
	};

}
