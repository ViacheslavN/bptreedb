#include "stdafx.h"
#include "StringVal.h"

namespace bptreedb
{

	StringValue::StringValue() : m_utf8(nullptr), m_nLen(0)
	{

	}


	StringValue::StringValue(const byte_t *pStr, uint32_t len, CommonLib::IAllocPtr pAlloc)
	{
		m_pAlloc = pAlloc;
		CopyStr(pStr, len);
	}

	StringValue::StringValue(uint32_t len, CommonLib::IAllocPtr pAlloc)
	{
		m_pAlloc = pAlloc;
		m_nLen = len;

		m_utf8 = (byte_t*)m_pAlloc->Alloc(len);
	}


	StringValue::~StringValue()
	{
		Clear();
	}

	void StringValue::Clear()
	{
		if (m_pAlloc.get() && m_utf8 != nullptr)
		{
			m_pAlloc->Free(m_utf8);
			m_utf8 = nullptr;
			m_nLen = 0;
		}
	}

	void  StringValue::CopyStr(const byte_t *pStr, uint32_t len)
	{
		m_nLen = len;
		uint32_t copyLen = len;
		if (m_nLen != 0 && m_pAlloc.get() != nullptr)
		{
			if (pStr[len - 1] != '\0')
			{
				m_nLen = len + 1;
			}

			m_utf8 = (byte_t*)m_pAlloc->Alloc(m_nLen);
			memcpy(m_utf8, pStr, copyLen);
			m_utf8[m_nLen - 1] = '\0';

		}
	}

	uint32_t StringValue::Length() const
	{
		return m_nLen;
	}

	const byte_t * StringValue::CStr() const
	{
		return m_utf8;
	}

	byte_t * StringValue::Str()
	{
		return m_utf8;
	}

	StringValue::StringValue(const StringValue& val)
	{
		m_pAlloc = val.m_pAlloc;
		CopyStr(val.m_utf8, val.m_nLen);
		
	}

	StringValue::StringValue( StringValue&& val)
	{
		m_pAlloc = val.m_pAlloc;
		m_nLen = val.m_nLen;
		m_utf8 = val.m_utf8;

		val.m_utf8 = nullptr;
		val.m_nLen = 0;
	}

	StringValue& StringValue::operator= (const StringValue& val)
	{
		Clear();
		m_pAlloc = val.m_pAlloc;
		CopyStr(val.m_utf8, val.m_nLen);

		return *this;
	}

	StringValue& StringValue::operator= (StringValue&& val)
	{
		Clear();
		m_pAlloc = val.m_pAlloc;
		m_nLen = val.m_nLen;
		m_utf8 = val.m_utf8;

		val.m_utf8 = nullptr;
		val.m_nLen = 0;

		return *this;
	}
}