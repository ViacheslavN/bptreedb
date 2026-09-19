#include "stdafx.h"
#include "MemoryStreamBuffer.h"

namespace CommonLib
{
	CMemoryStreamBuffer::CMemoryStreamBuffer(std::shared_ptr<IAlloc> ptrAlloc) : m_ptrAlloc(ptrAlloc), m_pBuffer(0), m_bAttach(false), m_nSize(0)
	{

	}

	CMemoryStreamBuffer::~CMemoryStreamBuffer()
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

	void CMemoryStreamBuffer::AttachBuffer(byte_t* pBuffer, uint32_t nSize, bool bCopy)
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
		catch (CExcBase& exc)
		{
			exc.AddMsg(L"Can't attach to buffer");
			throw;
		}
	}

	bool CMemoryStreamBuffer::IsAttachedBuffer() const
	{
		return m_bAttach;
	}

	void CMemoryStreamBuffer::Create(uint32_t nSize)
	{
		try
		{

			if (m_ptrAlloc.get() == nullptr)
				throw CExcBase(L"Allocator is null");

			if (!m_bAttach && m_pBuffer)
			{
				m_ptrAlloc->Free(m_pBuffer);
			}

			m_pBuffer = (byte_t*)m_ptrAlloc->Alloc(sizeof(byte_t) * nSize);
			m_nSize = nSize;
			m_bAttach = false;
		}
		catch (CExcBase& exc)
		{
			exc.AddMsgT(L"Can't create stream size: {0}", nSize);
			throw;
		}
	}	

	void CMemoryStreamBuffer::Close()
	{
		try
		{
			m_nSize = 0;
			if (!m_bAttach && m_pBuffer)
			{
				if (m_ptrAlloc.get() == nullptr)
					throw CExcBase(L"Allocator is null");

				m_ptrAlloc->Free(m_pBuffer);
				m_pBuffer = 0;
			}
		}
		catch (CExcBase& exc)
		{
			exc.AddMsg(L"Can't close buffer");
			throw;
		}
	}

	byte_t* CMemoryStreamBuffer::DeattachBuffer()
	{
		byte_t* tmp = m_pBuffer;
		m_nSize = 0;
		m_pBuffer = 0;
		m_bAttach = false;
		return tmp;
	}

	byte_t* CMemoryStreamBuffer::GetData()
	{
		return m_pBuffer;
	}

	const byte_t* CMemoryStreamBuffer::GetData() const
	{
		return m_pBuffer;
	}

	uint32_t CMemoryStreamBuffer::GetSize() const
	{
		return  m_nSize;
	}

	IMemStreamBufferPtr CMemoryStreamBuffer::CreateBuffer() const
	{
		return std::make_shared<CMemoryStreamBuffer>(m_ptrAlloc);
	}
}