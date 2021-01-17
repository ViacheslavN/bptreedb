#pragma once

namespace bptreedb
{

	struct StringValue
	{

		StringValue();
		StringValue(const byte_t *pStr, uint32_t len, CommonLib::IAllocPtr pAlloc);
		StringValue(uint32_t len, CommonLib::IAllocPtr pAlloc);
		~StringValue();


		StringValue(const StringValue&);
		StringValue( StringValue&&);

		StringValue& operator= (const StringValue&);
		StringValue& operator= (StringValue&&);

		uint32_t Length() const;
		const byte_t * CStr() const;
		byte_t * Str();
	private:
		void Clear();
		void CopyStr(const byte_t *pStr, uint32_t len);
		byte_t *m_utf8{ nullptr };
		uint32_t m_nLen{ 0 };
		CommonLib::IAllocPtr m_pAlloc;
		

	};

	struct CompStringValue
	{


		int StrValcmp(const byte_t *pszLeft, const byte_t *pszRight) const
		{
			while (*pszLeft && *pszRight && *pszLeft == *pszRight)
				++pszLeft, ++pszRight;
			return *pszLeft - *pszRight;
		}

		bool LE(const StringValue& _Left, const StringValue& _Right) const
		{
			return StrValcmp(_Left.CStr(), _Right.CStr()) < 0;
		}
		bool EQ(const StringValue& _Left, const StringValue& _Right) const
		{
			if (_Left.Length() != _Right.Length())
				return false;

			return StrValcmp(_Left.CStr(), _Right.CStr()) == 0;
		}

		bool operator() (const StringValue& _Left, const StringValue& _Right) const
		{
			return StrValcmp(_Left.CStr(), _Right.CStr()) < 0;
		}

	};
}