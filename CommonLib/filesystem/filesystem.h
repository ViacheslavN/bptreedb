#pragma once
#include "../utils/ObjHolder.h"


#ifdef _WIN32
	#include "FileWin.h"


#elif  __linux__
	#include "FilePosix.h"
#endif


namespace CommonLib
{

#ifdef _WIN32
	class CFindFileHandle : public CommonLib::TObjHolder<HANDLE>
	{
	public:
		CFindFileHandle(HANDLE handle) :
			TObjHolder<HANDLE>(handle, [](HANDLE& h) {
			if (INVALID_HANDLE_VALUE != h)
			{
				FindClose(h);
				h = INVALID_HANDLE_VALUE;
			}
			})
		{

		}	};
#endif
	class CFileUtils
	{
	public:
		static bool IsFileExist(const std::wstring& path);
		static void DelFile(const std::wstring& path);
		static void FileDelFolder(const std::wstring& path);
		static void RenameFile(const std::wstring& oldFile, const std::wstring& newFile);

#ifdef _WIN32

		typedef std::function<bool(const std::wstring&)> TCheckFileObj;
		static void FileSearch(const std::wstring& searchMask,  const TCheckFileObj& addDir, const TCheckFileObj& addFile);
		static void FileSearch(const std::wstring& searchMask, std::vector<std::wstring>& folders, std::vector<std::wstring>& files);
#elif  __linux__
		typedef std::function<bool(const astr&)> TCheckFileObj;
		static void FileSearch(const astr& searchMask, const TCheckFileObj& addDir, const TCheckFileObj& addFile);
		static void FileSearch(const astr& searchMask, astrvec& folders, astrvec& files);
#endif

		static bool IsFileExist(const std::string& path);
		static void DelFile(const std::string& path);
		static void FileDelFolder(const std::string& path);
		static void RenameFile(const std::string& oldFile, const std::string& newFile);
        static void CreateDirectory(const std::string& path);
	};
}