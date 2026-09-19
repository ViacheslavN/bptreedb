#pragma once
#include <string>
#include <vector>
#include "../../CommonLib.h"
#include "BrowsingHistory.h"


class CHistoryFilter {
public:
    static void Filter(const std::vector<CBrowsingHistory::HistoryItem>& historyItems, const std::string& regex,  std::vector<CBrowsingHistory::HistoryItem> &outItems);
};