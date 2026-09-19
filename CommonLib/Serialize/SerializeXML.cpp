#include "stdafx.h"
#include "SerializeXML.h"

namespace CommonLib {
    CSerializeObjXML::CSerializeObjXML(xml::IXMLNodePtr ptrNode) : m_ptrNode(ptrNode)
    {

    }

    CSerializeObjXML::~CSerializeObjXML()
    {

    }

    const std::string& CSerializeObjXML::GetName() const
    {
        return m_ptrNode->GetName();
    }

    void  CSerializeObjXML::SetName(const std::string& name)
    {
        m_ptrNode->SetName(name);
    }

    ISerializeObjPtr  CSerializeObjXML::CreateChildNode(const std::string& name) const
    {
        return std::make_shared<CSerializeObjXML>(m_ptrNode->CreateChildNode(name));
    }

    uint32_t CSerializeObjXML::GetChildCnt() const
    {
        return m_ptrNode->GetChildCnt();
    }

    ISerializeObjPtr CSerializeObjXML::GetChild(uint32_t nIndex) const
    {
        return std::make_shared<CSerializeObjXML>(m_ptrNode->GetChild(nIndex));
    }

    ISerializeObjPtr CSerializeObjXML::GetChild(const std::string& name) const
    {
        return std::make_shared<CSerializeObjXML>(m_ptrNode->GetChild(name));
    }

    std::vector<ISerializeObjPtr> CSerializeObjXML::GetChilds(const std::string& name) const
    {
        std::vector<xml::IXMLNodePtr> nodes = m_ptrNode->GetChilds(name);
        std::vector<ISerializeObjPtr> ret;

        for (size_t i = 0, sz = nodes.size(); i < sz; ++i)
        {
            ret.push_back(std::make_shared<CSerializeObjXML>(nodes[i]));
        }

        return ret;
    }

    bool CSerializeObjXML::IsChildExists(const std::string& name) const
    {
        return m_ptrNode->IsChildExists(name);
    }

    void CSerializeObjXML::GetBlob(CommonLib::Data::TVecBuffer& data) const
    {
        m_ptrNode->GetBlobCDATA(data);
    }

    void CSerializeObjXML::SetBlob(const byte_t* data, size_t size)
    {
        m_ptrNode->SetBlobCDATA(data, size);
    }

    void CSerializeObjXML::AddPropertyInt16(const  std::string& name, int16_t value)
    {
        m_ptrNode->AddPropertyInt16(name, value);
    }

    void CSerializeObjXML::AddPropertyInt16U(const std::string& name, uint16_t value)
    {
        m_ptrNode->AddPropertyInt16U(name, value);
    }

    void CSerializeObjXML::AddPropertyInt32(const std::string& name, int32_t value)
    {
        m_ptrNode->AddPropertyInt32(name, value);
    }

    void CSerializeObjXML::AddPropertyInt32U(const std::string& name, uint32_t value)
    {
        m_ptrNode->AddPropertyInt32U(name, value);
    }

    void CSerializeObjXML::AddPropertyInt64(const std::string& name, int64_t value)
    {
        m_ptrNode->AddPropertyInt64(name, value);
    }

    void CSerializeObjXML::AddPropertyIntU64(const std::string& name, uint64_t value)
    {
        m_ptrNode->AddPropertyIntU64(name, value);
    }

    void CSerializeObjXML::AddPropertyDouble(const std::string& name, double value)
    {
        m_ptrNode->AddPropertyDouble(name, value);
    }

    void CSerializeObjXML::AddPropertyBool(const std::string& name, bool value)
    {
        m_ptrNode->AddPropertyBool(name, value);
    }

    void CSerializeObjXML::AddPropertyString(const std::string& name, const std::string& valueUtf8)
    {
        m_ptrNode->AddPropertyString(name, valueUtf8);
    }

    void CSerializeObjXML::AddPropertyWString(const std::string& name, const std::wstring& value)
    {
        m_ptrNode->AddPropertyWString(name, value);
    }

    void CSerializeObjXML::AddPropertyGuid(const std::string& name, const CommonLib::CGuid& value)
    {
        m_ptrNode->AddPropertyString(name, value.ToAstr(false));
    }


    bool CSerializeObjXML::IsPropertyExists(const std::string& name) const
    {
        return m_ptrNode->IsPropertyExists(name);
    }

