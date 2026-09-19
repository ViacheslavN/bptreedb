#include "stdafx.h"
#include "GuidLin.h"
#include "../str/StringEncoding.h"

namespace CommonLib
{
	namespace lin
	{
		CGuid::CGuid()
		{
			uuid_clear(m_guid);
		}

		CGuid::CGuid(const CGuid &gid)
		{
			uuid_copy(m_guid, gid.m_guid);
		}

		CGuid::CGuid(const uuid_t gid)
		{
			uuid_copy(m_guid, gid);
		}

		CGuid::CGuid(const std::string &gid)
		{
			FromAstr(gid);
		}

		CGuid::CGuid(const std::wstring &gid)
		{
			FromWstr(gid);
		}

		void CGuid::FromAstr(const std::string& gid)
		{
			const char* pStr = gid.c_str();
			if (pStr != nullptr)
			{
				if (*pStr == '{')
					++pStr;
			}

			uuid_parse(pStr, m_guid);
		
		}

		void CGuid::FromWstr(const std::wstring& gid)
		{
			FromAstr(StringEncoding::str_w2utf8_safe(gid));
		}

        std::string CGuid::ToAstr(bool withbrackets) const
		{
			char str[37];
			uuid_unparse(m_guid, str);
 
			if(withbrackets)
				return std::string("{") + str + std::string("}");

			return str;
		}

        std::wstring CGuid::ToWstr(bool withbrackets) const
		{
			return StringEncoding::str_utf82w_safe(ToAstr(withbrackets));
		}

		CGuid CGuid::CreateNew()
		{
			CGuid guid;
			uuid_generate(guid.m_guid);

			return guid;
		}

		CGuid  CGuid::CreateNull()
		{
			return CGuid();
		}

        CGuid CGuid::CreateFromAstr(const std::string& guid)
        {
            return CGuid(guid);
        }

		CGuid& CGuid::operator =(const CGuid& gid)
		{
			uuid_copy(m_guid, gid.m_guid);
			return *this;
		}

		CGuid::~CGuid()
		{

		}

		bool CGuid::operator ==(const CGuid &gid) const
		{
			return uuid_compare(m_guid, gid.m_guid) == 0;	 
		}

		bool CGuid::operator !=(const CGuid &gid) const
		{
			return uuid_compare(m_guid, gid.m_guid) != 0;
		}

		bool CGuid::operator <(const CGuid &gid) const
		{
			return uuid_compare(m_guid, gid.m_guid) < 0;
		}

		bool CGuid::operator >(const CGuid &gid) const
		{
			return uuid_compare(m_guid, gid.m_guid) > 0;
		}

		bool CGuid::operator <=(const CGuid &gid) const
		{
			return uuid_compare(m_guid, gid.m_guid) <= 0;
		}

		bool CGuid::operator >=(const CGuid &gid) const
		{
			return uuid_compare(m_guid, gid.m_guid) >= 0;
		}
	}
}