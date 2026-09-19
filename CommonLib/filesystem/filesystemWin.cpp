#include "stdafx.h"
#include "filesystem.h"
#include "../exception/WinExc.h"
#include "../str/StringEncoding.h"

namespace CommonLib
{





	bool CFileUtils::IsFileExist(const std::wstring& path)
	{
		DWORD dwAttr = ::GetFileAttributes(path.c_str());
		return ((dwAttr != INVALID_FILE_ATTRIBUTES) && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
	}

	void CFileUtils::DelFile(const std::wstring& path)
	{
		if (::DeleteFile(path.c_str()) == FALSE)
			throw CWinExc("Failed to delete file {0}", StringEncoding::str_w2a_safe(path));

	}

    void CFileUtils::DelFile(const std::string& path)
    {
        if (::DeleteFileA(path.c_str()) == FALSE)
            throw CWinExc("Failed to delete file {0}", path);

    }

	void CFileUtils::FileDelFolder(const std::wstring& path)
	{
		if (::RemoveDirectoryW(path.c_str()) == FALSE)
			throw CWinExc("Failed to delete folder {0}", StringEncoding::str_w2a_safe(path));
	}

    void CFileUtils::FileDelFolder(const std::string& path)
    {
        if (::RemoveDirectoryA(path.c_str()) == FALSE)
            throw CWinExc("Failed to delete folder {0}", path);
    }

	void CFileUtils::RenameFile(const std::wstring& oldFile, const std::wstring& newFile)
	{
		if(::MoveFileW(oldFile.c_str(), newFile.c_str()) == FALSE)
			throw CWinExc("Failed rename file {0}->{1}", StringEncoding::str_w2a_safe(oldFile), StringEncoding::str_w2a_safe(newFile));
	}



	void CFileUtils::FileSearch(const std::wstring& searchMask, const TCheckFileObj& addDir, const TCheckFileObj& addFile)
	{
		WIN32_FIND_DATAW  searchData = { 0 };
		HANDLE  hSearch = FindFirstFileW(searchMask.c_str(), &searchData);
		if (INVALID_HANDLE_VALUE == hSearch)
			return;

		CFindFileHandle  findHandle(hSearch);
		do
		{
            std::wstring nextChild = searchData.cFileName;
			if (_wcsicmp(nextChild.c_str(), L".") == 0)
				continue;

			if (_wcsicmp(nextChild.c_str(), L"..") == 0)
				continue;

			if ((searchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				if (!addDir(nextChild))
					break;
			}
			else
			{
				if(!addFile(nextChild))
					break;
			}

		} while ( FindNextFile(findHandle.Get(), &searchData) != 0);

	}


	void CFileUtils::FileSearch(const std::wstring& searchMask, std::vector<std::wstring>& folders, std::vector<std::wstring>& files)
	{

		TCheckFileObj addDir = [&folders](const std::wstring& dir)-> bool
		{
			folders.push_back(dir);
			return true;
		};

		TCheckFileObj addFile = [&files](const std::wstring& file)-> bool
		{
			files.push_back(file);
			return true;
		};

		FileSearch(searchMask, addDir, addFile);
	}


	bool CFileUtils::IsFileExist(const std::string& path)
	{
		return IsFileExist(StringEncoding::str_a2w_safe(path));
	}

	void CFileUtils::RenameFile(const std::string& oldFile, const std::string& newFile)
	{
		RenameFile(StringEncoding::str_a2w_safe(oldFile), StringEncoding::str_a2w_safe(newFile));
	}

    void CFileUtils::CreateDirectory(const std::string& path)
    {
        if (::CreateDirectoryA(path.c_str(), NULL) == FALSE)
            throw CWinExc("Failed to create directory {0}", path);
    }
}