    const std::string& CSerializeObjXML::GetProperty(const std::string& name) const
    {
        return m_ptrNode->GetProperty(name);
    }

    uint32_t CSerializeObjXML::GetPropertyCnt() const
    {
        return m_ptrNode->GetPropertyCnt();
    }

    const std::string&  CSerializeObjXML::GetProperty(uint32_t nIndex) const
    {
        return m_ptrNode->GetProperty(nIndex);
    }

    int16_t	CSerializeObjXML::GetPropertyInt16(const std::string& name, int16_t defValue) const
    {
        return m_ptrNode->GetPropertyInt16(name, defValue);
    }

    uint16_t CSerializeObjXML::GetPropertyInt16U(const std::string& name, uint16_t defValue) const
    {
        return m_ptrNode->GetPropertyInt16U(name, defValue);
    }

    int32_t	 CSerializeObjXML::GetPropertyInt32(const std::string& name, int32_t defValue) const
    {
        return m_ptrNode->GetPropertyInt32(name, defValue);
    }

    uint32_t CSerializeObjXML::GetPropertyInt32U(const std::string& name, uint32_t defValue) const
    {
        return m_ptrNode->GetPropertyInt32U(name, defValue);
    }

    int64_t	CSerializeObjXML::GetPropertyInt64(const std::string& name, int64_t defValue) const
    {
        return m_ptrNode->GetPropertyInt64(name, defValue);
    }

    uint64_t CSerializeObjXML::GetPropertyIntU64(const std::string& name, uint64_t defValue) const
    {
        return m_ptrNode->GetPropertyIntU64(name, defValue);
    }

    double  CSerializeObjXML::GetPropertyDouble(const std::string& name, double defValue) const
    {
        return m_ptrNode->GetPropertyDouble(name, defValue);
    }

    bool  CSerializeObjXML::GetPropertyBool(const std::string& name, bool defValue) const
    {
        return m_ptrNode->GetPropertyBool(name, defValue);
    }

    std::string CSerializeObjXML::GetPropertyString(const std::string& name, const  std::string& defValueUtf8) const
    {
        return m_ptrNode->GetPropertyString(name, defValueUtf8);
    }

    std::wstring CSerializeObjXML::GetPropertyWString(const std::string& name, const  std::wstring& defValue) const
    {
        return m_ptrNode->GetPropertyWString(name, defValue);
    }

    CommonLib::CGuid CSerializeObjXML::GetPropertyGuid(const std::string& name, const CommonLib::CGuid& value) const
    {
        std::string strValue = m_ptrNode->GetPropertyString(name, "");
        if(strValue.empty())
            return value;

        return CommonLib::CGuid::CreateFromAstr(strValue);
    }


    int16_t	CSerializeObjXML::GetPropertyInt16(const std::string& name) const
    {
        return m_ptrNode->GetPropertyInt16(name);
    }

    uint16_t CSerializeObjXML::GetPropertyInt16U(const std::string& name) const
    {
        return m_ptrNode->GetPropertyInt16U(name);
    }

    int32_t	 CSerializeObjXML::GetPropertyInt32(const std::string& name) const
    {
        return m_ptrNode->GetPropertyInt32(name);
    }

    uint32_t CSerializeObjXML::GetPropertyInt32U(const std::string& name) const
    {
        return m_ptrNode->GetPropertyInt32U(name);
    }

    int64_t	CSerializeObjXML::GetPropertyInt64(const std::string& name) const
    {
        return m_ptrNode->GetPropertyInt64(name);
    }

    uint64_t CSerializeObjXML::GetPropertyIntU64(const std::string& name) const
    {
        return m_ptrNode->GetPropertyIntU64(name);
    }

    double  CSerializeObjXML::GetPropertyDouble(const std::string& name) const
    {
        return m_ptrNode->GetPropertyDouble(name);
    }

    bool  CSerializeObjXML::GetPropertyBool(const std::string& name) const
    {
        return m_ptrNode->GetPropertyBool(name);
    }

    std::string CSerializeObjXML::GetPropertyString(const std::string& name) const
    {
        return m_ptrNode->GetPropertyString(name);
    }

    CommonLib::CGuid CSerializeObjXML::GetPropertyGuid(const std::string& name) const
    {
        return CommonLib::CGuid::CreateFromAstr(  m_ptrNode->GetPropertyString(name));
    }
}