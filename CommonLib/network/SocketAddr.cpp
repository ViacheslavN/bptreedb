#include "stdafx.h"
#include "SocketAddr.h"
#include "../str/str.h"

namespace CommonLib
{
	namespace network
	{
		CSocketAddr::CSocketAddr()
		{
			m_sockaddr.sin_port = -1;
		}

		CSocketAddr::CSocketAddr(const sockaddr_in& addr)
		{
			m_sockaddr.sin_family = addr.sin_family;
			m_sockaddr.sin_port = addr.sin_port;
			m_sockaddr.sin_addr = addr.sin_addr;
			//memcpy(m_sockaddr.sin_zero, addr.sin_zero, sizeof(m_sockaddr.sin_zero));
		}

		CSocketAddr::CSocketAddr(const in_addr& addr)
		{
			m_sockaddr.sin_family = AF_INET;
			m_sockaddr.sin_port = 0;
			m_sockaddr.sin_addr = addr;
			//memset(m_sockaddr.sin_zero, 0, sizeof(m_sockaddr.sin_zero));
		}

		CSocketAddr::CSocketAddr(const in_addr& addr, unsigned short port)
		{
			m_sockaddr.sin_family = AF_INET;
			m_sockaddr.sin_port = htons(port);
			m_sockaddr.sin_addr = addr;
		}

		CSocketAddr::~CSocketAddr()
		{

		}

		CSocketAddr & CSocketAddr::operator = (const sockaddr_in & other)
		{
			m_sockaddr.sin_family = other.sin_family;
			m_sockaddr.sin_port = other.sin_port;
			m_sockaddr.sin_addr = other.sin_addr;

			return *this;
		}

		CSocketAddr & CSocketAddr::operator = (const in_addr & address)
		{
			m_sockaddr.sin_family = AF_INET;
			m_sockaddr.sin_port = 0;
			m_sockaddr.sin_addr = address;

			return *this;
		}

		in_addr CSocketAddr::Address() const
		{
			return m_sockaddr.sin_addr;
		}

		unsigned short CSocketAddr::Port() const
		{
			return ntohs(m_sockaddr.sin_port);
		}

		void CSocketAddr::SetPort(unsigned short port)
		{
			m_sockaddr.sin_port = htons(port);
		}

		void CSocketAddr::SetFamily(unsigned short family)
		{
			m_sockaddr.sin_family = family;
		}

		void CSocketAddr::SetAddr(unsigned long addr)
		{
			m_sockaddr.sin_addr.s_addr = addr;
		}

		void CSocketAddr::SetAddrIn(const in_addr & addr)
		{
			m_sockaddr.sin_addr = addr;
		}

        std::string CSocketAddr::GetAddres() const
		{
			return GetAddres(m_sockaddr.sin_addr, m_sockaddr.sin_port);
		}

        std::string CSocketAddr::GetAddres(const in_addr& sin_addr, unsigned short port)
		{
			return str_format::AStrFormatSafeT("{0}.{1}.{2}.{3}:{4}", int(sin_addr.s_addr & 0xFF),
				int((sin_addr.s_addr & 0xFF00) >> 8),
				int((sin_addr.s_addr & 0xFF0000) >> 16),
				int((sin_addr.s_addr & 0xFF000000) >> 24),
				htons(port));
		}
	}
}