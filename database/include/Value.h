#pragma once
#include "bptreedb.h"

namespace bptreedb
{
	class CValue : public IValue
	{
	public:
		virtual bool IsNull() const;
		virtual eFieldDataType GetDataType() const;

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

			union 

	};
}