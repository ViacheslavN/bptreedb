#include "stdafx.h"
#include "AcceptedSocket.h"


namespace CommonLib
{
	namespace network
	{

		CAcceptedSocket::CAcceptedSocket(SocketType socket, const std::string& connectedAddr) : CBaseSocketIP( socket),
			m_connectedAddr(connectedAddr)
		{

		}

		CAcceptedSocket::~CAcceptedSocket()
		{

		}

		const std::string CAcceptedSocket::GetClientAddr() const
		{
			return m_connectedAddr;
		}
	}
}