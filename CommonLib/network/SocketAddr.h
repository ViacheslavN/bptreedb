#pragma once


namespace CommonLib
{
	namespace network
	{
		class CSocketAddr
		{

			public:
				CSocketAddr();
				CSocketAddr(const sockaddr_in& addr);
				CSocketAddr(const in_addr& addr);
				CSocketAddr(const in_addr& addr, unsigned short port);
				~CSocketAddr();

				CSocketAddr & operator = (const sockaddr_in & other);
				CSocketAddr & operator = (const in_addr & address);


				const sockaddr_in& Get() const { return m_sockaddr; }
				sockaddr_in& Get()  { return m_sockaddr; }

				operator sockaddr_in* ()
				{
					return &m_sockaddr;
				}

				operator const sockaddr_in* () const
				{
					return &m_sockaddr;
				}

				void SetPort(unsigned short port);
				void SetFamily(unsigned short family);
				void SetAddr(unsigned long addr);
				void SetAddrIn(const in_addr & addr);

				in_addr Address() const;
				unsigned short Port() const;

                std::string GetAddres() const;
				static std::string GetAddres(const in_addr& sin_addr, unsigned short port);

			protected:
				sockaddr_in m_sockaddr;
 		};
	}
}