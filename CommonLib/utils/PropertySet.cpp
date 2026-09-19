#include "PropertySet.h"
#include "../exception/exc_base.h"
namespace CommonLib
{
     CPropertySet::CPropertySet()
     {

     }

     CPropertySet::~CPropertySet()
     {

     }

     int  CPropertySet::Count() const
     {
         return (int)m_mapProp.size();
     }

     bool  CPropertySet::PropertyExists(const std::string& name) const
     {
         return m_mapProp.find(name) != m_mapProp.end();
     }

     const CVariantPtr CPropertySet::GetProperty(const std::string& name) const
     {
         TMapProp::const_iterator c_it =  m_mapProp.find(name);
         if(c_it == m_mapProp.end())
             throw CExcBase("CPropertySet: property: {0} not found", name);

        return c_it->second;
     }

     void  CPropertySet::SetProperty(const std::string& name, CVariantPtr value)
     {
         m_mapProp[name] = value;
     }

     void  CPropertySet::RemoveProperty(const std::string& name)
     {
         TMapProp::iterator it =  m_mapProp.find(name);
         if(it != m_mapProp.end())
         {
             m_mapProp.erase(it);
         }
     }

     void  CPropertySet::RemoveAllProperties()
     {
         m_mapProp.clear();
     }

}