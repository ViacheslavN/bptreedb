#pragma once

#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/exception/exc_base.h"
#include "../../../CommonLib/stream/stream.h"

namespace bptreedb
{
	typedef CommonLib::CGuid CGuid;

	class CEmptyVariant
	{
	public:
		CEmptyVariant()	{}
		virtual ~CEmptyVariant() {}

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
		IVisitor() {}
		virtual ~IVisitor() {}
		virtual void Visit(const CEmptyVariant& val) = 0;
		virtual void Visit(const bool   &val) = 0;
		virtual void Visit(const int8_t  &val) = 0;
		virtual void Visit(const byte_t   &val) = 0;
		virtual void Visit(const int16_t  &val) = 0;
		virtual void Visit(const uint16_t &val) = 0;
		virtual void Visit(const int32_t  &val) = 0;
		virtual void Visit(const uint32_t  &val) = 0;
		virtual void Visit(const int64_t &val) = 0;
		virtual void Visit(const uint64_t &val) = 0;
		virtual void Visit(const float &val) = 0;
		virtual void Visit(const double &val) = 0;
		virtual void Visit(const astr& val) = 0;
		virtual void Visit(const CGuid& val) = 0;
	};
	   
	typedef void(*constructorVariantFunc)(void *, const void *);
	typedef void(*destructorVariantFunc)(void *);
	typedef int(*compareVariantFunc)(const void *, const void *);
	typedef void(*copyVariantFunc)(void *, const void *);
	typedef void(*assignVariantFunc)(void *, int, const void *);
	typedef void(*acceptVariantFunc)(const void *, IVisitor &);


	template <int> struct SMaxSize {
		enum { maxSize = 4 };
	};

