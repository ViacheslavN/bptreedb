#pragma once
#include "../CommonLib.h"
#include "VariantVisitor.h"
#include "../exception/exc_base.h"

namespace CommonLib
{

    typedef std::string astr_t;
    typedef std::wstring wstr_t;
    typedef Data::CBlobPtr CBlobPtr_t;



    typedef void (*constructorVariant)(void *,const void *);
    typedef void (*destructorVariant)(void *);
    typedef int  (*compareVariantFunc)(const void *,const void *);
    typedef void (*copyVariantFunc)(void *,const void *);
    typedef void (*assignVariantFunc)(void *,int, const void *);
    typedef void (*acceptVariantFunc)(const void *,IVisitor &);


    template <int> struct maxSize{
        enum { mxSize=4 };
    };
    template <typename T > struct type2int;


    ///////////////////// TYPE LIST //////////////////////////////////

#define START_TYPE_LIST() \
	enum {_start_type_list_ = __LINE__ + 1 };

#define _curVal_ (__LINE__-_start_type_list_)

#define FINISH_TYPE_LIST() \
	enum {_type_list_count_ = _curVal_ };



#define DECLARE_TYPE(TT) \
	template<> struct maxSize<_curVal_>{ \
	enum { mxSize=(sizeof(TT) > maxSize<_curVal_-1>::mxSize) ? sizeof(TT) : maxSize<_curVal_-1>::mxSize }; \
	}; \
	template<> struct type2int<TT>{ \
	enum { typeId = _curVal_ }; \
	};

#define DECLARE_SIMPLE_TYPE(TT) DECLARE_TYPE(TT)
#define DECLARE_FUNC_TABLES

#include "VariantTypeList.h"

#undef DECLARE_FUNC_TABLES
#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE
#undef START_TYPE_LIST
#undef FINISH_TYPE_LIST
#undef _curVal_

    ////////////////////////////////////////////// TypeID ////////////////////////////////

#define START_TYPE_LIST()
#define FINISH_TYPE_LIST()

#define GIS_VAR_TYPE(TT) varType_##TT

#define DECLARE_TYPE(TT) \
	varType_##TT = type2int<TT>::typeId,

#define DECLARE_SIMPLE_TYPE(TT) DECLARE_TYPE(TT)

    //////////////DataType enum

    enum eDataTypes{
#include "VariantTypeList.h"
    };

#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE

    typedef std::shared_ptr<class CVariant> CVariantPtr;
    class CVariant {
    protected:
        eDataTypes m_id;

#define DECLARE_TYPE(TT)
#define DECLARE_SIMPLE_TYPE(TT) TT m_val##TT;
        union {
            char buffer[MAX_GV_SIZE];

#include "VariantTypeList.h"


        } m_DataBuffer;
#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE

    public:

        eDataTypes GetTypeID() const {
            return m_id;
        }

        void *getBuffer() { return &m_DataBuffer.buffer[0]; }

        const void *getBuffer() const { return &m_DataBuffer.buffer[0]; }

        CVariant()
        {
            m_id = (eDataTypes) (int) type2int<CEmptyVariant>::typeId;
        }

        CVariant(const CVariant &v)
        {
            m_id = v.GetTypeID();
            var_constructors_[m_id](getBuffer(), v.getBuffer());
        }

        template<typename T>
        void SetType() {
            m_id = eDataTypes(type2int<T>::typeId);
        }

        template<typename T>
        CVariant(const T &v) {
            new(&m_DataBuffer.buffer[0]) T(v);
            m_id = eDataTypes(type2int<T>::typeId);
        }

        ~CVariant()
        {
            var_destructors_[m_id](getBuffer());
        }

        bool IsNull() const
        {
            return (m_id == eDataTypes(type2int<CEmptyVariant>::typeId));
        }

        template<typename T>
        bool IsType() const
        {
            return (m_id == eDataTypes(type2int<T>::typeId));
        }

        CVariant &operator=(const CVariant &variant)
        {
            eDataTypes destId = variant.GetTypeID();
            if (m_id == destId)
                var_copy_[m_id](getBuffer(), variant.getBuffer());
            else
            {
                var_assign_[m_id](getBuffer(), destId, variant.getBuffer());
                m_id = destId;
            }
            return *this;
        }

