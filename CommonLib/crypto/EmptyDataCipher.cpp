#include "stdafx.h"
#include "EmptyDataCipher.h"

namespace CommonLib
{
	namespace crypto
	{
		CEmptyDataCipher::CEmptyDataCipher()
		{

		}

		CEmptyDataCipher::~CEmptyDataCipher()
		{

		}
		
		uint32_t CEmptyDataCipher::GetBufferSize(uint32_t bufDataSize) const
		{
			return bufDataSize;
		}

		uint32_t CEmptyDataCipher::Encrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
		{
			memcpy(dstBuf, srcBuf, bufSize);
			return bufSize;
		}

		uint32_t CEmptyDataCipher::Decrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
		{
			memcpy(dstBuf, srcBuf, bufSize);
			return bufSize;
		}

		uint32_t CEmptyDataCipher::Encrypt( byte_t* srcBuf, uint32_t bufSize)
		{
			return bufSize;
		}

		uint32_t CEmptyDataCipher::Decrypt( byte_t* srcBuf, uint32_t bufSize)
		{
			return bufSize;
		}


		CEmptyXTSDataCipher::CEmptyXTSDataCipher()
		{}

		CEmptyXTSDataCipher::~CEmptyXTSDataCipher()
		{}

		uint32_t CEmptyXTSDataCipher::Encrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
		{
			memcpy(dstBuf, srcBuf, bufSize);
			return bufSize;
		}

		uint32_t CEmptyXTSDataCipher::Decrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
		{
			memcpy(dstBuf, srcBuf, bufSize);
			return bufSize;
		}

		uint32_t CEmptyXTSDataCipher::Encrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t bufSize)
		{
			return bufSize;
		}

		uint32_t CEmptyXTSDataCipher::Decrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t bufSize)
		{
			return bufSize;
		}


	}
}