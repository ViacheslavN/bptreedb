#include "stdafx.h"
#include "AddrInfo.h"

namespace CommonLib
{
	namespace network
	{
		CAddrInfo::CAddrInfo() : m_addrinfo(nullptr)
		{}

		CAddrInfo::~CAddrInfo()
		{
			if (m_addrinfo)
				freeaddrinfo(m_addrinfo);
		}

		addrinfo* CAddrInfo::Get()
		{
			return m_addrinfo;
		}

		addrinfo** CAddrInfo::GetPP()
		{
			return &m_addrinfo;
		}
	}
}