
#include "stdafx.h"
#include "../str/convertToString.h"
#include "ClientSocket.h"
#include "AddrInfo.h"

namespace CommonLib
{
	namespace network
	{
		CClientSocket::CClientSocket()
		{

		}

		CClientSocket::~CClientSocket()
		{

		}


		void CClientSocket::Connect(const CSocketAddr& addr)
		{
			int retVal = connect(m_socket, (struct sockaddr*)&addr.Get(), sizeof(sockaddr_in));
			CheckSocketRetVal(retVal, "Failed to connect");

			m_connectdAddr = addr.GetAddres();
		}

		void CClientSocket::ConnectToIP(const std::string& addr, unsigned short port)
		{
			CSocketAddr sockAddr;
			sockAddr.SetFamily(AF_INET);
			sockAddr.SetPort(port);

			struct in_addr addr4;
			inet_pton(AF_INET, addr.c_str(), &addr4);
			sockAddr.SetAddrIn(addr4);

			Connect(sockAddr);
		}

		void CClientSocket::ConnectToHost(const std::string& hostName, unsigned short port)
		{
			addrinfo hints;
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET; 
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

            std::string strPort = str_utils::AStrUint16(port);

			CAddrInfo addrInfo;

			int retVal = getaddrinfo(hostName.c_str(), strPort.c_str(), &hints, addrInfo.GetPP());
			CheckSocketRetVal(retVal, "failed to get getaddrinfo");


			addrinfo* pAddrInfo = addrInfo.Get();
            std::string errors;
			while (pAddrInfo)
			{
				if ((pAddrInfo->ai_family == AF_INET)&& (pAddrInfo->ai_socktype == SOCK_STREAM) && (pAddrInfo->ai_protocol == IPPROTO_TCP))
				{
					try
					{
						retVal = connect(m_socket, pAddrInfo->ai_addr, sizeof(sockaddr_in));
						CheckSocketRetVal(retVal, "failed to connect");
						m_connectdAddr = hostName + std::string(":") + str_utils::AStrUint16(port);
						return;
					}
					catch (std::exception& excSrc)
					{
						errors += excSrc.what();
					}
				}
				pAddrInfo = pAddrInfo->ai_next;
			}

			throw CExcBase("Cannot connect to host {0}. Errors: {1}", hostName, errors);
		}
	}
}