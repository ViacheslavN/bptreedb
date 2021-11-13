#pragma once
#include "bptreedb.h"

namespace bptreedb
{

	template<typename T>
	inline eFieldDataType CFieldTypeId()
	{
		return fdtUnknown;
	}

	template<>
	inline eFieldDataType CFieldTypeId<bool>()
	{
		return fdtBool;
	}

	template<>
	inline eFieldDataType CFieldTypeId<int8_t>()
	{
		return fdtInteger8;
	}

	template<>
	inline eFieldDataType CFieldTypeId<uint8_t>()
	{
		return fdtUInteger8;
	}

	template<>
	inline eFieldDataType CFieldTypeId<int16_t>()
	{
		return fdtInteger16;
	}

	template<>
	inline eFieldDataType CFieldTypeId<uint16_t>()
	{
		return fdtUInteger16;
	}

	template<>
	inline eFieldDataType CFieldTypeId<int32_t>()
	{
		return fdtInteger32;
	}

	template<>
	inline eFieldDataType CFieldTypeId<uint32_t>()
	{
		return fdtUInteger32;
	}

	template<>
	inline eFieldDataType CFieldTypeId<int64_t>()
	{
		return fdtInteger64;
	}

	template<>
	inline eFieldDataType CFieldTypeId<uint64_t>()
	{
		return fdtUInteger64;
	}

	template<>
	inline eFieldDataType CFieldTypeId<float>()
	{
		return fdtFloat;
	}

	template<>
	inline eFieldDataType CFieldTypeId<double>()
	{
		return fdtDouble;
	}

	template<>
	inline eFieldDataType CFieldTypeId<astr>()
	{
		return fdtString;
	}

	template<typename _TValue>
	class CValueBaseT : public IValue
	{
	public:
		typedef _TValue TValue;

		virtual bool IsNull() const
		{
			return 
		}

		virtual eFieldDataType GetDataType() const
		{
			return CFieldTypeId<TValue>();
		}

		virtual void Get(uint8_t& val);
		virtual void Get(uint16_t& val);
		virtual void Get(uint32_t& val);
		virtual void Get(uint64_t& val);
		virtual void Get(int8_t& val);
		virtual void Get(int16_t& val);
		virtual void Get(int32_t& val);
		virtual void Get(int64_t& val);
		virtual void Get(double& val);
		virtual void Get(float& val);
		virtual void Get(astr& val);

		virtual void Set(const uint8_t& val);
		virtual void Set(const uint16_t& val);
		virtual void Set(const uint32_t& val);
		virtual void Set(const uint64_t& val);
		virtual void Set(const int8_t& val);
		virtual void Set(const int16_t& val);
		virtual void Set(const int32_t& val);
		virtual void Set(const int64_t& val);
		virtual void Set(const double& val);
		virtual void Set(const float& val);
		virtual void Set(const astr& val);

		template <class T>
		void GetT(T& val)
		{
			Get(val);
		}

		template <class T>
		void SetT(const T& val)
		{
			Set(val);
		}


		protected:
			eFieldDataType m_filedType{ fdtUnknown };
			bool m_IsNull{ true };

	};
}