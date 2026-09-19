#pragma once
#include "../db/sqlite3.h"

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{
			class CVfsProxyIO
			{
			public:
				static int IoClose(sqlite3_file* pFile);
				static int IoRead(sqlite3_file* pFile, void*, int iAmt, sqlite3_int64 iOfst);
				static int IoWrite(sqlite3_file* pFile, const void*, int iAmt, sqlite3_int64 iOfst);
				static int IoTruncate(sqlite3_file* pFile, sqlite3_int64 size);
				static int IoSync(sqlite3_file* pFile, int flags);
				static int IoFileSize(sqlite3_file* pFile, sqlite3_int64* pSize);
				static int IoLock(sqlite3_file* pFile, int lock);
				static int IoUnlock(sqlite3_file* pFile, int lock);
				static int IoCheckReservedLock(sqlite3_file* pFile, int *pResOut);
				static int IoFileControl(sqlite3_file* pFile, int op, void *pArg);
				static int IoSectorSize(sqlite3_file* pFile);
				static int IoDeviceCharacteristics(sqlite3_file* pFile);
				static int IoShmMap(sqlite3_file* pFile, int iPg, int pgsz, int map, void volatile** p);
				static int IoShmLock(sqlite3_file* pFile, int offset, int n, int flags);
				static void IoShmBarrier(sqlite3_file* pFile);
				static int IoShmUnmap(sqlite3_file* pFile, int deleteFlag);
				static int IoFetch(sqlite3_file* pFile, sqlite3_int64 iOfst, int iAmt, void** pp);
				static int IoUnfetch(sqlite3_file* pFile, sqlite3_int64 iOfst, void* p);
			};
		}
	}
}