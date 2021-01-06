#pragma once

namespace bptreedb
{

	struct StringValue
	{
		byte_t *m_utf8;
		uint32_t m_nLen;

		StringValue() : m_utf8(0), m_nLen(0)
		{}


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
			return StrValcmp(_Left.m_utf8, _Right.m_utf8) < 0;
		}
		bool EQ(const StringValue& _Left, const StringValue& _Right) const
		{
			if (_Left.m_nLen != _Right.m_nLen)
				return false;

			return StrValcmp(_Left.m_utf8, _Right.m_utf8) == 0;
		}

	};
}