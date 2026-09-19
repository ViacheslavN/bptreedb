#include "stdafx.h"

#include "FilePosix.h"
#include "../exception/PosixExc.h"
#include "../str/StringEncoding.h"


namespace CommonLib
{
	namespace file
	{
		namespace posix
		{

			CFilePosix::CFilePosix()
			{

			}

			CFilePosix::~CFilePosix()
			{
				try
				{
					CloseFile();
				}
				catch (std::exception& ex)
				{
					ex;
					//to to log
				}
			}

			void CFilePosix::Attach(FileHandle handle)
			{
				if (!m_Attach && IsValid())
					CloseFile();

				m_FileHandle = handle;
				m_Attach = true;
			}

			FileHandle CFilePosix::Deattach()
			{
				FileHandle handle = m_FileHandle;
				m_FileHandle = -1;
				m_Attach = false;
				return handle;
			}

			FileHandle CFilePosix::Handle() const
			{
				return m_FileHandle;
			}

			void CFilePosix::OpenFileA(const char *pszFileNameUtf8, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType)
			{

				mode_t pmode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;

				long file_flags = 0;
				if ((access & arRead) && (access & arWrite))
					file_flags = O_RDWR;
				else if (access & arRead)
					file_flags = O_RDONLY; 
				else if (access & arWrite)
					file_flags = O_WRONLY;

				if (mode == ofmCreateAlways)
				{
					file_flags |= O_CREAT | O_TRUNC;
				}
				else if (mode == ofmCreateNew || mode == ofmOpenAlways)
				{
					file_flags |= O_CREAT;
				}
			

				file_flags |= O_LARGEFILE;
						
				m_FileHandle = open(pszFileNameUtf8, file_flags, pmode);

				if (!IsValid())
					throw CPosixExc(errno, "Filed open file: {0}, ft: {1}, mode {2}", pszFileNameUtf8, file_flags, (int)pmode);

			}

			void CFilePosix::OpenFile(const wchar_t *pszFileName, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType)
			{
				OpenFileA(StringEncoding::str_w2utf8_safe(pszFileName).c_str(), mode, access, share, openType);
			}

			uint64_t CFilePosix::GetFileSize() const
			{
				struct stat64 st;
				int nRet = fstat64(m_FileHandle, &st);
				if(nRet == -1)
					throw CPosixExc("Filed to get size", errno);

				return st.st_size;
			}

			void CFilePosix::SetFilePos64(uint64_t nPos, enSeekOffset offset)
			{
				int64_t nRet = lseek64(m_FileHandle, nPos, offset == soFromBegin ? SEEK_SET : offset == soFromCurrent ? SEEK_CUR : SEEK_END);

				if(nRet == -1)
					throw CPosixExc(errno, "Filed to set file64 pos: {0}, offset: {1}", nPos, offset);
			}

			void CFilePosix::SetFilePos(uint32_t nPos, enSeekOffset offset)
			{
				int nRet = lseek(m_FileHandle, nPos, offset == soFromBegin ? SEEK_SET : offset == soFromCurrent ? SEEK_CUR
					: SEEK_END);
				throw CPosixExc(errno, "Filed to set file pos: {0}, offset: {1}", nPos, offset);
			}

			uint64_t CFilePosix::GetFilePos() const
			{
				int64_t nRet = lseek64(m_FileHandle, 0, SEEK_CUR);
				if (nRet == -1)
					throw CPosixExc("Filed to get file64 pos", errno);

                return nRet;
			}

			void CFilePosix::SetFileEnd()
			{
				SetFilePos(0, soFromEnd);
			}

			void CFilePosix::SetFileBegin()
			{
				SetFilePos(0, soFromBegin);
			}

			std::streamsize CFilePosix::Write(const byte_t* pData, size_t nSize)
			{
				ssize_t  nWriteBytes = 0;
				nWriteBytes = write(m_FileHandle, pData, nSize);
				if (nWriteBytes == -1)
					throw CPosixExc("Filed write file", errno);

				return nWriteBytes;
			}

			std::streamsize CFilePosix::Read(byte_t* pData, size_t nSize)
			{
				ssize_t nReadBytes = 0;
				nReadBytes = read(m_FileHandle, pData, nSize);
				if (nReadBytes == -1)
					throw CPosixExc("Filed read file", errno);

				return nReadBytes;
			}

			void CFilePosix::CloseFile()
			{
				if (!IsValid())
					throw CExcBase("Invalid file handle");

				if (m_Attach)
					throw CExcBase("file has been attached");

				int nRet = close(m_FileHandle);
				m_FileHandle = -1;

				if (nRet != 0)
					throw CPosixExc("Error close file", errno);
			}

			bool CFilePosix::IsValid() const
			{
				return m_FileHandle != -1;
			}

			void CFilePosix::Flush()
			{
				if(fsync(m_FileHandle) != 0)
					throw CPosixExc("Error flush file", errno);
			}

		}
	}
}