	template <typename T > struct Type2Int;

#define START_TYPE_LIST() \
	enum {_start_type_list_ = __LINE__ + 1 };

#define _curVal_ (__LINE__-_start_type_list_)

#define FINISH_TYPE_LIST() \
	enum {_type_list_count_ = _curVal_ };



#define DECLARE_TYPE(TT) \
	template<> struct SMaxSize<_curVal_>{ \
	enum { maxSize=(sizeof(TT) > SMaxSize<_curVal_-1>::maxSize) ? sizeof(TT) : SMaxSize<_curVal_-1>::maxSize }; \
	}; \
	template<> struct Type2Int<TT>{ \
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
	varType_##TT = Type2Int<TT>::typeId,

#define DECLARE_SIMPLE_TYPE(TT) DECLARE_TYPE(TT)

	//////////////DataType enum

	enum eDataTypes {
#include "VariantTypeList.h"
	};

#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE


	class CVariant
	{
	protected:
		eDataTypes m_id;

#define DECLARE_TYPE(TT)
#define DECLARE_SIMPLE_TYPE(TT) TT m_val##TT;
		union {
			char buffer[MAX_GV_SIZE];
#include "VariantTypeList.h"
			astr *val_str_t;
		}m_DataBuffer;

#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE

	public:

		eDataTypes GetTypeID() const
		{
			return m_id;
		}
		void *GetBuffer() { return &m_DataBuffer.buffer[0]; }
		const void *GetBuffer() const { return &m_DataBuffer.buffer[0]; }

		CVariant()
		{

			m_id = (eDataTypes)(int)Type2Int<CEmptyVariant>::typeId;
		}

		CVariant(const CVariant& v)
		{
			m_id = v.GetTypeID();
			var_constructors_[m_id](GetBuffer(), v.GetBuffer());

		}
		template <typename T>
		void SetType()
		{
			m_id = eDataTypes(Type2Int<T>::typeId);
		}

		template <typename T>
		CVariant(const T& v)
		{
			new(&m_DataBuffer.buffer[0]) T(v);
			m_id = eDataTypes(Type2Int<T>::typeId);
		}

		bool IsNull() const
		{
			return (m_id == eDataTypes(Type2Int<CEmptyVariant>::typeId));
		}

		template <typename T>
		bool IsType() const
		{
			return (m_id == eDataTypes(Type2Int<T>::typeId));
		}

		template <typename T>
		void IsTypeExc() const
		{
			if (m_id != eDataTypes(Type2Int<T>::typeId))
				throw CommonLib::CExcBase("Types don't match %1-%2", (int)m_id, (int)(Type2Int<T>::typeId));
		}


		CVariant& operator=(const CVariant& variant)
		{
			eDataTypes destId = variant.GetTypeID();
			if (m_id == destId)
				var_copy_[m_id](GetBuffer(), variant.GetBuffer());
			else
			{
				var_assign_[m_id](GetBuffer(), destId, variant.GetBuffer());
				m_id = destId;
			}
			return *this;
		}

		template <typename T>
		CVariant& operator = (const T& value)
		{
			eDataTypes destId = (eDataTypes)Type2Int<T>::typeId;
			if (m_id == destId)
				var_copy_[m_id](GetBuffer(), &value);
			else
			{
				var_assign_[m_id](GetBuffer(), destId, &value);
				m_id = destId;
			}
			return *this;
		}

		template <typename T>
		T& Get()
		{
			IsTypeExc<T>();

			return *(T *)GetBuffer();
		}

		template <typename T>
		const T& Get() const
		{
			IsTypeExc<T>();

			return *(T *)GetBuffer();
		}

		template <typename T>
		void GetVal(T& value) const
		{
			IsTypeExc<T>();

			value = *(T *)GetBuffer();
		}

		template <typename T>
		void SetVal(const T& value)
		{
			eDataTypes destId = (eDataTypes)Type2Int<T>::typeId;
			if (m_id == destId)
				var_copy_[m_id](GetBuffer(), &value);
			else
			{
				var_assign_[m_id](GetBuffer(), destId, &value);
				m_id = destId;
			}

		}

		template <typename T>
		T* GetPtr()
		{
			IsTypeExc<T>();

			return (T *)GetBuffer();
		}

		template <typename T>
		const T* GetPtr() const
		{
			IsTypeExc<T>();

			return (T *)GetBuffer();
		}
		void Accept(IVisitor& visitor) const
		{
			var_accept_[m_id](GetBuffer(), visitor);
		}
		int Compare(const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if (delta == 0)
				return var_compare_[m_id](GetBuffer(), variant.GetBuffer());
			return delta;
		}


		bool operator == (const CVariant& variant) const
		{
			if (GetTypeID() != variant.GetTypeID())
				return false;
			return (var_compare_[m_id](GetBuffer(), variant.GetBuffer()) == 0);
		}

		bool operator != (const CVariant& variant) const
		{
			if (GetTypeID() != variant.GetTypeID())
				return true;

			return (var_compare_[m_id](GetBuffer(), variant.GetBuffer()) != 0);
		}

		bool operator < (const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if (delta == 0)
				return (var_compare_[m_id](GetBuffer(), variant.GetBuffer()) < 0);

			return (delta < 0);
		}

		bool operator <= (const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if (delta == 0)
				return (var_compare_[m_id](GetBuffer(), variant.GetBuffer()) <= 0);

			return (delta <= 0);

		}

		bool operator > (const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if (delta == 0)
				return (var_compare_[m_id](GetBuffer(), variant.GetBuffer()) > 0);
			return (delta > 0);
		}

		bool operator >= (const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if (delta == 0)
				return (var_compare_[m_id](GetBuffer(), variant.GetBuffer()) >= 0);

			return (delta >= 0);
		}

	};
	class ToStringVisitor : public IVisitor
	{
	public:
		ToStringVisitor() {}
		operator astr()const { return m_val; };


		virtual void Visit(const CEmptyVariant&) { m_val = astr(); }
		virtual void Visit(const bool           & val)
		{
			m_val = val ? astr("true") : astr("false");
		}

		virtual void Visit(const byte_t& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const int8_t& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const int16_t& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const uint16_t& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const int32_t& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const uint32_t& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const uint64_t& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const int64_t& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const float& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const double& val)
		{
			m_val = CommonLib::str_utils::AStrFrom(val);
		}

		virtual void Visit(const astr& val)
		{
			m_val = val;
		}	

		virtual void Visit(const CGuid& val)
		{
			m_val = val.ToAstr(false);
		}

	private:
		astr m_val;

	};


	class ToStreamVisitor : public IVisitor
	{
	public:
		ToStreamVisitor(CommonLib::WriteStreamPtr ptrStream) : m_ptrStream(ptrStream)
		{}

		virtual void Visit(const CEmptyVariant&) {}

		virtual void Visit(const bool& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const byte_t& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const int8_t& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const int16_t& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const uint16_t& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const int32_t & val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const uint32_t& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const uint64_t& val)
		{
			m_ptrStream->Write(val);
		}
		virtual void Visit(const int64_t& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const float& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const double& val)
		{
			m_ptrStream->Write(val);
		}

		virtual void Visit(const astr& val)
		{
			m_ptrStream->Write(val);
		}
		
	private:
		CommonLib::WriteStreamPtr m_ptrStream;
	};
	
	template <class TVisitor>
	TVisitor apply_visitor(const CVariant& variant, TVisitor &visitor)
	{
		variant.Accept(visitor);
		return visitor;
	};

#undef START_TYPE_LIST
#undef FINISH_TYPE_LIST

}

