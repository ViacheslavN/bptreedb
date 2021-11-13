#pragma once

#include "../../CommonLib/CommonLib.h"
#include "PageObject.h"

namespace bptreedb
{

	enum ePageFlags
	{
		eFP_NEW = 1,
		eFP_CHANGE = 2,
		eFP_REMOVE = 4,
		eFP_FROM_FREE = 8

	};

	class IFilePage
	{
	public:
		IFilePage() {}
		virtual ~IFilePage() {}

		virtual int64_t GetPageAddr() const = 0;
		virtual byte_t* GetData() = 0;
		virtual const byte_t* GetData() const = 0;
		virtual uint32_t GetPageSize() const = 0;
		virtual const byte_t* GetFullData() const = 0;
		virtual byte_t* GetFullData()  = 0;
		virtual uint32_t GetFullPageSize() const = 0;
		virtual uint32_t GetFlags() const = 0;
		virtual void SetFlag(ePageFlags nFlag, bool bSet) = 0;
		virtual bool IsNeedEncrypt() const = 0;
		virtual void SetNeedEncrypt(bool bEncrypt) = 0;
		virtual bool CheckCRC() const = 0;
		virtual void WriteCRC() = 0;
		virtual void SetAddr(int64_t nAddr) = 0;
		virtual void Erase(byte_t val = 0) = 0;

		virtual void ReloadHeader() = 0;
		virtual ObjectPageType GetObjectType() const = 0;
		virtual uint32_t GetObjectID() const = 0;
		virtual ObjectPageType GetParentType() const = 0;
		virtual uint32_t GetParentObjectID() const = 0;

		virtual byte_t* GetMetaData() = 0;
		virtual const byte_t* GetMetaData() const = 0;
		virtual uint32_t GetMetaDataSize() const = 0;
	};


	typedef std::shared_ptr<IFilePage> FilePagePtr;
	


	class IFreePageHolder
	{
	public:
		IFreePageHolder() {}
		virtual ~IFreePageHolder() {}

		virtual FilePagePtr GetPage(uint64_t nPageSize) = 0;
		virtual void AddPage(FilePagePtr& pPage) = 0;
		virtual void AddPage(int64_t nAddr, uint32_t nSize) = 0;

	};



	class IFilePages
	{
	public:
			IFilePages(){}
			virtual ~IFilePages(){}

			virtual FilePagePtr GetFilePage(int64_t nAddr, uint32_t nSize, bool decrypt) = 0;
			virtual void GetFilePage(FilePagePtr& pPage, int64_t nAddr, uint32_t nSize, bool decrypt) = 0;
			virtual void SaveFilePage(FilePagePtr& pPage) = 0;
			virtual void DropFilePage(int64_t nAddr) = 0;
			virtual FilePagePtr GetNewFilePage(uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype, uint32_t nSize = 0) = 0;
			virtual int64_t GetNewFilePageAddr(uint32_t nSize = 0) = 0;
			virtual FilePagePtr GetEmptyFilePage(int64_t nAddr, uint32_t nSize, uint32_t objectID, ObjectPageType objecttype, uint32_t parentID, ObjectPageType parenttype) = 0;
	};

	typedef std::shared_ptr<IFilePages> FilePagesPtr;

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
		virtual void Close() = 0;
		virtual void ReadData(int64_t nAddr, byte_t* pData, uint32_t nSize, bool decrypt) = 0;
		virtual void WriteData(int64_t nAddr, const byte_t* pData, uint32_t nSize, bool decrypt) = 0;	
		virtual void SetOffset(uint64_t offset) = 0;
		virtual void Flush() = 0;
		virtual void SetStoragePerformer(CommonLib::TPrefCounterPtr pStoragePerformer) = 0;	
	};

	typedef std::shared_ptr<IStorage> IStoragePtr;

}
