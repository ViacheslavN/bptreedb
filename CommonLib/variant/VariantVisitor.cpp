#include "VariantVisitor.h"
#include "../str/str.h"

namespace CommonLib {

    CStringVisitor:: CStringVisitor()
    {}

    CStringVisitor::~CStringVisitor()
    {}

    void CStringVisitor::Visit(const CEmptyVariant& val)
    {
        m_val = std::string();
    }

    void CStringVisitor::Visit(const bool& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const int8_t& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const uint8_t& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const int16_t& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const uint16_t& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const int32_t& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const uint32_t& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const int64_t& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const uint64_t& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const float& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const double& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const std::string& val)
    {
        m_val = val;
    }

    void CStringVisitor::Visit(const std::wstring& val)
    {
        m_val = str_format::AStrFormatSafeT("{0}", val);
    }

    void CStringVisitor::Visit(const Data::CBlobPtr& val)
    {
        m_val = std::string();
    }

    void CStringVisitor::Visit(const IGeoShapePtr& val)
    {
        m_val = std::string();
    }

    void CStringVisitor::Visit(const CGuid& val)
    {
        m_val = val.ToAstr(true);
    }


}