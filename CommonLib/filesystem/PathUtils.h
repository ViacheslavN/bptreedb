#pragma once

#include "CommonLib.h"

namespace CommonLib
{
	class CPathUtils
	{
	public:
        static std::string GetPathDelim(const std::string& path);
        static std::wstring GetPathDelim(const std::wstring& path);
        static size_t FindLastPathDelim( const std::wstring& path);
        static size_t FindLastPathDelim(const std::string& path);



		static std::string FindFileName(const std::string& path);
		static std::string FindOnlyFileName(const std::string& path);
		static std::string FindFileExtension(const std::string& path);
		static std::string FindFilePath(const std::string& fullPath);
        static std::string FileMergePath(const std::string& dir, const std::string& path);

		static std::wstring FindFileName(const std::wstring& path);
		static std::wstring FindOnlyFileName(const std::wstring& path);
		static std::wstring FindFileExtension(const std::wstring& path);
		static std::wstring FindFilePath(const std::wstring& fullPath);
        static std::wstring FileMergePath(const std::wstring& dir, const std::wstring& path);

	};
}