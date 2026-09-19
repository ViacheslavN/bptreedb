
#include "filesystem.h"
#include "Directory.h"

namespace CommonLib {
    namespace file {

        class DirectoryIteratorWin : public IDirectoryIterator {
        public:
            DirectoryIteratorWin(const std::wstring& rootPath, CDirectoryBrowser::DirectoryBrowsingFilter filter, CDirectoryBrowser::DirectoryBrowsingLevel level);
            virtual ~DirectoryIteratorWin(){}
            virtual bool GetNextItem(std::wstring& path, int& type );
        private:
            bool ProcessFindData(std::wstring& path, int& type,  WIN32_FIND_DATA&  searchData );

        private:
            std::wstring m_rootPath;
            CDirectoryBrowser::DirectoryBrowsingFilter m_filter;
            CDirectoryBrowser::DirectoryBrowsingLevel m_level;
            std::shared_ptr<CFindFileHandle> m_FindFileHandlePtr;

        };

    }
}