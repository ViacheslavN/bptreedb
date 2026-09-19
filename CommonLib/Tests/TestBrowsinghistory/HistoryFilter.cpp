#include <regex>
#include "HistoryFilter.h"
#include "../../exception/exc_base.h"


void CHistoryFilter::Filter(const std::vector<CBrowsingHistory::HistoryItem> &historyItems, const std::string &url_regex,  std::vector<CBrowsingHistory::HistoryItem> &outItems) {

    try
    {
        std::regex regex(url_regex);

        for (size_t i = 0; i < historyItems.size(); i++) {
           ;
           auto history_item = historyItems[i];
            if ( std::regex_match(history_item.m_url, regex)) {
                outItems.push_back(history_item);

            }
        }


    }
    catch(std::exception &exc)
    {
        CommonLib::CExcBase::RegenExcT("CHistoryFilter: Failed to filter history",exc);
    }
}
