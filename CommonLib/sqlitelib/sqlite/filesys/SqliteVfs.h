#pragma once

#include "../db/sqlite3.h"


namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{
			class CVfsIO;
		}
	}
}

struct sqlite3En_file
{
	sqlite3_file base;
	sqlite3_file *pFile;
	sqlite3_int64 offset;
	CommonLib::database::sqlite::CVfsIO *pFileSystem;
	const char* zFileName;        
	int openFlags;              
};

struct sqlite3En_vfs
{
	sqlite3_vfs base;
};

#define REALVFS(p) ((sqlite3_vfs*)(((sqlite3En_vfs*)(p))->base.pAppData))
#define REALFILE(p) (((sqlite3En_file*)(p))->pFile)

#define ORIGVFS(p)  ((sqlite3_vfs*)((p)->pAppData))

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{

			class CVfs
			{
			public:

				static int VfsCreate(const char* zVfsReal, int makeDefault);


				static int VfsOpen(sqlite3_vfs* pVfs, const char* zName, sqlite3_file* pFile, int flags, int* pOutFlags);
				static int VfsDelete(sqlite3_vfs* pVfs, const char* zName, int syncDir);
				static int VfsAccess(sqlite3_vfs* pVfs, const char* zName, int flags, int* pResOut);
				static int VfsFullPathname(sqlite3_vfs* pVfs, const char* zName, int nOut, char* zOut);
				static void* VfsDlOpen(sqlite3_vfs* pVfs, const char* zFilename);
				static void VfsDlError(sqlite3_vfs* pVfs, int nByte, char* zErrMsg);
				static void(*VfsDlSym(sqlite3_vfs* pVfs, void* p, const char* zSymbol))(void)
				{
					return REALVFS(pVfs)->xDlSym(REALVFS(pVfs), p, zSymbol);
				}
				static void VfsDlClose(sqlite3_vfs* pVfs, void* p);
				static int VfsRandomness(sqlite3_vfs* pVfs, int nByte, char* zOut);
				static int VfsSleep(sqlite3_vfs* pVfs, int microseconds);
				static int VfsCurrentTime(sqlite3_vfs* pVfs, double* pOut);
				static int VfsGetLastError(sqlite3_vfs* pVfs, int nErr, char* zOut);
				static int VfsCurrentTimeInt64(sqlite3_vfs* pVfs, sqlite3_int64* pOut);
				static int VfsSetSystemCall(sqlite3_vfs* pVfs, const char* zName, sqlite3_syscall_ptr pNewFunc);
				static sqlite3_syscall_ptr VfsGetSystemCall(sqlite3_vfs* pVfs, const char* zName);
				static const char* VfsNextSystemCall(sqlite3_vfs* pVfs, const char* zName);
			};			
		}
	}
}
