#include "stdafx.h"
#include "ZDecompressStream.h"
#include "../../exception/exc_base.h"

namespace CommonLib
{
	CZDecompressStream::CZDecompressStream()
	{
		try
		{
			m_strm.zalloc = 0;
			m_strm.zfree = 0;
			int res = inflateInit(&m_strm);
			if (res != Z_OK)
				throw CExcBase("inflateInit error {0}", res);
		}
		catch (std::exception& exc)
		{
			CExcBase::RegenExcT("CZDecompressStream failed to create", exc);
		}
	}

	CZDecompressStream::~CZDecompressStream()
	{
		try
		{
			int err = inflateEnd(&m_strm);
			if (err != Z_OK)
				throw CExcBase("inflateEnd error {0}", err);
		}
		catch (std::exception& exc)
		{
			exc; //TO DO log
		}
	}

	void CZDecompressStream::Decompress()
	{
		int res = Inflate(Z_NO_FLUSH);
		if (res != Z_OK && res != Z_STREAM_END)
			throw CExcBase("Inflate error {0}", res);
	}

	int CZDecompressStream::Inflate(int flush)
	{
		return inflate(&m_strm, flush);
	}

}