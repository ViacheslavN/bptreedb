#include "stdafx.h"

#include "../exception/exc_base.h"

#ifdef _WIN32
	#include "../exception/WinSockExc.h"
#else
	#include "../exception/PosixExc.h"
#endif

#include "SocketUtil.h"

namespace CommonLib
{
	namespace network
	{
		SocketType CSocketUtil::GetInvalidSocket()
		{

#ifdef _WIN32
			return INVALID_SOCKET;
#else
			return -1;
#endif
		}

		void CSocketUtil::CheckSocketExc(SocketType socket)
		{
			if (socket == GetInvalidSocket())
#ifdef _WIN32
				throw CWinSockExc();
#else
				throw CPosixExc(errno);
#endif
		}

		int  CSocketUtil::GetSocketLastError()
		{
#ifdef _WIN32
			return WSAGetLastError();
#else
			return errno;
#endif
		}

		bool CSocketUtil::IsEINTR(int error)
		{
#ifdef _WIN32
			return error == WSAEINTR;
#else
			return error == EINTR;
#endif
		}

		bool CSocketUtil::IsSocketError(int retVal)
		{
#ifdef _WIN32
			return retVal == SOCKET_ERROR;
#else
			return retVal == -1;
#endif
		}

		bool CSocketUtil::IsBlockingError(int error)
		{
#ifdef _WIN32
			return error == WSAEWOULDBLOCK;
#else
			return error == EAGAIN || error == EWOULDBLOCK;
#endif
		}

		bool CSocketUtil::IsConnectAborted(int error)
		{
#ifdef _WIN32
			return error == WSAECONNABORTED;
#else
			return false;
#endif
		}

	}
}