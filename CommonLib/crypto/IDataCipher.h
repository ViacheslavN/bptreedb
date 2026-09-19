#pragma once
 

namespace CommonLib
{
	namespace crypto
	{
		typedef std::shared_ptr<class IDataCipher> IDataCipherPtr;
		typedef std::shared_ptr<class IXTSDataCipher> IXTSDataCipherPtr;

		class IDataCipher
		{
		public:
			IDataCipher(){}
			virtual ~IDataCipher() {}

			virtual uint32_t GetBufferSize(uint32_t bufDataSize) const = 0;
			virtual uint32_t Encrypt(const byte_t* srcBuf, uint32_t srcSize, byte_t* dstBuf, uint32_t dstSize) = 0;
			virtual uint32_t Decrypt(const byte_t* srcBuf, uint32_t srcSize, byte_t* dstBuf, uint32_t dstSize) = 0;
			virtual uint32_t Encrypt(byte_t* srcBuf, uint32_t bufSize) = 0;
			virtual uint32_t Decrypt(byte_t* srcBuf, uint32_t bufSize) = 0;

		};


		class IXTSDataCipher
		{
		public:
			IXTSDataCipher() {}
			virtual ~IXTSDataCipher() {}

			virtual uint32_t Encrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize) = 0;
			virtual uint32_t Decrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize) = 0;
			virtual uint32_t Encrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t bufSize) = 0;
			virtual uint32_t Decrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t bufSize) = 0;
		};

	}
}