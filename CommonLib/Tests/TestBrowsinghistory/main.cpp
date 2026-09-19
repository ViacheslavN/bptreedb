
#include "../../CommonLib.h"
#include "../../filesystem/filesystem.h"
#include "../../filesystem/File.h"
#include "../../filesystem/Directory.h"

#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <shlobj.h>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include "BrowsingHistory.h"
#include "HistoryFilter.h"
#include <regex>
#include <fstream>

bool CopyDatabaseToTemp(const std::wstring& dbPath, std::wstring& tempDbPath)
{
    wchar_t tempPath[MAX_PATH];
    if (GetTempPathW(MAX_PATH, tempPath) == 0)
    {
        return false;
    }

    wchar_t tempFileName[MAX_PATH];
    if (GetTempFileNameW(tempPath, L"dbcopy", 0, tempFileName) == 0)
    {
        return false;
    }

    tempDbPath = std::wstring(tempFileName);

    try
    {
        std::filesystem::copy_file(dbPath, tempDbPath, std::filesystem::copy_options::overwrite_existing);
        return true;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::wcout << L"Error copy DB: " << dbPath << std::endl;
        return false;
    }
}




int main()
{

   /* for (int i = 0; i < 10000; ++i) {
        ::Sleep(10);
    }*/

    try {
        WCHAR path[MAX_PATH];
        SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path);
        std::wstring userProfilePath(path);
        std::wstring chromeHistoryPath = userProfilePath + L"\\AppData\\Local\\Google\\Chrome\\User Data\\";

        CommonLib::file::DirectoryIteratorPtr fileIteror =CommonLib::file::CDirectoryBrowser::GetDirectoryIterator(chromeHistoryPath + L"*", CommonLib::file::CDirectoryBrowser::ONLY_DIRECTORY);

        std::wstring filePath;
        int type = 0;

        std::vector<std::wstring> historiesDbs;
        while (fileIteror->GetNextItem(filePath, type))
        {
            if (filePath.find(L"Profile") == 0) {
                std::wstring database = chromeHistoryPath + filePath + L"\\History";

                if (CommonLib::CFileUtils::IsFileExist(database))
                    historiesDbs.push_back(database);
            }
        }

        std::vector<CBrowsingHistory::HistoryItem> items;
        std::vector<CBrowsingHistory::HistoryItem> filteredItems;

        for (size_t i = 0; i < historiesDbs.size(); ++i) {

            std::wstring dbInTemp;
            try {
                if (CopyDatabaseToTemp(historiesDbs[i], dbInTemp)) {
                    CBrowsingHistory history(dbInTemp);
                    history.Export(items);
                }
            }
            catch (std::exception& e) {
                std::wcout << L"Error: " << e.what() << std::endl;

            }
            if (CommonLib::CFileUtils::IsFileExist(dbInTemp)) {
                CommonLib::CFileUtils::DelFile(dbInTemp);
            }
        }

        CHistoryFilter::Filter(items, "^.*cambridge.org.*$", filteredItems);

        std::map<std::string, int> words;

        for (size_t i = 0; i < filteredItems.size(); ++i) {
            std::regex pattern("\\s*[\\|-].*");
            std::string cleaned = std::regex_replace(filteredItems[i].m_title, pattern, "");

            words[cleaned] += 1;
        }

        size_t size = words.size();

        std::ofstream ofs("D:\\vocabular.txt");

        for (auto it = words.begin(); it != words.end(); ++it) {
            ofs << it->first <<"     |       " << it->second << std::endl;
        }
        ofs.flush();
        ofs.close();

    }
    catch (std::exception& exc) {
        std::cout << "Error: " << exc.what();
    }

    return 0;
}