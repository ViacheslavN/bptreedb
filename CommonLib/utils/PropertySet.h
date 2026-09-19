#pragma once
#include "../CommonLib.h"
#include "../variant/Variant.h"

namespace CommonLib
{

    typedef std::shared_ptr<class IPropertySet> IPropertySetPtr;

    class IPropertySet
    {
    public:
        IPropertySet(){}
        virtual ~IPropertySet(){}
        virtual int  Count() const = 0;
        virtual bool  PropertyExists(const std::string& name) const = 0;
        virtual CVariantPtr  GetProperty(const std::string& name) const = 0;
        virtual void  SetProperty(const std::string& name, CVariantPtr value) = 0;
        virtual void  RemoveProperty(const std::string& name) = 0;
        virtual void  RemoveAllProperties() = 0;

    };


    class CPropertySet
    {
    public:
        CPropertySet();
        virtual ~CPropertySet();
        virtual int  Count() const;
        virtual bool  PropertyExists(const std::string& name) const;
        virtual const CVariantPtr GetProperty(const std::string& name) const;
        virtual void  SetProperty(const std::string& name, CVariantPtr value);
        virtual void  RemoveProperty(const std::string& name);
        virtual void  RemoveAllProperties();

    private:



        typedef std::map<std::string, CVariantPtr> TMapProp;
        TMapProp m_mapProp;
    };
}
