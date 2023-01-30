#include "pch.h"
#include "PageMemoryBuffer.h"

namespace bptreedb
{
	namespace storage
	{
		CPageMemoryBuffer::CPageMemoryBuffer(CommonLib::IAllocPtr ptrAlloc, uint32_t offset) : m_ptrAlloc(ptrAlloc), m_dataOffset(offset)
		{
			
		}

		CPageMemoryBuffer::~CPageMemoryBuffer()
		{
			try
			{
				if (!m_bAttach && m_pBuffer)
				{
					m_ptrAlloc->Free(m_pBuffer);
					m_pBuffer = 0;
					m_nSize = 0;
				}
			}
			catch (CommonLib::CExcBase& exc)
			{
				exc; // TO DO log;
			}
		}

		void CPageMemoryBuffer::Create(size_t nSize)
		{
			try
			{

				if (m_ptrAlloc.get() == nullptr)
					throw CommonLib::CExcBase(L"Allocator is null");

				if (!m_bAttach && m_pBuffer)
				{
					m_ptrAlloc->Free(m_pBuffer);
				}

				m_pBuffer = (byte_t*)m_ptrAlloc->Alloc(sizeof(byte_t) * nSize);
				m_nSize = nSize;
				m_bAttach = false;
			}
			catch (CommonLib::CExcBase& exc)
			{
				exc.AddMsgT(L"Can't create stream size: %1", nSize);
				throw;
			}
		}

		void CPageMemoryBuffer::AttachBuffer(byte_t* pBuffer, size_t nSize, bool bCopy)
		{
			try
			{
				if (bCopy)
				{
					Create(nSize);
					memcpy(m_pBuffer, pBuffer, nSize);
					m_bAttach = false;
				}
				else
				{
					m_pBuffer = pBuffer;
					m_bAttach = true;

				}
				m_nSize = nSize;
			}
			catch (CommonLib::CExcBase& exc)
			{
				exc.AddMsg(L"Can't attach to buffer");
				throw;
			}
		}

		bool CPageMemoryBuffer::IsAttachedBuffer() const
		{
			return m_bAttach;
		}

		byte_t* CPageMemoryBuffer::DeattachBuffer()
		{
			byte_t* tmp = m_pBuffer;
			m_nSize = 0;
			m_pBuffer = 0;
			m_bAttach = false;
			return tmp;
		}

		byte_t* CPageMemoryBuffer::GetData()
		{
			return m_pBuffer + m_dataOffset;
		}

		const byte_t* CPageMemoryBuffer::GetData() const
		{
			return m_pBuffer + m_dataOffset;
		}

		uint32_t CPageMemoryBuffer::GetSize() const
		{
			return m_nSize - m_dataOffset;
		}

		byte_t* CPageMemoryBuffer::GetFullData()
		{
			return m_pBuffer;
		}

		const byte_t* CPageMemoryBuffer::GetFullData() const
		{
			return m_pBuffer;
		}

		uint32_t CPageMemoryBuffer::GetFullSize() const
		{
			return m_nSize;
		}

		void CPageMemoryBuffer::SetOffset(uint32_t offset)
		{
			m_dataOffset = offset;
		}

		void CPageMemoryBuffer::Close()
		{
			try
			{
				m_nSize = 0;
				if (!m_bAttach && m_pBuffer)
				{
					if (m_ptrAlloc.get() == nullptr)
						throw CommonLib::CExcBase(L"Allocator is null");

					m_ptrAlloc->Free(m_pBuffer);
					m_pBuffer = 0;
				}
			}
			catch (CommonLib::CExcBase& exc)
			{
				exc.AddMsg(L"Can't close buffer");
				throw;
			}
		}

		CommonLib::IMemStreamBufferPtr CPageMemoryBuffer::CreateBuffer()
		{
			return std::make_shared<CPageMemoryBuffer>(m_ptrAlloc, m_dataOffset);
		}

	}
}