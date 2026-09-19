#include "stdafx.h"
#include "SqliteVfs.h"
#include "VfsIO.h"
#include "VfsProxyIo.h"

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{
			int CVfsProxyIO::IoClose(sqlite3_file* pFile)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				int rc = p->pFileSystem->IoClose(p);

				delete p->pFileSystem;
				p->pFileSystem = 0;

				return rc;

			}

			int CVfsProxyIO::IoRead(sqlite3_file* pFile, void* pBuf, int iAmt, sqlite3_int64 iOfst)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoRead(p, pBuf, iAmt, iOfst);
			}

			int CVfsProxyIO::IoWrite(sqlite3_file* pFile, const void* pBuf, int iAmt, sqlite3_int64 iOfst)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoWrite(p, pBuf, iAmt, iOfst);
			}

			int CVfsProxyIO::IoTruncate(sqlite3_file* pFile, sqlite3_int64 size)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoTruncate(p, size);
			}

			int CVfsProxyIO::IoSync(sqlite3_file* pFile, int flags)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoSync(p, flags);
			}

			int CVfsProxyIO::IoFileSize(sqlite3_file* pFile, sqlite3_int64* pSize)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoFileSize(p, pSize);
			}

			int CVfsProxyIO::IoLock(sqlite3_file* pFile, int lock)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoLock(p, lock);
			}

			int CVfsProxyIO::IoUnlock(sqlite3_file* pFile, int lock)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoUnlock(p, lock);
			}

			int CVfsProxyIO::IoCheckReservedLock(sqlite3_file* pFile, int *pResOut)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoCheckReservedLock(p, pResOut);
			}

			int CVfsProxyIO::IoFileControl(sqlite3_file* pFile, int op, void *pArg)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoFileControl(p, op, pArg);
			}

			int CVfsProxyIO::IoSectorSize(sqlite3_file* pFile)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoSectorSize(p);
			}

			int CVfsProxyIO::IoDeviceCharacteristics(sqlite3_file* pFile)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoDeviceCharacteristics(p);
			}

			int CVfsProxyIO::IoShmMap(sqlite3_file* pFile, int iPg, int pgsz, int map, void volatile** p)
			{
				sqlite3En_file* pf = (sqlite3En_file*)pFile;
				return pf->pFileSystem->IoShmMap(pf, iPg, pgsz, map, p);
			}

			int CVfsProxyIO::IoShmLock(sqlite3_file* pFile, int offset, int n, int flags)
			{
				sqlite3En_file* pf = (sqlite3En_file*)pFile;
				return pf->pFileSystem->IoShmLock(pf, offset, n, flags);
			}

			void CVfsProxyIO::IoShmBarrier(sqlite3_file* pFile)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				p->pFileSystem->IoShmBarrier(p);
			}

			int CVfsProxyIO::IoShmUnmap(sqlite3_file* pFile, int deleteFlag)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoShmUnmap(p, deleteFlag);
			}

			int CVfsProxyIO::IoFetch(sqlite3_file* pFile, sqlite3_int64 iOfst, int iAmt, void** pp)
			{
				sqlite3En_file* p = (sqlite3En_file*)pFile;
				return p->pFileSystem->IoFetch(p, iOfst, iAmt, pp);
			}

			int CVfsProxyIO::IoUnfetch(sqlite3_file* pFile, sqlite3_int64 iOfst, void* p)
			{
				sqlite3En_file* pf = (sqlite3En_file*)pFile;
				return pf->pFileSystem->IoUnfetch(pf, iOfst, p);
			}
		}
	}
}