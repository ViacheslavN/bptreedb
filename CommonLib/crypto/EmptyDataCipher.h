#pragma once
#include "IDataCipher.h"

namespace CommonLib
{
	namespace crypto
	{
		class CEmptyDataCipher : public IDataCipher
		{
		public:
			CEmptyDataCipher();
			virtual ~CEmptyDataCipher();


			virtual uint32_t GetBufferSize(uint32_t bufDataSize) const;

			virtual uint32_t Encrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
			virtual uint32_t Decrypt(const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
			virtual uint32_t Encrypt(byte_t* srcBuf, uint32_t bufSize);
			virtual uint32_t Decrypt(byte_t* srcBuf, uint32_t bufSize);

		};

		class CEmptyXTSDataCipher : public IXTSDataCipher
		{
		public:
			CEmptyXTSDataCipher();
			virtual ~CEmptyXTSDataCipher();

			virtual uint32_t Encrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
			virtual uint32_t Decrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
			virtual uint32_t Encrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t bufSize);
			virtual uint32_t Decrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t bufSize);

		};
	}
}