#pragma once
#include <memory>
#include <string>

namespace CommonLib {
    namespace file {

        class IDirectoryIterator {
            public:
            IDirectoryIterator(){}
            virtual ~IDirectoryIterator(){}

            enum DirectoryEntryType {FILE_TYPE_DIRECTORY, FILE_TYPE_FILE};

            virtual bool GetNextItem(std::wstring& path, int& type ) = 0;
        };

        typedef std::shared_ptr<IDirectoryIterator> DirectoryIteratorPtr;

        class CDirectoryBrowser
        {
        public:
            enum DirectoryBrowsingFilter{ONLY_DIRECTORY = 1, ONLY_FILE =2, ALL_ITEMS = 3};
            enum DirectoryBrowsingLevel{ONLY_ONE_LEVEL = 1,  ALL_FILE_TREE = 2};

            static DirectoryIteratorPtr GetDirectoryIterator(const std::wstring& path, DirectoryBrowsingFilter filter = ALL_ITEMS, DirectoryBrowsingLevel level = ALL_FILE_TREE);



        };


    }
}
