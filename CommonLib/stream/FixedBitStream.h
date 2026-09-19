#pragma once

#include "stream.h"
#include "BitStream.h"

namespace CommonLib
{

	class CFxBitWriteStream : public TBaseBitMemryStream<IWriteBitStreamBase>
	{
	public:
		typedef TBaseBitMemryStream<IWriteBitStreamBase> TBase;
		CFxBitWriteStream();
		~CFxBitWriteStream();
		virtual void WriteBit(bool bBit);
		virtual bool WriteBitSafe(bool bBit);
	private:

	};



	class CFxBitReadStream : public TBaseBitMemryStream<IReadBitStreamBase>
	{
	public:
		typedef  TBaseBitMemryStream<IReadBitStreamBase> TBase;
		CFxBitReadStream();
		~CFxBitReadStream();

		virtual bool ReadBit();
	private:


	};
}
