#include "stdafx.h"
#include "ZStream.h"
#include "../../exception/exc_base.h"


namespace CommonLib
{



	CZStream::CZStream()
	{

	}

	CZStream::~CZStream()
	{

	}

	void CZStream::AttachOut(byte_t *pData, size_t nSize)
	{
		m_strm.next_out = (Bytef*)pData;
		m_strm.avail_out = (uInt)nSize;
	}

	void CZStream::AttachIn(const byte_t *pData, size_t nSize)
	{
		m_strm.next_in = (z_const Bytef*)pData;
		m_strm.avail_in = (uInt)nSize;
	}

	uint32_t CZStream::GetAvailIn() const
	{
		return m_strm.avail_in;
	}

	uint32_t CZStream::GetAvailOut() const
	{
		return m_strm.avail_out;
	}



	//////////////////////////////////////////////////////////////////////
/*
	CZStream::CZStream(int level) : m_encode(true)
	{
		try
		{
			m_strm.zalloc = 0;
			m_strm.zfree = 0;
			int res = deflateInit(&m_strm, level);
			if (res != Z_OK)
				throw CommonLib::CExcBase("deflateInit error {0}", res);
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExcT("CZStream failed to create", exc);
		}
	}

	CZStream::CZStream() : m_encode(false)
	{
		try
		{
			m_strm.zalloc = 0;
			m_strm.zfree = 0;
			int	res = inflateInit(&m_strm);
			if (res != Z_OK)
				throw CommonLib::CExcBase("deflateInit error {0}", res);
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExcT("CZStream failed to create", exc);
		}
	}

	CZStream::~CZStream()
	{
		try
		{
			int err = Z_OK;
			if(m_encode)
				err = deflateEnd(&m_strm);
			else
				err = inflateEnd(&m_strm);

			if (err != Z_OK)
				throw CExcBase("deflateEnd error {0}", err);
		}
		catch (std::exception& exc)
		{
			exc; //TO DO log
		}
	}

	void CZStream::AttachOut(byte_t *pData, size_t nSize)
	{
		m_strm.next_out = (Bytef*)pData;
		m_strm.avail_out = (uInt)nSize;
	}

	void CZStream::AttachIn(const byte_t *pData, size_t nSize)
	{
		m_strm.next_in = (z_const Bytef*)pData;
		m_strm.avail_in = (uInt)nSize;
	}

	int CZStream::Deflate(int flush)
	{
		return deflate(&m_strm, flush);
	}

	int CZStream::Inflate(int flush)
	{
		return inflate(&m_strm, flush);
	}

	void CZStream::DeflatePending(unsigned *pending,
		int *bits)
	{
		deflatePending(&m_strm, pending, bits);
	}

	uint32_t CZStream::GetAvailIn() const
	{
		return m_strm.avail_in;
	}

	uint32_t CZStream::GetAvailOut() const
	{
		return m_strm.avail_out;
	}

	uint32_t CZStream::GetTotalOut() const
	{
		return m_strm.total_out;
	}

	uint32_t CZStream::GetTotalIn() const
	{
		return m_strm.total_in;
	}*/
}