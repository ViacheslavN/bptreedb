#include "stdafx.h"
#include "ZCompressStream.h"
#include "../../exception/exc_base.h"

namespace CommonLib
{
	CZCompressStream::CZCompressStream(int compressionLevel)
	{
		try
		{
			m_strm.zalloc = 0;
			m_strm.zfree = 0;
			int res = deflateInit(&m_strm, compressionLevel);
			if (res != Z_OK)
				throw CExcBase("deflateInit error {0}", res);
		}
		catch (std::exception& exc)
		{
			CExcBase::RegenExcT("CZStream failed to create", exc);
		}
	}

	CZCompressStream::~CZCompressStream()
	{
		try
		{
			int err = deflateEnd(&m_strm);
			if (err != Z_OK)
				throw CExcBase("deflateEnd error {0}", err);
		}
		catch (std::exception& exc)
		{
			exc; //TO DO log
		}
	}

	void CZCompressStream::Compress()
	{
		int res = Deflate(Z_NO_FLUSH);
		if (res != Z_OK)
			throw CExcBase("Inflate error {0}", res);
	}

	void CZCompressStream::FinishCompress()
	{
		int res = Deflate(Z_FINISH);
		if (res != Z_STREAM_END && res !=  Z_OK)
			throw CommonLib::CExcBase("inflate fihish error {0}", res);
	}

	int CZCompressStream::Deflate(int flush)
	{
		return deflate(&m_strm, flush);
	}

}