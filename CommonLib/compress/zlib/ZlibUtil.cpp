#include "stdafx.h"
#include "ZlibUtil.h"
#include "zlib.h"
#include "../../exception/exc_base.h"

namespace CommonLib
{
	void CZLibUtil::CompressData(ECompressLevel level, io::TReadPtr pSrc, io::TWritePtr pDst)
	{
		try
		{
			if (pSrc.get() == nullptr || pDst.get() == nullptr)
				throw CExcBase("invalid params");

			std::vector<byte_t> outBuf(1024 * 1024);
			std::vector<byte_t> inBuf(1024 * 1024);

			z_stream strm;
			strm.zalloc = 0;
			strm.zfree = 0;
			strm.next_out = (Bytef*)&outBuf[0];
			strm.avail_out = (uInt)outBuf.size();


			int res = deflateInit(&strm, level);
			if (res != Z_OK)
				throw CExcBase("deflateInit error {0}", res);

			std::streamsize size;
			while ((size =  pSrc->Read(&inBuf[0], inBuf.size())) != 0)
			{

				strm.next_in = (Bytef*)&inBuf[0];
				strm.avail_in = (uInt)size;

				while (strm.avail_in != 0)
				{
					res = deflate(&strm, Z_NO_FLUSH);
					if (res != Z_OK)
						throw CExcBase("deflate error {0}", res);

					if (strm.avail_out == 0)
					{
						pDst->Write(&outBuf[0], outBuf.size());

						strm.next_out = (Bytef*)&outBuf[0];
						strm.avail_out = (uInt)outBuf.size();
					}
				}
			}

			res = Z_OK;
			while (res == Z_OK)
			{
				if (strm.avail_out == 0)
				{
					pDst->Write(&outBuf[0], outBuf.size());

					strm.next_out = (Bytef*)&outBuf[0];
					strm.avail_out = (uInt)outBuf.size();
				}
				res = deflate(&strm, Z_FINISH);
			}

			if (res != Z_STREAM_END)
				throw CExcBase("deflate error {0}", res);

			pDst->Write(&outBuf[0], outBuf.size() - strm.avail_out);
			int err = deflateEnd(&strm);
			if (err != Z_OK)
				throw CExcBase("deflateEnd error {0}", err);
		}
		catch (std::exception& exc)
		{
			CExcBase::RegenExc("failed to compress", exc);
		}
		
	}


	void CZLibUtil::DecompressData(io::TReadPtr pSrc, io::TWritePtr pDst)
	{
		try
		{
			if (pSrc.get() == nullptr || pDst.get() == nullptr)
				throw CExcBase("invalid params");

			std::vector<byte_t> outBuf(1024 * 1024);
			std::vector<byte_t> inBuf(1024 * 1024);

			z_stream strm;
			strm.zalloc = 0;
			strm.zfree = 0;
			strm.next_out = (Bytef*)&outBuf[0];
			strm.avail_out = (uInt)outBuf.size();

			int res = inflateInit(&strm);
			if (res != Z_OK)
				throw CExcBase("inflateInit error {0}", res);

			std::streamsize size;
			while ((size = pSrc->Read(&inBuf[0], inBuf.size())) != 0)
			{
				strm.next_in = (Bytef*)&inBuf[0];
				strm.avail_in = (uInt)size;

				while (strm.avail_in != 0)
				{
					res = inflate(&strm, Z_NO_FLUSH);
							 
					if (strm.avail_out == 0)
					{
						pDst->Write(&outBuf[0], outBuf.size());

						strm.next_out = (Bytef*)&outBuf[0];
						strm.avail_out = (uInt)outBuf.size();
					}
				}
			}
		
			while (res == Z_OK)
			{
				if (strm.avail_out == 0)
				{
					pDst->Write(&outBuf[0], outBuf.size());

					strm.next_out = (Bytef*)&outBuf[0];
					strm.avail_out = (uInt)outBuf.size();
				}
				res = inflate(&strm, Z_FINISH);
			}

			if (res != Z_STREAM_END)
				throw CExcBase("inflate error {0}", res);
				 
			pDst->Write(&outBuf[0], outBuf.size() - strm.avail_out);
			res = inflateEnd(&strm);
			if (res != Z_OK)
				throw CExcBase("inflateEnd error {0}", res);

		}
		catch (std::exception& exc)
		{
			CExcBase::RegenExc("failed to decompress", exc);
		}
	}

	void CZLibUtil::CompressData(ECompressLevel level, const byte_t* pData, size_t size, io::TWritePtr& pDst)
	{
		try
		{

		}
		catch (std::exception& exc)
		{
			CExcBase::RegenExc("failed to compress", exc);
		}
	}
}