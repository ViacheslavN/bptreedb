#pragma once

#include "BaseSocketIP.h"

namespace CommonLib
{
	namespace network
	{
		typedef std::shared_ptr<class CClientSocket> CClientSocketPtr;

		class CClientSocket : public CBaseSocketIP
		{
		public:
			CClientSocket();
			virtual ~CClientSocket();


			void Connect(const CSocketAddr& addr);
			void ConnectToIP(const std::string& addr, unsigned short port);
			void ConnectToHost(const std::string& hostName, unsigned short port);

			const std::string& GetEndPoint() const;

		private:
            std::string m_connectdAddr;
		};
	}
}