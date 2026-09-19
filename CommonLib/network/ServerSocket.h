#pragma once

#include "BaseSocketIP.h"

namespace CommonLib
{
	namespace network
	{

		class CAcceptedSocket;

		class CServerSocket : public CBaseSocketIP
		{
			public:
				CServerSocket();
				virtual ~CServerSocket();

				void Listen(int backlog = SOMAXCONN);
				void Bind(unsigned short port);
				void Bind(unsigned short startPort, unsigned short endPort);
				std::shared_ptr<CAcceptedSocket> Accept();

				const CSocketAddr& GetSocketAddr() const;
				int GetPort() const;

		protected:
			void SetExclusiveAddressOption();
			CSocketAddr m_socketAddr;

		};
	}
}