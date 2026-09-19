#include "stdafx.h"
#include "SqliteVfs.h"
#include "VfsIO.h"
#include "VfsProxyIo.h"
#include "../db/sqlite3.h"
#include "../CryptoContextHolder.h"

#define SQLITE3EN_VFS_NAME "encriptedfilesystem"


static sqlite3_io_methods mcIoMethodsGlobal =
{
  3,															  /* iVersion */
  CommonLib::database::sqlite::CVfsProxyIO::IoClose,                  /* xClose */
  CommonLib::database::sqlite::CVfsProxyIO::IoRead,                   /* xRead */
  CommonLib::database::sqlite::CVfsProxyIO::IoWrite,                  /* xWrite */
  CommonLib::database::sqlite::CVfsProxyIO::IoTruncate,               /* xTruncate */
  CommonLib::database::sqlite::CVfsProxyIO::IoSync,                   /* xSync */
  CommonLib::database::sqlite::CVfsProxyIO::IoFileSize,               /* xFileSize */
  CommonLib::database::sqlite::CVfsProxyIO::IoLock,                   /* xLock */
  CommonLib::database::sqlite::CVfsProxyIO::IoUnlock,                 /* xUnlock */
  CommonLib::database::sqlite::CVfsProxyIO::IoCheckReservedLock,      /* xCheckReservedLock */
  CommonLib::database::sqlite::CVfsProxyIO::IoFileControl,            /* xFileControl */
  CommonLib::database::sqlite::CVfsProxyIO::IoSectorSize,             /* xSectorSize */
  CommonLib::database::sqlite::CVfsProxyIO::IoDeviceCharacteristics,  /* xDeviceCharacteristics */
  CommonLib::database::sqlite::CVfsProxyIO::IoShmMap,                 /* xShmMap */
  CommonLib::database::sqlite::CVfsProxyIO::IoShmLock,                /* xShmLock */
  CommonLib::database::sqlite::CVfsProxyIO::IoShmBarrier,             /* xShmBarrier */
  CommonLib::database::sqlite::CVfsProxyIO::IoShmUnmap,               /* xShmUnmap */
  CommonLib::database::sqlite::CVfsProxyIO::IoFetch,                  /* xFetch */
  CommonLib::database::sqlite::CVfsProxyIO::IoUnfetch,                /* xUnfetch */
};


namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{

			int CVfs::VfsCreate(const char* zVfsReal, int makeDefault)
			{

				sqlite3_vfs *pvfs = sqlite3_vfs_find(SQLITE3EN_VFS_NAME);
				if (pvfs != nullptr)
				{
					return SQLITE_OK;
				}

				static sqlite3_vfs mcVfsTemplate =
				{
				  3,                      /* iVersion */
				  0,                      /* szOsFile */
				  1024,                   /* mxPathname */
				  0,                      /* pNext */
				  0,                      /* zName */
				  0,                      /* pAppData */
				  CVfs::VfsOpen,              /* xOpen */
				  CVfs::VfsDelete,            /* xDelete */
				  CVfs::VfsAccess,            /* xAccess */
				  CVfs::VfsFullPathname,      /* xFullPathname */
			  #ifndef SQLITE_OMIT_LOAD_EXTENSION
				  CVfs::VfsDlOpen,            /* xDlOpen */
				  CVfs::VfsDlError,           /* xDlError */
				  CVfs::VfsDlSym,             /* xDlSym */
				  CVfs::VfsDlClose,           /* xDlClose */
			  #else
				  0, 0, 0, 0,
			  #endif
				  CVfs::VfsRandomness,        /* xRandomness */
				  CVfs::VfsSleep,             /* xSleep */
				  CVfs::VfsCurrentTime,       /* xCurrentTime */
				  CVfs::VfsGetLastError,      /* xGetLastError */
				  CVfs::VfsCurrentTimeInt64,  /* xCurrentTimeInt64 */
				  CVfs::VfsSetSystemCall,     /* xSetSystemCall */
				  CVfs::VfsGetSystemCall,     /* xGetSystemCall */
				  CVfs::VfsNextSystemCall     /* xNextSystemCall */
				};

				sqlite3En_vfs* pVfsNew = 0;  /* Newly allocated VFS */
				sqlite3_vfs* pVfsReal = sqlite3_vfs_find(zVfsReal); /* Real VFS */
				int rc;

				if (pVfsReal)
				{
					size_t nPrefix = strlen(SQLITE3EN_VFS_NAME);
					size_t nRealName = 0;// strlen(pVfsReal->zName);
					size_t nName = nPrefix + nRealName + 1;
					size_t nByte = sizeof(sqlite3En_vfs) + nName + 1;
					pVfsNew = (sqlite3En_vfs*)sqlite3_malloc64(nByte);
					if (pVfsNew)
					{
						char* zSpace = (char*)&pVfsNew[1];
						memset(pVfsNew, 0, nByte);
						memcpy(&pVfsNew->base, &mcVfsTemplate, sizeof(sqlite3_vfs));
						pVfsNew->base.iVersion = pVfsReal->iVersion;
						pVfsNew->base.pAppData = pVfsReal;
						pVfsNew->base.mxPathname = pVfsReal->mxPathname;
						pVfsNew->base.szOsFile = sizeof(sqlite3En_file) + pVfsReal->szOsFile;

						/* Set name of new VFS as combination of the multiple ciphers prefix and the name of the underlying VFS */
						pVfsNew->base.zName = (const char*)zSpace;
						memcpy(zSpace, SQLITE3EN_VFS_NAME, nPrefix);
					//	memcpy(zSpace + nPrefix, "-", 1);
					//	memcpy(zSpace + nPrefix + 1, pVfsReal->zName, nRealName);


						rc = sqlite3_vfs_register(&pVfsNew->base, makeDefault);
						if (rc != SQLITE_OK)
						{
							sqlite3_free(pVfsNew);
						}
					}
					else
					{
						/* New VFS could not be allocated */
						rc = SQLITE_NOMEM;
					}
				}
				else
				{
					/* Underlying VFS not found */
					rc = SQLITE_NOTFOUND;
				}
				return rc;
			}


			int CVfs::VfsOpen(sqlite3_vfs* pVfs, const char* zName, sqlite3_file* pFile, int flags, int* pOutFlags)
			{
				int rc;
				sqlite3En_vfs* enVfs = (sqlite3En_vfs*)pVfs;
				sqlite3En_file* enFile = (sqlite3En_file*)pFile;
				enFile->pFile = (sqlite3_file*)&enFile[1];
				enFile->openFlags = flags;
				enFile->zFileName = zName;
				enFile->offset = 0;

				rc = REALVFS(pVfs)->xOpen(REALVFS(pVfs), zName, enFile->pFile, flags, pOutFlags);

				if (rc == SQLITE_OK)
				{
					const char* dbFileName = sqlite3_filename_database(zName);
					pFile->pMethods = &mcIoMethodsGlobal;
					ICryptoContextPtr ptrCryptoContext = CCryptoContextHolder::Instance().GetCryptoContext(dbFileName);
					enFile->pFileSystem = new CVfsIO(ptrCryptoContext);

					rc = enFile->pFileSystem->ValidatePwd(enFile);
					if (rc != SQLITE_OK)
					{
						pFile->pMethods->xClose(pFile);
						return rc;
					}


				}
				return rc;
			}

			int CVfs::VfsDelete(sqlite3_vfs* pVfs, const char* zName, int syncDir)
			{
				return REALVFS(pVfs)->xDelete(REALVFS(pVfs), zName, syncDir);
			}

			int  CVfs::VfsAccess(sqlite3_vfs* pVfs, const char* zName, int flags, int* pResOut)
			{
				return REALVFS(pVfs)->xAccess(REALVFS(pVfs), zName, flags, pResOut);
			}

			int CVfs::VfsFullPathname(sqlite3_vfs* pVfs, const char* zName, int nOut, char* zOut)
			{
				return REALVFS(pVfs)->xFullPathname(REALVFS(pVfs), zName, nOut, zOut);
			}

			void* CVfs::VfsDlOpen(sqlite3_vfs* pVfs, const char* zFilename)
			{
				return REALVFS(pVfs)->xDlOpen(REALVFS(pVfs), zFilename);
			}

			void CVfs::VfsDlError(sqlite3_vfs* pVfs, int nByte, char* zErrMsg)
			{
				REALVFS(pVfs)->xDlError(REALVFS(pVfs), nByte, zErrMsg);
			}

			void CVfs::VfsDlClose(sqlite3_vfs* pVfs, void* p)
			{
				REALVFS(pVfs)->xDlClose(REALVFS(pVfs), p);
			}

			int CVfs::VfsRandomness(sqlite3_vfs* pVfs, int nByte, char* zOut)
			{
				return REALVFS(pVfs)->xRandomness(REALVFS(pVfs), nByte, zOut);
			}

			int CVfs::VfsSleep(sqlite3_vfs* pVfs, int microseconds)
			{
				return REALVFS(pVfs)->xSleep(REALVFS(pVfs), microseconds);
			}

			int CVfs::VfsCurrentTime(sqlite3_vfs* pVfs, double* pOut)
			{
				return REALVFS(pVfs)->xCurrentTime(REALVFS(pVfs), pOut);
			}

			int CVfs::VfsGetLastError(sqlite3_vfs* pVfs, int code, char* zOut)
			{
				return REALVFS(pVfs)->xGetLastError(REALVFS(pVfs), code, zOut);
			}

			int CVfs::VfsCurrentTimeInt64(sqlite3_vfs* pVfs, sqlite3_int64* pOut)
			{
				return REALVFS(pVfs)->xCurrentTimeInt64(REALVFS(pVfs), pOut);
			}

			int CVfs::VfsSetSystemCall(sqlite3_vfs* pVfs, const char* zName, sqlite3_syscall_ptr pNewFunc)
			{
				return REALVFS(pVfs)->xSetSystemCall(REALVFS(pVfs), zName, pNewFunc);
			}

			sqlite3_syscall_ptr CVfs::VfsGetSystemCall(sqlite3_vfs* pVfs, const char* zName)
			{
				return REALVFS(pVfs)->xGetSystemCall(REALVFS(pVfs), zName);
			}

			const char* CVfs::VfsNextSystemCall(sqlite3_vfs* pVfs, const char* zName)
			{
				return REALVFS(pVfs)->xNextSystemCall(REALVFS(pVfs), zName);
			}
		}
	}
}