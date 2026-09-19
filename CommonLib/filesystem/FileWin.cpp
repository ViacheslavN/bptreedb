#include "stdafx.h"

#include "FileWin.h"
#include "../exception/WinExc.h"
#include "../str/StringEncoding.h"

namespace CommonLib
{
	namespace file
	{
		namespace win
		{
			CFileWin32::CFileWin32()
			{

			}

			CFileWin32::~CFileWin32()
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


			void CFileWin32::CloseFile()
			{
				if (!IsValid())
					throw CExcBase(L"Invalid file handle");

				if (m_Attach)
					throw CExcBase(L"file has been attached");

				BOOL bRet = ::CloseHandle(m_FileHandle);
				m_FileHandle = INVALID_HANDLE_VALUE;

				if (bRet != TRUE)
					throw CWinExc(L"Error close file");
			}

			bool CFileWin32::IsValid() const
			{
				return m_FileHandle != INVALID_HANDLE_VALUE;
			}


			void CFileWin32::Attach(FileHandle handle)
			{
				if (!m_Attach && IsValid())
					CloseFile();

				m_FileHandle = handle;
				m_Attach = true;
			}

			FileHandle CFileWin32::Deattach()
			{
				FileHandle handle = m_FileHandle;
				m_FileHandle = INVALID_HANDLE_VALUE;
				m_Attach = false;
				return handle;
			}

			FileHandle CFileWin32::Handle() const
			{
				return m_FileHandle;
			}

			void CFileWin32::OpenFileA(const char *pszFileNameUtf8, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType)
			{
                std::wstring fileName = StringEncoding::str_utf82w_safe(pszFileNameUtf8);
				return OpenFile(fileName.c_str(), mode, access, share, openType);
			}

			void CFileWin32::OpenFile(const wchar_t *pszFileName, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType)
			{
				OpenFileWin(pszFileName, mode, access, share, openType, NULL, NULL, NULL);
			}

			void CFileWin32::OpenFileWin(const wchar_t *pszFileName, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
			{
				DWORD win_access = 0, win_share = 0, win_mode = 0;
				if (access & arExecute)
					win_access |= GENERIC_EXECUTE;

				if (access & arRead)
					win_access |= GENERIC_READ;

				if (access & arWrite)
					win_access |= GENERIC_WRITE;

				if (share & smDelete)
					win_share |= FILE_SHARE_DELETE;

				if (share & smRead)
					win_share |= FILE_SHARE_READ;

				if (share & smWrite)
					win_share |= FILE_SHARE_WRITE;

				switch (mode)
				{
				case file::ofmCreateAlways:
					win_mode = CREATE_ALWAYS;
					break;
				case file::ofmCreateNew:
					win_mode = CREATE_NEW;
					break;
				case file::ofmOpenAlways:
					win_mode = OPEN_ALWAYS;
					break;
				case file::ofmOpenExisting:
					win_mode = OPEN_EXISTING;
					break;
				default:
					throw CExcBase(L"Error open file {0}, Wrong OpenFileType {1}", pszFileName, (int)mode);
					break;
				}

				m_FileHandle = ::CreateFile(pszFileName, win_access, win_share, lpSecurityAttributes, win_mode, dwFlagsAndAttributes, hTemplateFile);
				if (!IsValid())
					throw CWinExc(L"Error open file {0}", pszFileName);
			}

			uint64_t CFileWin32::GetFileSize() const
			{
				LARGE_INTEGER fileSize;
				BOOL bRet = ::GetFileSizeEx(m_FileHandle, &fileSize);
				if (bRet == FALSE)
					throw CWinExc(L"Error get file size");

				return (uint64_t)fileSize.QuadPart;
			}

			void CFileWin32::SetFilePos64(uint64_t nPos, enSeekOffset offset)
			{
				LARGE_INTEGER lpos;
				lpos.QuadPart = nPos;
				BOOL bRet = ::SetFilePointerEx(m_FileHandle, lpos, 0, offset == soFromBegin ? FILE_BEGIN : offset == soFromCurrent ? FILE_CURRENT
					: FILE_END);

				if (bRet == FALSE)
					throw CWinExc(L"Error set file pos, position {0}, type {1}", nPos, (int)offset);
			}

			void CFileWin32::SetFilePos(uint32_t nPos, enSeekOffset offset)
			{
				SetFilePos64(uint64_t(nPos), offset);
			}

			uint64_t CFileWin32::GetFilePos() const
			{
				LARGE_INTEGER lpos;
				lpos.QuadPart = 0;
				LARGE_INTEGER retPos;
				BOOL bRet = ::SetFilePointerEx(m_FileHandle, lpos, &retPos, FILE_CURRENT);
				if (bRet == FALSE)
					throw CWinExc(L"Error get file pos");

				return retPos.QuadPart;
			}

			void CFileWin32::SetFileEnd()
			{
				DWORD bRet = SetFilePointer(m_FileHandle, 0, NULL, FILE_END);
				if (bRet == INVALID_SET_FILE_POINTER)
					throw CWinExc(L"Error set end file");
			}

			void CFileWin32::SetFileBegin()
			{
				try
				{
					SetFilePos64(0, soFromBegin);
				}
				catch (CExcBase& exc)
				{
					exc.AddMsg(L"Error set file begin");
					throw;
				}
			}

			std::streamsize CFileWin32::Write(const byte_t* pData, size_t nSize)
			{
				DWORD dww = 0;
				BOOL bRet = ::WriteFile(m_FileHandle, pData, (DWORD)nSize, &dww, NULL);
				if (bRet == FALSE)
					throw CWinExc(L"Error write file, data size {0}", nSize);

				return dww;
			}

			std::streamsize CFileWin32::Read(byte_t* pData, size_t nSize)
			{
				DWORD dwr = 0;
				BOOL bRet = ::ReadFile(m_FileHandle, pData, (DWORD)nSize, &dwr, NULL);
				if (bRet == FALSE)
					throw CWinExc(L"Error read file, data size {0}", nSize);

				return dwr;
			}

			void CFileWin32::Flush()
			{
				BOOL bRet = ::FlushFileBuffers(m_FileHandle);
				if (bRet == FALSE)
					throw CWinExc(L"Error flush file");
			}

		}

	}
}