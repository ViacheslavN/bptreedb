#include "stdafx.h"
#include "VfsIO.h"
#include "SqliteVfs.h"
#include "../db/sqlite3.h"
#include "../../../crypto/CryptoData.h"

#define INIT_PAGE_SIZE 8192
#define SQLITE_DEFAULT_PAGE_SIZE 4096
#define SQLITE_MAX_PAGE_SIZE 65536

//copy from sqlite3.c
#ifndef SQLITE_BYTEORDER
# if defined(i386)      || defined(__i386__)      || defined(_M_IX86) ||    \
     defined(__x86_64)  || defined(__x86_64__)    || defined(_M_X64)  ||    \
     defined(_M_AMD64)  || defined(_M_ARM)        || defined(__x86)   ||    \
     defined(__ARMEL__) || defined(__AARCH64EL__) || defined(_M_ARM64)
#   define SQLITE_BYTEORDER    1234
# elif defined(sparc)     || defined(__ppc__) || \
       defined(__ARMEB__) || defined(__AARCH64EB__)
#   define SQLITE_BYTEORDER    4321
# else
#   define SQLITE_BYTEORDER 0
# endif
#endif
#if SQLITE_BYTEORDER==4321
# define SQLITE_BIGENDIAN    1
# define SQLITE_LITTLEENDIAN 0
# define SQLITE_UTF16NATIVE  SQLITE_UTF16BE
#elif SQLITE_BYTEORDER==1234
# define SQLITE_BIGENDIAN    0
# define SQLITE_LITTLEENDIAN 1
# define SQLITE_UTF16NATIVE  SQLITE_UTF16LE
#else
# ifdef SQLITE_AMALGAMATION
const int sqlite3one = 1;
# else
extern const int sqlite3one;
# endif
# define SQLITE_BIGENDIAN    (*(char *)(&sqlite3one)==0)
# define SQLITE_LITTLEENDIAN (*(char *)(&sqlite3one)==1)
# define SQLITE_UTF16NATIVE  (SQLITE_BIGENDIAN?SQLITE_UTF16BE:SQLITE_UTF16LE)
#endif
/*
/*
** Read or write a four-byte big-endian integer value.
*/
 
