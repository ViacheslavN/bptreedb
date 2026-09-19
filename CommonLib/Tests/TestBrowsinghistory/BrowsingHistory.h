#pragma once
#include <string>
#include <vector>
#include "../../CommonLib.h"
#include "../../sqlitelib/Database.h"

class CBrowsingHistory {
public:
    struct HistoryItem {
        std::string m_url;
        std::string m_title;
        int64_t m_visitTime;

        HistoryItem(const std::string& url, const std::string& title, int64_t visitTime):
        m_url(url), m_title(title), m_visitTime(visitTime)
        {

        }
    };

    CBrowsingHistory(const std::wstring& dbPath);

    void Export(std::vector<HistoryItem>& items);

private:
    CommonLib::database::IDatabasePtr m_ptrDatabase;
};