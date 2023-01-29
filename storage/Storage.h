#pragma once
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/perf/PerfCount.h"

namespace bptreedb
{
	namespace storage
	{
		class IStorageCipher
		{
		public:
			IStorageCipher() {}
			virtual ~IStorageCipher() {}

			virtual uint32_t Encrypt(int64_t nPageAddr, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize) = 0;
			virtual uint32_t Decrypt(int64_t nPageAddr, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize) = 0;
			virtual uint32_t Encrypt(int64_t nPageAddr, byte_t* srcBuf, uint32_t bufSize) = 0;
			virtual uint32_t Decrypt(int64_t nPageAddr, byte_t* srcBuf, uint32_t bufSize) = 0;
		};

		class IStorage
		{
		public:
			enum EPrefOperation
			{
				eReadData,
				eWriteData,
				eEncryptData,
				eDecryptData
			};

			IStorage() {}
			virtual ~IStorage() {}

			virtual int32_t GetStorageId() const = 0;
			virtual void Open(const char* pszNameUtf8, bool bCreate, uint32_t nMinPageSize = 8192) = 0;
			virtual void Open(const wchar_t* pszName, bool bCreate, uint32_t nMinPageSize = 8192) = 0;
			virtual void Close() = 0;
			virtual void ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize) = 0;
			virtual void WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize) = 0;
			virtual void Flush() = 0;
			virtual void SetStoragePerformer(CommonLib::TPrefCounterPtr pStoragePerformer) = 0;
			virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0) = 0;
		};

		typedef std::shared_ptr<IStorage> IStoragePtr;
		typedef std::shared_ptr<IStorageCipher> IStorageCipherPtr;
	}
}