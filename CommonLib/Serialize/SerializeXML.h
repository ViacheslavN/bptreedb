#pragma once
#include "CommonLib.h"
#include "SerializeObj.h"
#include "../xml/xml.h"
#include "../str/convertToString.h"

namespace CommonLib {
    class CSerializeObjXML : public ISerializeObj
    {
    public:
        CSerializeObjXML(xml::IXMLNodePtr ptrNode);
        virtual ~CSerializeObjXML();

        virtual const std::string& GetName() const;
        virtual void       SetName(const std::string&  name);
        virtual ISerializeObjPtr   CreateChildNode(const std::string& name) const;
        virtual uint32_t	GetChildCnt() const;
        virtual ISerializeObjPtr	GetChild(uint32_t nIndex) const;
        virtual ISerializeObjPtr	GetChild(const std::string& name) const;
        virtual std::vector<ISerializeObjPtr> GetChilds(const std::string& name) const;
        virtual bool	IsChildExists(const std::string& name) const;

        virtual	void GetBlob(CommonLib::Data::TVecBuffer& data) const;
        virtual void SetBlob(const byte_t* data, size_t size);

        virtual void AddPropertyInt16(const  std::string& name, int16_t value);
        virtual void AddPropertyInt16U(const std::string& name, uint16_t value);
        virtual void AddPropertyInt32(const std::string& name, int32_t value);
        virtual void AddPropertyInt32U(const std::string& name, uint32_t value);
        virtual void AddPropertyInt64(const std::string& name, int64_t value);
        virtual void AddPropertyIntU64(const std::string& name, uint64_t value);
        virtual void AddPropertyDouble(const std::string& name, double value);
        virtual void AddPropertyBool(const std::string& name, bool value);
        virtual void AddPropertyString(const std::string& name, const std::string& valueUtf8);
        virtual void AddPropertyWString(const std::string& name, const std::wstring& value);
        virtual void AddPropertyGuid(const std::string& name, const CommonLib::CGuid& value);


        virtual bool IsPropertyExists(const std::string& name) const;
        virtual const std::string& GetProperty(const std::string& name) const;
        virtual uint32_t GetPropertyCnt() const;
        virtual const std::string&  GetProperty(uint32_t nIndex) const;

        virtual int16_t	GetPropertyInt16(const std::string& name, int16_t defValue) const;
        virtual uint16_t GetPropertyInt16U(const std::string& name, uint16_t defValue) const;
        virtual int32_t	 GetPropertyInt32(const std::string& name, int32_t defValue) const;
        virtual uint32_t GetPropertyInt32U(const std::string& name, uint32_t defValue) const;
        virtual int64_t	GetPropertyInt64(const std::string& name, int64_t defValue) const;
        virtual uint64_t GetPropertyIntU64(const std::string& name, uint64_t defValue) const;
        virtual double  GetPropertyDouble(const std::string& name, double defValue) const;
        virtual bool  GetPropertyBool(const std::string& name, bool defValue) const;
        virtual std::string GetPropertyString(const std::string& name, const  std::string& defValueUtf8) const;
        virtual std::wstring GetPropertyWString(const std::string& name, const  std::wstring& defValue) const;
        virtual CommonLib::CGuid GetPropertyGuid(const std::string& name, const CommonLib::CGuid& value) const;


        virtual int16_t	GetPropertyInt16(const std::string& name) const;
        virtual uint16_t GetPropertyInt16U(const std::string& name) const;
        virtual int32_t	 GetPropertyInt32(const std::string& name) const ;
        virtual uint32_t GetPropertyInt32U(const std::string& name) const ;
        virtual int64_t	GetPropertyInt64(const std::string& name) const;
        virtual uint64_t GetPropertyIntU64(const std::string& name) const;
        virtual double  GetPropertyDouble(const std::string& name) const;
        virtual bool  GetPropertyBool(const std::string& name) const;
        virtual std::string GetPropertyString(const std::string& name) const;
        virtual CommonLib::CGuid GetPropertyGuid(const std::string& name) const;

    private:
        xml::IXMLNodePtr m_ptrNode;
    };


}