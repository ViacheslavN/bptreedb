#include "stdafx.h"
#include "File.h"

#ifdef _WIN32
	#include "FileWin.h"
#elif __linux__
	#include "FilePosix.h"
#endif



namespace CommonLib
{
	namespace file
	{

		TFilePtr CFileCreator::OpenFileA(const char *pszFileNameUtf8, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType)
		{
			TFilePtr pFile = std::shared_ptr<IFile>(new CFile());
			pFile->OpenFileA(pszFileNameUtf8, mode, access, share, openType);
			return pFile;
		}

		TFilePtr CFileCreator::OpenFile(const wchar_t *pszFileName,  enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType)
		{
			TFilePtr pFile = std::shared_ptr < IFile>(new CFile());
			pFile->OpenFile(pszFileName, mode, access, share, openType);
			return pFile;
		}

	}
}