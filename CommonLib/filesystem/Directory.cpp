#include "stdafx.h"
#include "Directory.h"
#ifdef _WIN32
#include "DirectoryWin.h"
#endif


namespace CommonLib {
    namespace file {

#ifdef _WIN32
        DirectoryIteratorPtr CDirectoryBrowser::GetDirectoryIterator(const std::wstring& path, DirectoryBrowsingFilter filter, DirectoryBrowsingLevel level) {

            return std::make_shared<DirectoryIteratorWin>(  path, filter, level );
        }

#endif

    }
}