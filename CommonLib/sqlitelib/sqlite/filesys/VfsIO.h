#pragma once
#include "../db/sqlite3.h"
#include "../Database.h"

struct sqlite3En_file;


namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{
			typedef std::shared_ptr<class CVfsIO> CVfsIOPtr;

			class CVfsIO
			{
			public:
				~CVfsIO();
				CVfsIO(ICryptoContextPtr ptrCryptoContext);

				int ValidatePwd(sqlite3En_file *pFile);

				int IoClose(sqlite3En_file *pFile);
				int IoRead(sqlite3En_file* pFile, void* pBuf, int iAmt, sqlite3_int64 iOfst);
				int IoWrite(sqlite3En_file* pFile, const void* pBuf, int iAmt, sqlite3_int64 iOfst);
				int IoTruncate(sqlite3En_file* pFile, sqlite3_int64 size);
				int IoSync(sqlite3En_file* pFile, int flags);
				int IoFileSize(sqlite3En_file* pFile, sqlite3_int64* pSize);
				int IoLock(sqlite3En_file* pFile, int lock);
				int IoUnlock(sqlite3En_file* pFile, int lock);
				int IoCheckReservedLock(sqlite3En_file* pFile, int *pResOut);
				int IoFileControl(sqlite3En_file* pFile, int op, void *pArg);
				int IoSectorSize(sqlite3En_file* pFile);
				int IoDeviceCharacteristics(sqlite3En_file* pFile);
				int IoShmMap(sqlite3En_file* pFile, int iPg, int pgsz, int map, void volatile** p);
				int IoShmLock(sqlite3En_file* pFile, int offset, int n, int flags);
				void IoShmBarrier(sqlite3En_file* pFile);
				int IoShmUnmap(sqlite3En_file* pFile, int deleteFlag);
				int IoFetch(sqlite3En_file* pFile, sqlite3_int64 iOfst, int iAmt, void** pp);
				int IoUnfetch(sqlite3En_file* pFile, sqlite3_int64 iOfst, void* p);
			private:
				int  CreateInitPage(sqlite3En_file* pFile);
				int  CheckInitPage(sqlite3En_file* pFile);
				int  IoWriteWal(sqlite3En_file* pFile, const void* pBuf, int iAmt, sqlite3_int64 iOfst);
				int  IoReadWal(sqlite3En_file* pFile, void* pBuf, int iAmt, sqlite3_int64 iOfst);
			private:

				static const int walFrameHeaderSize = 24;
				static const int walFileHeaderSize = 32;

				ICryptoContextPtr m_ptrCryptoContext;
				crypto::IXTSDataCipherPtr m_ptrXTSDataCipher;
				std::vector<byte_t> m_buffer;

			};
		}
	}
}