#pragma once

#include "BaseSocketIP.h"

namespace CommonLib
{
	namespace network
	{
		typedef std::shared_ptr<class CAcceptedSocket> AcceptedSocketPtr;

		class CAcceptedSocket : public CBaseSocketIP
		{
		public:
			CAcceptedSocket(SocketType socket, const std::string& connectedAddr);
			virtual ~CAcceptedSocket();
			const std::string GetClientAddr() const;

		private:
            std::string m_connectedAddr;

		};
	}
}