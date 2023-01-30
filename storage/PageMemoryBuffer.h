#pragma once
#include "../../CommonLib/stream/MemoryStreamBuffer.h"


namespace bptreedb
{
	namespace storage
	{
		typedef std::shared_ptr<class CPageMemoryBuffer> CPageMemoryBufferPtr;

		class CPageMemoryBuffer : public CommonLib::IMemStreamBuffer
		{
		public:

			CPageMemoryBuffer(CommonLib::IAllocPtr ptrAlloc, uint32_t offset = 0);
			virtual ~CPageMemoryBuffer();

			virtual void Create(size_t nSize);
			virtual void AttachBuffer(byte_t* pBuffer, size_t nSize, bool bCopy = false);
			virtual bool IsAttachedBuffer() const;
			virtual byte_t* DeattachBuffer();
			virtual byte_t* GetData();
			virtual const byte_t* GetData() const;
			virtual uint32_t GetSize() const;
			virtual void Close();
			virtual CommonLib::IMemStreamBufferPtr CreateBuffer();

			byte_t* GetFullData();
			const byte_t* GetFullData() const;
			uint32_t GetFullSize() const;
			void SetOffset(uint32_t offset);

		private:
			byte_t* m_pBuffer{ nullptr };
			CommonLib::IAllocPtr m_ptrAlloc;
			bool m_bAttach{ false };
			size_t m_nSize{ 0 };
			int32_t m_dataOffset{ 0 };
		};
	}
}