#pragma once
#include "../CommonLib.h"
#include "../data/blob.h"
#include "../SpatialData/IGeoShape.h"
#include "../guid/Guid.h"

namespace CommonLib
{

    class CEmptyVariant
    {
    public:
        CEmptyVariant()
        {}
        ~CEmptyVariant()
        {}
        CEmptyVariant(const CEmptyVariant&)
        {}
        bool operator<(const CEmptyVariant&) const
        {
            return false;
        }
        bool operator<=(const CEmptyVariant&) const
        {
            return true;
        }
        bool operator>(const CEmptyVariant&) const
        {
            return false;
        }
        bool operator>=(const CEmptyVariant&) const
        {
            return true;
        }
        bool operator==(const CEmptyVariant&) const
        {
            return true;
        }
        bool operator!=(const CEmptyVariant&) const
        {
            return false;
        }
    };

    class IVisitor
    {
    public:
        IVisitor(){}
        virtual ~IVisitor(){}
        virtual void Visit(const CEmptyVariant& val) = 0;
        virtual void Visit(const bool& val) = 0;
        virtual void Visit(const int8_t& val) = 0;
        virtual void Visit(const uint8_t& val) = 0;
        virtual void Visit(const int16_t& val) = 0;
        virtual void Visit(const uint16_t& val) = 0;
        virtual void Visit(const int32_t& val) = 0;
        virtual void Visit(const uint32_t& val) = 0;
        virtual void Visit(const int64_t& val) = 0;
        virtual void Visit(const uint64_t& val) = 0;
        virtual void Visit(const float& val) = 0;
        virtual void Visit(const double& val) = 0;
        virtual void Visit(const std::string& val) = 0;
        virtual void Visit(const std::wstring& val) = 0;
        virtual void Visit(const Data::CBlobPtr& val) = 0;
        virtual void Visit(const IGeoShapePtr& val) = 0;
        virtual void Visit(const CGuid& val) = 0;
    };


    class CStringVisitor : public IVisitor
    {
    public:
        CStringVisitor();
        virtual ~CStringVisitor();
        virtual void Visit(const CEmptyVariant& val);
        virtual void Visit(const bool& val);
        virtual void Visit(const int8_t& val);
        virtual void Visit(const uint8_t& val);
        virtual void Visit(const int16_t& val);
        virtual void Visit(const uint16_t& val);
        virtual void Visit(const int32_t& val);
        virtual void Visit(const uint32_t& val);
        virtual void Visit(const int64_t& val);
        virtual void Visit(const uint64_t& val);
        virtual void Visit(const float& val) ;
        virtual void Visit(const double& val);
        virtual void Visit(const std::string& val);
        virtual void Visit(const std::wstring& val);
        virtual void Visit(const Data::CBlobPtr& val);
        virtual void Visit(const IGeoShapePtr& val);
        virtual void Visit(const CGuid& val);
    private:
        std::string m_val;

    };


}