uint32_t sqlite3Get4byte(const byte_t *p)
{
#if SQLITE_BYTEORDER==4321
	uint32_t x;
	memcpy(&x, p, 4);
	return x;
#elif SQLITE_BYTEORDER==1234 && GCC_VERSION>=4003000
	uint32_t x;
	memcpy(&x, p, 4);
	return __builtin_bswap32(x);
#elif SQLITE_BYTEORDER==1234
	uint32_t x;
	memcpy(&x, p, 4);
	return _byteswap_ulong(x);
#else
	testcase(p[0] & 0x80);
	return ((unsigned)p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
#endif
}


namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{
		
			CVfsIO::CVfsIO(ICryptoContextPtr ptrCryptoContext) : m_ptrCryptoContext(ptrCryptoContext)
			{
				m_buffer.resize(SQLITE_DEFAULT_PAGE_SIZE);
			}

			CVfsIO::~CVfsIO()
			{

			}

			int CVfsIO::ValidatePwd(sqlite3En_file *pFile)
			{
				sqlite3_int64 fileSize = 0;
				int retVal = SQLITE_OK;
				retVal = IoFileSize(pFile, &fileSize);
				if (retVal != SQLITE_OK)
					return retVal;

				if (fileSize == 0)
				{
					retVal = CreateInitPage(pFile);
					if (retVal != SQLITE_OK)
						return retVal;
				}
				else
				{
					retVal = CheckInitPage( pFile);
					if (retVal != SQLITE_OK)
						return retVal;
				}
				return retVal;
			}

			int CVfsIO::CreateInitPage(sqlite3En_file* pFile)
			{
				size_t size =  m_ptrCryptoContext->GetInitBlockSize();
				crypto::crypto_vector vecBuffer(size);
				m_ptrCryptoContext->CreateInitBlock(vecBuffer.data(), vecBuffer.size());
				m_ptrXTSDataCipher = m_ptrCryptoContext->GetXTSDataCipher();
				
				int rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), vecBuffer.data(), (int)vecBuffer.size(), 0);
				pFile->offset = size;

				return rc;
			}


			int CVfsIO::CheckInitPage(sqlite3En_file* pFile)
			{
				size_t size = m_ptrCryptoContext->GetInitBlockSize();
				crypto::crypto_vector vecBuffer(size);

				int rc = REALFILE(pFile)->pMethods->xRead(REALFILE(pFile), vecBuffer.data(), (int)vecBuffer.size(), 0);
				pFile->offset = size;

				if (!m_ptrCryptoContext->ValidateInitBlock(vecBuffer.data(), vecBuffer.size()))
					return SQLITE_AUTH;

				m_ptrXTSDataCipher = m_ptrCryptoContext->GetXTSDataCipher();
				
				return rc;
			}

			int CVfsIO::IoClose(sqlite3En_file *pFile)
			{
				return REALFILE(pFile)->pMethods->xClose(REALFILE(pFile));
			}

			int CVfsIO::IoRead(sqlite3En_file* pFile, void* pBuf, int iAmt, sqlite3_int64 iOfst)
			{				


				if (pFile->openFlags & SQLITE_OPEN_WAL)
				{
					return IoReadWal(pFile, pBuf, iAmt, iOfst);
				}

				const int pageOffset = iOfst % SQLITE_DEFAULT_PAGE_SIZE;
				const int deltaOffset = iAmt % SQLITE_DEFAULT_PAGE_SIZE;
				int rc = SQLITE_OK;
				if (pageOffset || deltaOffset)
				{
					const sqlite3_int64 prevOffset = iOfst - pageOffset;
					rc = REALFILE(pFile)->pMethods->xRead(REALFILE(pFile), m_buffer.data(), (int)m_buffer.size(), prevOffset + pFile->offset);
					if (rc == SQLITE_IOERR_SHORT_READ)
					{
						return rc;
					}

					sqlite3_int64 pageNo = (prevOffset / SQLITE_DEFAULT_PAGE_SIZE) + 1;
					m_ptrXTSDataCipher->Decrypt(pageNo, m_buffer.data(), (int)m_buffer.size());

					memcpy(pBuf, m_buffer.data() + deltaOffset, iAmt);

				}
				else
				{
					int rc = REALFILE(pFile)->pMethods->xRead(REALFILE(pFile), pBuf, iAmt, iOfst + pFile->offset);
					if (rc == SQLITE_IOERR_SHORT_READ)
					{
						return rc;
					}

					byte_t* data = (byte_t*)pBuf;
					int pageNo = (int)(iOfst / SQLITE_DEFAULT_PAGE_SIZE) + 1;
					int nPages = iAmt / SQLITE_DEFAULT_PAGE_SIZE;
					int iPage;
					for (iPage = 0; iPage < nPages; ++iPage)
					{
						m_ptrXTSDataCipher->Decrypt(pageNo, data, SQLITE_DEFAULT_PAGE_SIZE);
						data += SQLITE_DEFAULT_PAGE_SIZE;
						iAmt += SQLITE_DEFAULT_PAGE_SIZE;
						++pageNo;
					}
				}			 

				return rc;
			}

			int CVfsIO::IoReadWal(sqlite3En_file* pFile, void* buffer, int count, sqlite3_int64 offset)
			{
				int rc = REALFILE(pFile)->pMethods->xRead(REALFILE(pFile), (byte*)buffer, count, offset + pFile->offset);
				if (rc == SQLITE_IOERR_SHORT_READ)
				{
					return rc;
				}
				if (count == SQLITE_DEFAULT_PAGE_SIZE)
				{
					int pageNo = 0;
					byte_t ac[4];

					/*
					** Determine page number
					**
					** It is necessary to explicitly read the page number from the frame header.
					*/
					rc = REALFILE(pFile)->pMethods->xRead(REALFILE(pFile), ac, 4, (offset + pFile->offset) - walFrameHeaderSize);
					if (rc == SQLITE_OK)
					{
						pageNo = sqlite3Get4byte(ac);
					}

					/*
					** Decrypt page content if page number is valid
					*/
					if (pageNo != 0)
					{
						m_ptrXTSDataCipher->Decrypt(pageNo, (byte_t*)buffer, SQLITE_DEFAULT_PAGE_SIZE);
					}
				}
				else if (count == SQLITE_DEFAULT_PAGE_SIZE + walFrameHeaderSize)
				{
					int pageNo = sqlite3Get4byte((const byte_t*)buffer);

					/*
					** Decrypt page content if page number is valid
					*/
					if (pageNo != 0)
					{
						m_ptrXTSDataCipher->Decrypt(pageNo, (byte_t*)buffer + walFrameHeaderSize, SQLITE_DEFAULT_PAGE_SIZE);
					}
				}

				return rc;
			}

			int CVfsIO::IoWrite(sqlite3En_file* pFile, const void* pBuf, int iAmt, sqlite3_int64 iOfst)
			{

				if (pFile->openFlags & SQLITE_OPEN_WAL)
				{
					return IoWriteWal(pFile, pBuf, iAmt, iOfst);
				}

 
				const int deltaOffset = iOfst % SQLITE_DEFAULT_PAGE_SIZE;
				const int deltaCount = iAmt % SQLITE_DEFAULT_PAGE_SIZE;
				int rc = SQLITE_OK;

				if (deltaOffset || deltaCount)
				{
					rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), pBuf, iAmt, iOfst + pFile->offset);
				}
				else
				{
					/*
					** Write full page(s)
					**
					** In fact, SQLite writes only one database page at a time.
					** This would allow to remove the page loop below.
					*/
					byte_t* data = (byte_t*)pBuf;
					sqlite3_int64 pageNo = iOfst / SQLITE_DEFAULT_PAGE_SIZE + 1;
					int nPages = iAmt / SQLITE_DEFAULT_PAGE_SIZE;
					int iPage;
					for (iPage = 0; iPage < nPages; ++iPage)
					{
						m_ptrXTSDataCipher->Encrypt(pageNo, data, SQLITE_DEFAULT_PAGE_SIZE, m_buffer.data(), (uint32_t)m_buffer.size());
						rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), m_buffer.data(), SQLITE_DEFAULT_PAGE_SIZE, iOfst + pFile->offset);
						if (rc != SQLITE_OK)
							return rc;

						data += SQLITE_DEFAULT_PAGE_SIZE;
						iOfst += SQLITE_DEFAULT_PAGE_SIZE;
						++pageNo;
					}
				}

				return rc;
			}

			int  CVfsIO::IoWriteWal(sqlite3En_file* pFile, const void* buffer, int count, sqlite3_int64 offset)
			{
				int rc = SQLITE_OK;
				if (count == SQLITE_DEFAULT_PAGE_SIZE)
				{
					int pageNo = 0;
					byte_t ac[4];

					/*
					** Read the corresponding page number from the file
					**
					** In WAL mode SQLite does not write the page number of a page to file
					** immediately before writing the corresponding page content.
					** Page numbers and checksums are written to file independently.
					** Therefore it is necessary to explicitly read the page number
					** on writing to file the content of a page.
					*/
					rc = REALFILE(pFile)->pMethods->xRead(REALFILE(pFile), ac, 4,  (offset +pFile->offset) - walFrameHeaderSize);
					if (rc == SQLITE_OK)
					{
						pageNo = sqlite3Get4byte(ac);
					}

					if (pageNo != 0)
					{
						/*
						** Encrypt the page buffer, but only if the page number is valid
						*/
						m_ptrXTSDataCipher->Encrypt(pageNo, (const byte_t*)buffer, SQLITE_DEFAULT_PAGE_SIZE, m_buffer.data(), (uint32_t)m_buffer.size());
						rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), m_buffer.data(), (uint32_t)m_buffer.size(), offset + pFile->offset);
					}
					else
					{
						/*
						** Write buffer without encryption if the page number could not be determined
						*/
						rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), buffer, count, offset + pFile->offset);
					}
				}
				else if (count == SQLITE_DEFAULT_PAGE_SIZE + walFrameHeaderSize)
				{
					int pageNo = sqlite3Get4byte((const byte_t*)buffer);
					if (pageNo != 0)
					{
						/*
						** Encrypt the page buffer, but only if the page number is valid
						*/
						m_ptrXTSDataCipher->Encrypt(pageNo, (const byte_t*)buffer + walFrameHeaderSize, SQLITE_DEFAULT_PAGE_SIZE, m_buffer.data(), (uint32_t)m_buffer.size());
						rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), buffer, walFrameHeaderSize, offset + pFile->offset);
						rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), m_buffer.data(), SQLITE_DEFAULT_PAGE_SIZE, offset + pFile->offset + walFrameHeaderSize);
					}
					else
					{
						/*
						** Write buffer without encryption if the page number could not be determined
						*/
						rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), buffer, count, offset + pFile->offset);
					}
				}
				else
				{
					/*
					** Write buffer without encryption if it is not a database page
					*/
					rc = REALFILE(pFile)->pMethods->xWrite(REALFILE(pFile), buffer, count, offset + pFile->offset);
				}

				return rc;
			}
			
			int CVfsIO::IoTruncate(sqlite3En_file* pFile, sqlite3_int64 size)
			{
				return REALFILE(pFile)->pMethods->xTruncate(REALFILE(pFile), size + pFile->offset);
			}

			int CVfsIO::IoSync(sqlite3En_file* pFile, int flags)
			{
				return REALFILE(pFile)->pMethods->xSync(REALFILE(pFile), flags);
			}

			int CVfsIO::IoFileSize(sqlite3En_file* pFile, sqlite3_int64* pSize)
			{
				int retVal  =  REALFILE(pFile)->pMethods->xFileSize(REALFILE(pFile), pSize);
				if (retVal == SQLITE_OK)
				{
					*pSize -= pFile->offset;
				}

				return retVal;
			}

			int CVfsIO::IoLock(sqlite3En_file* pFile, int lock)
			{
				return REALFILE(pFile)->pMethods->xLock(REALFILE(pFile), lock);
			}

			int CVfsIO::IoUnlock(sqlite3En_file* pFile, int lock)
			{
				return REALFILE(pFile)->pMethods->xUnlock(REALFILE(pFile), lock);
			}

			int CVfsIO::IoCheckReservedLock(sqlite3En_file* pFile, int *pResOut)
			{
				return REALFILE(pFile)->pMethods->xCheckReservedLock(REALFILE(pFile), pResOut);
			}

			int CVfsIO::IoFileControl(sqlite3En_file* pFile, int op, void *pArg)
			{
				return REALFILE(pFile)->pMethods->xFileControl(REALFILE(pFile), op, pArg);
			}

			int CVfsIO::IoSectorSize(sqlite3En_file* pFile)
			{
				return REALFILE(pFile)->pMethods->xSectorSize(REALFILE(pFile));
			}

			int CVfsIO::IoDeviceCharacteristics(sqlite3En_file* pFile)
			{
				return REALFILE(pFile)->pMethods->xDeviceCharacteristics(REALFILE(pFile));
			}

			int CVfsIO::IoShmMap(sqlite3En_file* pFile, int iPg, int pgsz, int map, void volatile** p)
			{
				return REALFILE(pFile)->pMethods->xShmMap(REALFILE(pFile), iPg, pgsz, map, p);
			}

			int CVfsIO::IoShmLock(sqlite3En_file* pFile, int offset, int n, int flags)
			{
				return REALFILE(pFile)->pMethods->xShmLock(REALFILE(pFile), offset, n, flags);
			}

			void CVfsIO::IoShmBarrier(sqlite3En_file* pFile)
			{
				REALFILE(pFile)->pMethods->xShmBarrier(REALFILE(pFile));
			}

			int CVfsIO::IoShmUnmap(sqlite3En_file* pFile, int deleteFlag)
			{
				return REALFILE(pFile)->pMethods->xShmUnmap(REALFILE(pFile), deleteFlag);
			}

			int CVfsIO::IoFetch(sqlite3En_file* pFile, sqlite3_int64 iOfst, int iAmt, void** pp)
			{
				return REALFILE(pFile)->pMethods->xFetch(REALFILE(pFile), iOfst, iAmt, pp);
			}

			int CVfsIO::IoUnfetch(sqlite3En_file* pFile, sqlite3_int64 iOfst, void* p)
			{
				return REALFILE(pFile)->pMethods->xUnfetch(REALFILE(pFile), iOfst, p);
			}
		}
	}
}