#include "stdafx.h"
#include "filesystem.h"
#include "DirectoryWin.h"
#include "../exception/WinExc.h"

namespace CommonLib {
    namespace file {

        DirectoryIteratorWin::DirectoryIteratorWin(const std::wstring& rootPath, CDirectoryBrowser::DirectoryBrowsingFilter filter, CDirectoryBrowser::DirectoryBrowsingLevel level):
        m_rootPath(rootPath), m_filter(filter), m_level(level)
        {

        }

        bool DirectoryIteratorWin::GetNextItem(std::wstring& path, int& type )
        {
            try
            {
                WIN32_FIND_DATA searchData= {0};
                if (!m_FindFileHandlePtr.get())
                {
                    HANDLE  hSearch = FindFirstFileW(m_rootPath.c_str(), &searchData);
                    if (INVALID_HANDLE_VALUE == hSearch)
                        throw CWinExc("DirectoryIteratorWin: field FindFirstFileW");
                    m_FindFileHandlePtr = std::make_shared<CFindFileHandle>(hSearch);
                   return  ProcessFindData(path, type, searchData);
                }
                else {
                    if (FindNextFile(m_FindFileHandlePtr->Get(), &searchData)== TRUE)
                        return  ProcessFindData(path, type, searchData);
                }


            }
            catch (std::exception& exc) {
                CExcBase::RegenExcT("DirectoryIteratorWin: Failed to iterate, root path {0}", m_rootPath, exc);
                throw;
            }

            return false;
        }

        bool DirectoryIteratorWin::ProcessFindData(std::wstring& path, int& type,  WIN32_FIND_DATA&  searchData ) {


            do
            {
                std::wstring nextChild = searchData.cFileName;
                if (_wcsicmp(nextChild.c_str(), L".") == 0)
                    continue;

                if (_wcsicmp(nextChild.c_str(), L"..") == 0)
                    continue;

                if ((searchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    if (m_filter & CDirectoryBrowser::ONLY_DIRECTORY)
                    {
                        path = nextChild;
                        type = IDirectoryIterator::FILE_TYPE_DIRECTORY;
                        return true;
                    };
                }
                else
                {
                    if (m_filter & CDirectoryBrowser::ONLY_FILE)
                    {
                        path = nextChild;
                        type = IDirectoryIterator::FILE_TYPE_FILE;
                        return true;
                    };
                }

            }
            while (FindNextFile(m_FindFileHandlePtr->Get(), &searchData) !=0);

            return false;

        }

    }
}