#pragma once


#include "../alloc/alloc.h"
#include "../exception/exc_base.h"


namespace CommonLib
{


	typedef std::shared_ptr<class IMemStreamBuffer> IMemStreamBufferPtr;

	class IMemStreamBuffer
	{
	public:
		IMemStreamBuffer() {}
		virtual ~IMemStreamBuffer() {}

		virtual void Create(uint32_t nSize) = 0;
		virtual void AttachBuffer(byte_t* pBuffer, uint32_t nSize, bool bCopy = false) = 0;
		virtual bool IsAttachedBuffer() const = 0;
		virtual byte_t* DeattachBuffer() = 0;
		virtual byte_t* GetData() = 0;
		virtual const byte_t* GetData() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual void Close() = 0;
		virtual IMemStreamBufferPtr CreateBuffer() const = 0;
	};


	class CMemoryStreamBuffer : public IMemStreamBuffer
	{
	public:
		CMemoryStreamBuffer(std::shared_ptr<IAlloc> ptrAlloc = std::shared_ptr<IAlloc>());
		virtual ~CMemoryStreamBuffer();


		virtual void Create(uint32_t nSize);
		virtual void AttachBuffer(byte_t* pBuffer, uint32_t nSize, bool bCopy = false);
		virtual bool IsAttachedBuffer() const;
		virtual byte_t* DeattachBuffer();
		virtual byte_t* GetData();
		virtual const byte_t* GetData() const;
		virtual uint32_t GetSize() const;
		virtual void Close();
		virtual IMemStreamBufferPtr CreateBuffer() const;

	private:
		byte_t* m_pBuffer;
		std::shared_ptr<IAlloc> m_ptrAlloc;
		bool m_bAttach;
		uint32_t m_nSize;

	};
}