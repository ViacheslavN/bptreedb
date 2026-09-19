#pragma once
#include "stream.h"


namespace CommonLib
{

	template<class I>
	class TMemoryStreamBaseEmpty : public I
	{

	private:
		TMemoryStreamBaseEmpty(const TMemoryStreamBaseEmpty& stream) {}
		TMemoryStreamBaseEmpty& operator=(const TMemoryStreamBaseEmpty& stream) {}
	public:

		TMemoryStreamBaseEmpty(std::shared_ptr<IAlloc> pAlloc = std::shared_ptr<IAlloc>())
		{
			m_bIsBigEndian = IStream::IsBigEndian();

		}

		~TMemoryStreamBaseEmpty()
		{}

		//IStream
		virtual size_t Size() const
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Size isn't implemented");
		}
		
		virtual void Seek(size_t position, enSeekOffset offset)
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Seek isn't implemented");
		}

		virtual bool  SeekSafe(size_t position, enSeekOffset offset)
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Seek isn't implemented");
		}
		
		virtual size_t Pos() const
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Pos isn't implemented");
		}
		
		virtual void Reset()
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Reset isn't implemented");
		
		}
					
		virtual TStreamPtr Deattach()
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Deattach isn't implemented");
		}
		virtual void Close()
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Close isn't implemented");
		}


		//IMemoryStream
		virtual void AttachBuffer(byte_t* pBuffer, size_t nSize, bool bCopy = false)
		{
			throw CExcBase("TMemoryStreamBaseEmpty: AttachBuffer isn't implemented");
		}

		virtual void AttachBuffer(CommonLib::IMemStreamBufferPtr ptrBuffer)
		{
			throw CExcBase("TMemoryStreamBaseEmpty: AttachBuffer isn't implemented");
		}

		virtual byte_t* DeattachBuffer()
		{
			throw CExcBase("TMemoryStreamBaseEmpty: DeattachBuffer isn't implemented");
		}
		
		virtual byte_t* Buffer()
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Buffer isn't implemented");
		}

		virtual const byte_t* Buffer() const
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Buffer isn't implemented");
		}
		
		virtual byte_t* BufferFromCurPos()
		{
			throw CExcBase("TMemoryStreamBaseEmpty: BufferFromCurPos isn't implemented");
		}

		virtual const byte_t* BufferFromCurPos() const 
		{
			throw CExcBase("TMemoryStreamBaseEmpty: BufferFromCurPos isn't implemented");
		}

		virtual void Create(size_t nSize)
		{
			throw CExcBase("TMemoryStreamBaseEmpty: Create isn't implemented");
		}

		virtual void Resize(size_t nSize) { throw CExcBase("TMemoryStreamBaseEmpty: resize isn't implemented"); }
	protected:
		bool m_bIsBigEndian;
	};
}