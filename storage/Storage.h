#pragma once
#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/perf/PerfCount.h"
#include "../../CommonLib/stream/FixedMemoryStream.h"
#include "PageMemoryBuffer.h"

namespace bptreedb
{
	namespace storage
	{
		typedef std::shared_ptr<class IStorageIO> IStorageIOPtr;
		typedef std::shared_ptr<class IFilePage> IFilePagePtr;
		typedef std::shared_ptr<class IFilePager> IFilePagerPtr;

		class IFilePager
		{
		public:

			IFilePager() {}
			virtual ~IFilePager() {}

			virtual int64_t GetNewFilePageAddr() = 0;
			virtual IFilePagePtr GetNewPage(int64_t nAddr) = 0;
			virtual IFilePagePtr ReadPage(int64_t nAddr) = 0;
			virtual void ReadPage(IFilePagePtr ptrPage, int64_t nAddr) = 0;
			virtual void SavePage(IFilePagePtr ptrPage) = 0;
		};

		class IFilePage
		{
		public:

			IFilePage() {}
			virtual ~IFilePage() {}

			virtual int64_t GetAddr() const = 0;
			virtual void SetAddr(int64_t nAddr) = 0;
			virtual uint32_t GetSize() const = 0;
	
			virtual void Save(IStorageIOPtr ptrStorageIO, int64_t nAddr = -1) = 0;
			virtual void Read(IStorageIOPtr ptrStorageIO, int64_t nAddr = -1) = 0;

			virtual CommonLib::IMemoryWriteStreamPtr GetWriteStream() = 0;
			virtual CommonLib::IMemoryReadStreamPtr GetReadStream() const = 0;
		};

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

		class IStorageIO
		{
		public:
			IStorageIO() {}
			virtual ~IStorageIO() {}

			virtual uint32_t GetPageSize() const = 0;
			virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0) = 0;
			virtual void ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize) = 0;
			virtual void WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize) = 0;
			virtual void DropData(int64_t nAddr, uint32_t nSize) = 0;
			 
		};
		 
		class IStorage : public IStorageIO
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
			virtual void Open(const char* pszNameUtf8, bool bCreate, uint64_t offset, uint32_t nMinPageSize = 8192) = 0;
			virtual void Open(const wchar_t* pszName, bool bCreate, uint64_t offset, uint32_t nMinPageSize = 8192) = 0;
			virtual void Close() = 0;
			virtual void Flush() = 0;
			virtual void SetStoragePerformer(CommonLib::TPrefCounterPtr pStoragePerformer) = 0;
			virtual void Lock() = 0;
			virtual void UnLock() = 0;
			virtual bool TryLock() = 0;
		};

		typedef std::shared_ptr<IStorage> IStoragePtr;

		class IStoragesHolder
		{
		public:
			IStoragesHolder() {};
			virtual ~IStoragesHolder() {}

			virtual IStoragePtr GetStorage(int32_t nId) const = 0;
			virtual IStoragePtr GetStorageThrowIfNull(int32_t nId) const = 0;
			virtual void AddStorage(IStoragePtr ptrStorage) = 0;
			virtual void RemoveStorage(int32_t nId) = 0;

		};

		
		typedef std::shared_ptr<IStorageCipher> IStorageCipherPtr;
		typedef std::shared_ptr<IStoragesHolder> IStoragesHolderPtr;
	}
}