
#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/stream/MemoryStream.h"
#include "../../CommonLib/stream/StreamBaseEmpty.h"

namespace bptreedb
{
	namespace utils
	{

template<class _TStorage>
class TReadStreamPage : public CommonLib::TMemoryStreamBaseEmpty<CommonLib::IReadStreamBase>
{
public:
	typedef _TStorage TStorage;
	typedef std::shared_ptr<TStorage> TStoragePtr;

	TReadStreamPage(TStoragePtr& pStorage) :
		m_pStorage(pStorage)
	{

	}

	void Open(int64_t nPageAddr, uint32_t nPageSize, bool bNeedDecrypt)
	{
		try
		{
			m_nPageSize = nPageSize;
			m_NeedDecrypt = bNeedDecrypt;

			OpenNextPage(nPageAddr);

		}
		catch (CommonLib::CExcBase& excSrc)
		{
			excSrc.AddMsgT("Failed open WriteStreamPage addr: %1, pagesize: %2", nPageAddr, m_nPageSize);
			throw;
		}

	}

	virtual void ReadBytes(byte_t* dst, size_t size)
	{
		read_bytes_impl(dst, size, false);
	}

	virtual void ReadInverse(byte_t* dst, size_t size)
	{
		read_bytes_impl(dst, size, true);
	}

	virtual void ReadStream(IStream *pStream, bool bAttach)
	{
		throw CommonLib::CExcBase("ReadStreamPage: ReadStream not implement");
	}

	virtual bool ReadBytesSafe(byte_t* dst, size_t size)
	{
		try
		{
			read_bytes_impl(dst, size, false);
			return true;
		}
		catch (...)
		{
		}

		return false;
	}

	virtual bool ReadInverseSafe(byte_t* dst, size_t size)
	{
		try
		{
			read_bytes_impl(dst, size, true);
			return true;
		}
		catch (...)
		{
		}

		return false;
	}

	virtual bool ReadStreamSafe(IStream *pStream, bool bAttach)
	{
		return false;
	}


private:

	void read_bytes_impl(byte_t* buffer, size_t size, bool bInvers)
	{
		try
		{
			size_t nPos = 0;
			while (size)
			{
				size_t nFreeSize = m_stream.Size() - m_stream.Pos();
				if (nFreeSize >= size)
				{
					if (bInvers)
						m_stream.ReadInverse(buffer + nPos, size);
					else
						m_stream.ReadBytes(buffer + nPos, size);

					size = 0;
				}
				else
				{
					size_t nReadSize = nFreeSize;
					if (nReadSize)
					{
						if (bInvers)
							m_stream.ReadInverse(buffer + nPos, nReadSize);
						else
							m_stream.ReadBytes(buffer + nPos, nReadSize);

						size -= nReadSize;
						nPos += nReadSize;
					}

					OpenNextPage(m_nNextAddr);
				}
			}

		}
		catch (std::exception& excSrc)
		{
			CommonLib::CExcBase::RegenExcT("ReadStreamPage: Failed read  size: %1", size, excSrc);
			throw;
		}

	}

	void OpenNextPage(int64_t nPageAddr)
	{
		try
		{
			m_pCurrentPage = m_pStorage->GetFilePage(nPageAddr, m_nPageSize, m_NeedDecrypt);
			m_stream.AttachBuffer(m_pCurrentPage->GetData(), m_pCurrentPage->GetPageSize());
			m_nNextAddr = m_stream.ReadInt64();
			m_stream.ReadInt64();
		}
		catch (std::exception& excSrc)
		{
			CommonLib::CExcBase::RegenExcT("ReadStreamPage: Failed openpage  addr: %1, pagesize: %2", nPageAddr, m_nPageSize, excSrc);
		}
	}

	private:
		int64_t m_nNextAddr{ -1 };;
		uint32_t m_nPageSize{ 0 };
		TStoragePtr m_pStorage;
		FilePagePtr m_pCurrentPage;
		CommonLib::CReadMemoryStream m_stream;
		bool m_NeedDecrypt{ false };

	};
}
}