        template<typename T>
        CVariant &operator=(const T &value)
        {
            eDataTypes destId = (eDataTypes) type2int<T>::typeId;
            if (m_id == destId)
                var_copy_[m_id](getBuffer(), &value);
            else
            {
                var_assign_[m_id](getBuffer(), destId, &value);
                m_id = destId;
            }
            return *this;
        }

        template<typename T>
        T &Get()
        {
            if (m_id != (eDataTypes) type2int<T>::typeId)
                throw CExcBase("Variant: Failed to type, type id: {0}, convert type id: {1}", m_id,
                               (eDataTypes) type2int<T>::typeId);

            return *(T *) getBuffer();
        }

        template<typename T>
        const T &Get() const
        {
            if (m_id != (eDataTypes) type2int<T>::typeId)
                throw CExcBase("Variant: Failed to type, type id: {0}, convert type id: {1}", m_id,
                               (eDataTypes) type2int<T>::typeId);

            return *(T *) getBuffer();
        }

        template<typename T>
        void GetVal(T &value) const {
            if (m_id != (eDataTypes) type2int<T>::typeId)
                throw CExcBase("Variant: Failed to type, type id: {0}, convert type id: {1}", m_id,
                               (eDataTypes) type2int<T>::typeId);

            value = *(T *) getBuffer();
        }

        template<typename T>
        void SetVal(const T &value)
        {
            eDataTypes destId = (eDataTypes) type2int<T>::typeId;
            if (m_id == destId)
                var_copy_[m_id](getBuffer(), &value);
            else
            {
                var_assign_[m_id](getBuffer(), destId, &value);
                m_id = destId;
            }

        }

        template<typename T>
        T *GetPtr()
        {
            if (m_id != (eDataTypes) type2int<T>::typeId)
                return NULL;

            return (T *) getBuffer();
        }

        template<typename T>
        const T *GetPtr() const
        {
            if (m_id != (eDataTypes) type2int<T>::typeId)
                return NULL;

            return (T *) getBuffer();
        }

        int Compare(const CVariant &variant) const
        {
            int destId = variant.GetTypeID();
            int delta = GetTypeID() - destId;
            if (delta == 0)
                return var_compare_[m_id](getBuffer(), variant.getBuffer());

            return delta;
        }

        void Accept(IVisitor& visitor) const
        {
            var_accept_[m_id](getBuffer(), visitor);
        }

        bool operator==(const CVariant &variant) const
        {
            if (GetTypeID() != variant.GetTypeID())
                return false;

            return (var_compare_[m_id](getBuffer(), variant.getBuffer()) == 0);
        }

        bool operator!=(const CVariant &variant) const
        {
            if (GetTypeID() != variant.GetTypeID())
                return true;

            return (var_compare_[m_id](getBuffer(), variant.getBuffer()) != 0);
        }

        bool operator<(const CVariant &variant) const
        {
            int destId = variant.GetTypeID();
            int delta = GetTypeID() - destId;
            if (delta == 0)
                return (var_compare_[m_id](getBuffer(), variant.getBuffer()) < 0);

            return (delta < 0);
        }

        bool operator<=(const CVariant &variant) const
        {
            int destId = variant.GetTypeID();
            int delta = GetTypeID() - destId;
            if (delta == 0)
                return (var_compare_[m_id](getBuffer(), variant.getBuffer()) <= 0);

            return (delta <= 0);
        }

        bool operator>(const CVariant &variant) const
        {
            int destId = variant.GetTypeID();
            int delta = GetTypeID() - destId;
            if (delta == 0)
                return (var_compare_[m_id](getBuffer(), variant.getBuffer()) > 0);

            return (delta > 0);
        }

        bool operator>=(const CVariant &variant) const
        {
            int destId = variant.GetTypeID();
            int delta = GetTypeID() - destId;
            if (delta == 0)
                return (var_compare_[m_id](getBuffer(), variant.getBuffer()) >= 0);

            return (delta >= 0);
        }

    };

    template <class TVisitor>
    TVisitor ApplyVisitor (const CVariant& variant, TVisitor &visitor)
    {
        variant.Accept (visitor);
        return visitor;
    };

#undef START_TYPE_LIST
#undef FINISH_TYPE_LIST

}

