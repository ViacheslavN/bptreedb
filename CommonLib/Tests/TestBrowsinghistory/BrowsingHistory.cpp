#include "BrowsingHistory.h"
#include "../../exception/exc_base.h"
#include "../../str/StringEncoding.h"

CBrowsingHistory::CBrowsingHistory(const std::wstring &dbPath) {
    try
    {
        m_ptrDatabase = CommonLib::database::IDatabaseSQLiteCreator::Create(CommonLib::StringEncoding::str_w2utf8_safe(dbPath).c_str(), uint32_t(CommonLib::database::ReadOnlyMode));
    }
    catch (std::exception &exc) {
        CommonLib::CExcBase::RegenExcT("CBrowsingHistory: Failed to open database,  path {0}", dbPath, exc);
    }

}

void CBrowsingHistory::Export(std::vector<HistoryItem>& items) {
    try
    {
        const char* sql =
                    "SELECT url, title, last_visit_time "
                    "FROM urls "
                    "ORDER BY last_visit_time DESC ";


       CommonLib::database::IStatmentPtr ptrStatment =  m_ptrDatabase->PrepareQuery(sql);
        while (ptrStatment->Next()) {
            std::string foundUrlUtf8 = ptrStatment->ReadText(0);
            std::string titleUrlUtf8 = ptrStatment->ReadText(1);
            int64_t visitTimeWebKit = ptrStatment->ReadInt64(2);

            items.push_back(HistoryItem(foundUrlUtf8, titleUrlUtf8, visitTimeWebKit));

        }

    }
    catch (std::exception &exc) {
       CommonLib::CExcBase::RegenExcT("CBrowsingHistory: Failed to export history",exc);
    }
}