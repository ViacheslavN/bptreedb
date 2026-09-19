#include "stdafx.h"
#include "filesystem.h"
#include "../exception/PosixExc.h"
#include "../utils/ObjHolder.h"
#include "../str/StringEncoding.h"

namespace CommonLib
{
	class CFindFileHandle : public TObjHolder<int>
	{
	public:
		CFindFileHandle(int handle) :
			TObjHolder<int>(handle, [](int& h) {
			if (-1 != h)
			{
				close(h);
				h = -1;
			}
		})
		{

		}

	};

	bool CFileUtils::IsFileExist(const astr& path)
	{
		return access(path.c_str(), F_OK) != -1;
	}

	bool CFileUtils::IsFileExist(const wstr& path)
	{
		return IsFileExist(StringEncoding::str_w2utf8_safe(path));
	}

	void CFileUtils::RenameFile(const astr& oldFile, const astr& newFile)
	{
		if(rename(oldFile.c_str(), newFile.c_str()) != 0)
			throw CPosixExc(errno, "Failed to rename file {0}->{1}", oldFile, newFile);
	}

	void CFileUtils::RenameFile(const wstr& oldFile, const wstr& newFile)
	{
		RenameFile(StringEncoding::str_w2utf8_safe(oldFile), StringEncoding::str_w2utf8_safe(newFile));
	}
 
	void  CFileUtils::DelFile(const astr& path)
	{
		if (remove(path.c_str()) != 0)
			throw CPosixExc(errno, "Failed to delete file {0}", path);
	}

	void CFileUtils::DelFile(const wstr& path)
	{
        DelFile(StringEncoding::str_w2utf8_safe(path));
	}

	void CFileUtils::FileDelFolder(const astr& path)
	{
		if (rmdir(path.c_str()) != 0)
			throw CPosixExc(errno, "Failed to delete dir {0}", path);
	}

	void CFileUtils::FileDelFolder(const wstr& path)
	{
		FileDelFolder(StringEncoding::str_w2utf8_safe(path));
	}
	

	class CDirHandle
	{
	public:
		CDirHandle(const astr& path)
		{
			m_dir = opendir(path.c_str());
		}

		~CDirHandle()
		{
			if(m_dir != nullptr)
				closedir(m_dir);
		}

		DIR* GetDirHandle() const
		{
			return m_dir;
		}
	private:

		DIR* m_dir{nullptr};
	};




	void CFileUtils::FileSearch(const astr& searchMask, const TCheckFileObj& addDir, const TCheckFileObj& addFile)
	{


		CDirHandle dir(searchMask);

		if (dir.GetDirHandle() == nullptr)
			return;

		dirent* pEntry;

		while (pEntry = readdir(dir.GetDirHandle())) 
		{
			if(strcmp(pEntry->d_name, "."))
				continue;
			if (strcmp(pEntry->d_name, ".."))
				continue;

			if (DT_DIR & pEntry->d_type ) 
			{
				if (!addDir(pEntry->d_name))
					break;

			}
			else
			{
				if (!addFile(pEntry->d_name))
					break;
			}
		}

	}


	void CFileUtils::FileSearch(const astr& searchMask, astrvec& folders, astrvec& files)
	{

		TCheckFileObj addDir = [&folders](const astr& dir)-> bool
		{
			folders.push_back(dir);
			return true;
		};

		TCheckFileObj addFile = [&files](const astr& file)-> bool
		{
			files.push_back(file);
			return true;
		};

		FileSearch(searchMask, addDir, addFile);
	}
	
}