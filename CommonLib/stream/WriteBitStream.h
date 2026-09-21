#pragma once

#include "BitStream.h"
namespace CommonLib
{
	class WriteBitStream : public TBaseBitMemryStream<IWriteBitStreamBase>
	{
	public:
		typedef TBaseBitMemryStream<IWriteBitStreamBase> TBase;
		WriteBitStream(IAllocPtr pAlloc);
		~WriteBitStream();

		// keep the byte_t / multi-bit overloads of the base visible
		using IWriteBitStreamBase::WriteBit;
		using IWriteBitStreamBase::WriteBitSafe;

		virtual void WriteBit(bool bBit);
		virtual bool WriteBitSafe(bool bBit);
		virtual void Create(size_t nSize);
		virtual void Resize(uint32_t nSize);
	private:


	};

}

