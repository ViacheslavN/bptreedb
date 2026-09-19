#pragma once
namespace CommonLib
{
	namespace network
	{
		class CAddrInfo
		{
		public:
			CAddrInfo();
			~CAddrInfo();

			addrinfo* Get();
			addrinfo** GetPP();
		private:
			addrinfo* m_addrinfo;
		};
		
	}
}