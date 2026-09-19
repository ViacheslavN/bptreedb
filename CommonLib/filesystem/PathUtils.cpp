#include "stdafx.h"
#include "PathUtils.h"
#include "../str/StrUtils.h"

namespace CommonLib
{

    template<typename TStr, typename  TChar>
    size_t FindLastPathDelimT(const TStr& path, TChar slash, TChar backSlash)
    {
        size_t slashPos = path.find_last_of(slash);
        size_t backSlashPos = path.find_last_of(backSlash);
        if ( TStr::npos == slashPos )
            return backSlashPos;

        if ( TStr::npos == backSlashPos )
            return slashPos;

        return  slashPos > backSlashPos ? slashPos : backSlashPos;
    }


    std::string CPathUtils::GetPathDelim(const std::string& path)
    {
#ifdef _WIN32
        if (std::string::npos != path.find_last_of( '\\' ) || std::string::npos == path.find_last_of('/'))
            return "\\";
#endif
        return "/";
    }

    std::wstring CPathUtils::GetPathDelim(const std::wstring& path)
    {
#ifdef _WIN32
        if (std::wstring::npos != path.find_last_of(L'\\') || std::wstring::npos == path.find_last_of(L'/'))
            return L"\\";
#endif
        return L"/";
    }

    size_t CPathUtils::FindLastPathDelim( const std::wstring& path )
    {
        return  FindLastPathDelimT<std::wstring, wchar_t>(path, L'/', L'\\');
    }

    size_t CPathUtils::FindLastPathDelim(const std::string& path)
    {
        return  FindLastPathDelimT<std::string, char>(path, '/', '\\');
    }


    std::string CPathUtils::FindFileName(const std::string& path)
	{
		if (!path.empty())
		{
			size_t nLen = path.length();
			for (size_t i = nLen - 1; i >= 0; --i)
			{
				if (path[i] == '\\' || path[i] == '/')
					return StringUtils::Right(path, i + 1);
			}
		}

		return path;
	}

    std::string CPathUtils::FindOnlyFileName(const std::string& path)
	{
        std::string sFileName = FindFileName(path);
		if (!sFileName.empty())
		{
			const size_t nLen = sFileName.length();
			for (size_t i = nLen - 1; i >= 0; --i)
			{
				if (sFileName[i] == '.')
					return StringUtils::Left(sFileName, i); 
			}
		}
		return sFileName;
	}

    std::string CPathUtils::FindFileExtension(const std::string& path)
	{
        std::string sFileName = FindFileName(path);
		if (sFileName.empty())
			return std::string();

		const size_t nLen =  sFileName.length();
		for (size_t i = nLen - 1; i >= 0; --i)
		{
			if (sFileName[i] == '.')
				return StringUtils::Right(sFileName, i + 1);
		}

		return std::string();
	}

    std::string CPathUtils::FindFilePath(const std::string& fullPath)
	{
		if (!fullPath.empty())
		{
			size_t nLen =  fullPath.length();
			for (size_t i = nLen - 1; i >= 0; --i)
			{
				if (fullPath[i] == '\\' || fullPath[i] == '/')
					return	StringUtils::Left(fullPath, i + 1); 
			}
		}

		return fullPath;
	}

    std::wstring CPathUtils::FindFileName(const std::wstring& path)
	{
		if (!path.empty())
		{
			size_t nLen =  path.length();
			for (size_t i = nLen - 1; i >= 0; --i)
			{
				if (path[i] == L'\\' || path[i] == L'/')
					return StringUtils::Right(path, i + 1);
			}
		}

		return path;
	}

    std::wstring CPathUtils::FindOnlyFileName(const std::wstring& path)
	{
        std::wstring sFileName = FindFileName(path);
		if (!sFileName.empty())
		{
			const size_t nLen = sFileName.length();
			for (size_t i = nLen - 1; i >= 0; --i)
			{
				if (sFileName[i] == L'.')
					return StringUtils::Left(sFileName, i);
			}
		}
		return sFileName;
	}

    std::wstring CPathUtils::FindFileExtension(const std::wstring& path)
	{
        std::wstring sFileName = FindFileName(path);
		if (sFileName.empty())
			return std::wstring();

		if (sFileName.empty())
			return sFileName;

		const size_t nLen =  sFileName.length();
		for (size_t i = nLen - 1; i >= 0; --i)
		{
			if (sFileName[i] == L'.')
				return StringUtils::Right(sFileName, i + 1);
		}

		return std::wstring();
	}

    std::wstring CPathUtils::FindFilePath(const std::wstring& fullPath)
	{
		if (!fullPath.empty())
		{
			const size_t nLen = fullPath.length();
			for (size_t i = nLen - 1; i >= 0; --i)
			{
				if (fullPath[i] == L'\\' || fullPath[i] == L'/')
					return	StringUtils::Left(fullPath, i + 1);
			}
		}

		return fullPath;
	}

    template<typename TStr>
    TStr FileMergePathT(const TStr& dir, const TStr& localPath)
    {

        if(dir.empty())
            return  localPath;

        size_t lastdelim_pos = CPathUtils::FindLastPathDelim(dir);
        if(TStr::npos != lastdelim_pos && lastdelim_pos + 1 == dir.length())
            return dir + localPath;

        return dir  + CPathUtils::GetPathDelim(dir) + localPath;
    }

    std::string CPathUtils::FileMergePath(const std::string& dir, const std::string& path)
    {
        return FileMergePathT<std::string>(dir, path);
    }

    std::wstring CPathUtils::FileMergePath(const std::wstring& dir, const std::wstring& path)
    {
        return FileMergePathT<std::wstring>(dir, path);
    }
}