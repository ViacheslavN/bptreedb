#include "stdafx.h"
#include "ServerSocket.h"
#include "AcceptedSocket.h"
#include "SocketUtil.h"

namespace CommonLib
{
	namespace network
	{
		CServerSocket::CServerSocket()
		{

		}

		CServerSocket::~CServerSocket()
		{

		}

		void CServerSocket::Listen(int backlog)
		{
			int retVal = listen(GetSocketExc(), backlog);
			CheckSocketRetVal(retVal, "Failed to listen");
		}

		void CServerSocket::Bind(unsigned short port)
		{
			try
			{
				SetExclusiveAddressOption();

				CSocketAddr addr;
				addr.SetFamily(AF_INET);
				addr.SetPort(port);
				addr.SetAddr(INADDR_ANY);

				int retVal = bind(GetSocketExc(), (struct sockaddr *)&addr.Get(), sizeof(struct sockaddr));
				CheckSocketRetVal(retVal, "bind");
				m_socketAddr = addr;

			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExcT("Failed to bind socket to port {0}", port, exc);
			}

		}

		void CServerSocket::Bind(unsigned short startPort, unsigned short endPort)
		{
			try
			{
				if (startPort > endPort)
					throw CExcBase("invalid range start {0}, end {1}", startPort, endPort);

				SetExclusiveAddressOption();
				CSocketAddr addr;
				addr.SetFamily(AF_INET);
				addr.SetAddr(INADDR_ANY);

				for (unsigned short i = startPort; i < endPort; ++i)
				{
					addr.SetPort(i);
					int retVal = bind(GetSocketExc(), (struct sockaddr *)&addr.Get(), sizeof(struct sockaddr));

					if (retVal != 0)
					{

#ifdef _WIN32      
						int error = WSAGetLastError();
						if (error == WSAEADDRINUSE || error == WSAEACCES)
							continue;

						throw CWinSockExc("failed to bind", error);

#else
						if (errno == EADDRINUSE)
							continue;

						throw CPosixExc("failed to bind", errno);
#endif
					}
				
					m_socketAddr = addr;
					break;
				}

			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExcT("Failed to bind socket from  {0} to {1}", startPort, endPort, exc);
			}
		}


		void CServerSocket::SetExclusiveAddressOption()
		{
#ifdef _WIN32
			//https://docs.microsoft.com/en-us/windows/win32/winsock/so-exclusiveaddruse
			int iOptval = 1;
			int retVal = setsockopt(GetSocketExc(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&iOptval, sizeof(iOptval));
			CheckSocketRetVal(retVal, "setsockopt for SO_EXCLUSIVEADDRUSE failed");
#endif
		}

		const CSocketAddr& CServerSocket::GetSocketAddr() const
		{
			return m_socketAddr;
		}

		int CServerSocket::GetPort() const
		{
			return m_socketAddr.Port();
		}

		std::shared_ptr<CAcceptedSocket>  CServerSocket::Accept()
		{
			try
			{
				sockaddr_in sockAddrOther;
				socklen_t len = sizeof(sockaddr_in);
				SocketType acceptedSocket = accept(m_socket, (sockaddr*)&sockAddrOther, &len);

				CSocketUtil::CheckSocketExc(acceptedSocket);

				return AcceptedSocketPtr(new CAcceptedSocket(acceptedSocket, CSocketAddr::GetAddres(sockAddrOther.sin_addr, sockAddrOther.sin_port)));

			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExc("Failed to accept", exc);
				throw;
			}
		}
	}
}