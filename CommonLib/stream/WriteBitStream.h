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

		virtual void WriteBit(bool bBit);
		virtual void Resize(uint32_t nSize);
	private:


	};

}

