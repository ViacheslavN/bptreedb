#include "stdafx.h"
#ifdef _WIN32
#include <mstcpip.h>
#else
#include <netinet/tcp.h>
#endif
#include "BaseSocketIP.h"
#include "SocketUtil.h"
#include "CloseConnectionExc.h"
#include "OperationTimeoutExc.h"
#include "CommonLib.h"

namespace CommonLib
{
	namespace network
	{
		CBaseSocketIP::CBaseSocketIP() 
		{
			m_socket = CSocketUtil::GetInvalidSocket();
		}

		CBaseSocketIP::CBaseSocketIP(SocketType socket) : m_socket(socket)
		{

		}

		CBaseSocketIP::~CBaseSocketIP()
		{

		}

		void CBaseSocketIP::OpenSocket(OpenType type)
		{
			try
			{
				if (type == SOCKSTREAM)
				{
					m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				}
				else
				{
					m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				}

				CSocketUtil::CheckSocketExc(m_socket);
			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExcT("Failed to create socket type {0}", type, exc);
			}
		}

		void CBaseSocketIP::Close()
		{
			if (m_socket != CSocketUtil::GetInvalidSocket())
			{
#ifdef _WIN32
				closesocket(m_socket);
#else
				close(m_socket);
#endif
				m_socket = CSocketUtil::GetInvalidSocket();
			}
		}

		SocketType CBaseSocketIP::GetSocketExc()
		{
			if (m_socket == CSocketUtil::GetInvalidSocket())
				throw CExcBase("invalid socket");

			return m_socket;
		}

		void CBaseSocketIP::Ioctl(long cmd, unsigned long* arg)
		{
#ifdef _WIN32
			int retVal = ioctlsocket(GetSocketExc(), cmd, arg);
#else
			int retVal = ioctl(GetSocketExc(), cmd, arg);
#endif // _WIN32

			CheckSocketRetVal(retVal, "Ioctl failed");
		}

		void CBaseSocketIP::Setsockopt(int level, int optname, const char*  optval, int optlen)
		{
			int retVal = setsockopt(GetSocketExc(), level, optname, optval, optlen);
			CheckSocketRetVal(retVal, "setsockopt failed");
		}

		void CBaseSocketIP::SetSocketBlockMode(bool block)
		{
			try
			{
				unsigned long iMode = block ? 0 : 1;
				Ioctl(FIONBIO, &iMode);
			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExcT("Failed to set blockmode  {0} ", block ? 1 : 0, exc);
			}
		}

		void CBaseSocketIP::SetNoDelay(bool enable)
		{
			int val = !enable;
			int retVal = setsockopt(GetSocketExc(), IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof(val));
			CheckSocketRetVal(retVal, "SetNoDelay")
		}

		void CBaseSocketIP::SetKeepAlive(bool enable, unsigned long time, unsigned long interval)
		{
#ifdef _WIN32
			struct tcp_keepalive alive;
			alive.onoff = enable ? TRUE : FALSE;
			alive.keepalivetime = time;
			alive.keepaliveinterval = interval;
			DWORD bytes_returned;
			int retVal = ::WSAIoctl(GetSocketExc(), SIO_KEEPALIVE_VALS, &alive, sizeof(alive),	NULL, 0, &bytes_returned, NULL, NULL);
			
#else
			int optval = enable ? 1 : 0;
			int idleTime = int(time / 1000);
			int keepaliveinterval = int(interval / 1000);
			int cnt = 5;
			int retVal = setsockopt(GetSocketExc(), SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
			if (retVal == 0)
				retVal = setsockopt(GetSocketExc(), SOL_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt));
			if (retVal == 0)
				retVal = setsockopt(GetSocketExc(), SOL_TCP, TCP_KEEPIDLE, &idleTime, sizeof(idleTime));
			if (retVal == 0)
				retVal = setsockopt(GetSocketExc(), SOL_TCP, TCP_KEEPINTVL, &keepaliveinterval, sizeof(keepaliveinterval));

#endif

			CheckSocketRetVal(retVal, "SetKeepAlive")
		}
			   		 
#ifdef _WIN32
		void CBaseSocketIP::SelecEvent(HANDLE hEvent, long networkEvents)
		{
			int retVal = WSAEventSelect(GetSocketExc(), hEvent, networkEvents);
			CheckSocketRetVal(retVal, "SelecEvent failed")
		}
#endif
		void CBaseSocketIP::Send(const byte_t* data, size_t count, long timeout)
		{
			char* posInBuf = (char*)data;
			size_t writeSize = count;

			while (true)
			{
				fd_set clSet;
				FD_ZERO(&clSet);
				FD_SET(m_socket, &clSet);
				timeval tv = { 0 };
				tv.tv_sec = timeout;

				int selRet;
				while (true)
				{
					selRet = select((int)m_socket + 1, 0, &clSet, 0, timeout ? &tv : 0);
					int err = CSocketUtil::GetSocketLastError();
					if ((selRet == -1) && (CSocketUtil::IsEINTR(err)))
						continue;

					break;
				}

				if (selRet == 0)
					throw COperationTimeout(timeout, "Socket timed out ({0} sec).", timeout);

				CheckSocketRetVal(selRet, "select socket");

				const int retVal = send(m_socket, posInBuf, (int)writeSize, 0);
				if (CSocketUtil::IsSocketError(retVal))
				{
					int error = CSocketUtil::GetSocketLastError();

					if (CSocketUtil::IsConnectAborted(error))
						throw CCloseConnection();

					if (CSocketUtil::IsBlockingError(error))
					{
						synch::CThread::Sleep(500);
						continue;
					}

					throw CSocketExeption(error, "Failed to read");
				}

				if (retVal == writeSize)
					break;

				writeSize -= retVal;
				posInBuf += retVal;
			}
		}

		void CBaseSocketIP::Recv(byte_t* data, size_t count, long timeout)
		{
			char* posInBuf = (char*)data;
			size_t readSize = count;

			while (true)
			{
				fd_set writeSet;
				FD_ZERO(&writeSet);
				FD_SET(m_socket, &writeSet);
				timeval tv = { 0 };
				tv.tv_sec = timeout;

				int selRet;
				while (true)
				{
					selRet = select((int)m_socket + 1, &writeSet, 0, 0, timeout ? &tv : 0);
					int err = CSocketUtil::GetSocketLastError();

					if ((selRet == -1) && (CSocketUtil::IsEINTR(err)))
						continue;

					break;
				}

				if (selRet == 0)
					throw COperationTimeout(timeout, "Socket timed out ({0} sec).", timeout);

				CheckSocketRetVal(selRet, "select socket");

				int retVal = recv(m_socket, posInBuf, (int)readSize, 0);

				CheckSocketRetVal(retVal, "recv");

				if (retVal == 0)
					throw CCloseConnection();

				if (retVal == readSize)
					break;

				readSize -= retVal;
				posInBuf += retVal;
			}
		}

		bool CBaseSocketIP::SendNotBlocking(const byte_t* data, size_t count, size_t& sent)
		{
			sent = 0;
			int retVal = send(GetSocketExc(), (char*)data, (int)count, 0);

			if (CSocketUtil::IsSocketError(retVal))
			{
				int error = CSocketUtil::GetSocketLastError();
				if (CSocketUtil::IsBlockingError(error))
					return false;

				if (CSocketUtil::IsConnectAborted(error))
					throw CCloseConnection();

				throw CSocketExeption(error, "failed to send not blocking");
			}

			sent = retVal;
			return true;
		}

		bool CBaseSocketIP::RecvNotBlocking(byte_t* data, size_t count, size_t& recvdata)
		{
			recvdata = 0;
			int retVal = recv(GetSocketExc(), (char*)data, (int)count, 0);
			
			if (retVal == 0)
				throw CCloseConnection();

			if (CSocketUtil::IsSocketError(retVal))
			{
				int error = CSocketUtil::GetSocketLastError();
				if (CSocketUtil::IsBlockingError(error))
					return false;

				if (CSocketUtil::IsConnectAborted(error))
					throw CCloseConnection();

				throw CSocketExeption(error, "failed to recv not blocking");
			}

			recvdata = retVal;
			return true;
		}

		std::streamsize CBaseSocketIP::Write(const byte_t* dataPtr, size_t dataSize)
		{
			try
			{
				Send(dataPtr, dataSize);
				return dataSize;
			}
			catch (std::exception& excSrc)
			{
				CExcBase::RegenExc("Failed to write to socket", excSrc);
				throw;
			}

		}

		std::streamsize CBaseSocketIP::Read(byte_t* dataPtr, size_t dataSize)
		{
			try
			{
				Recv(dataPtr, dataSize);
				return dataSize;
			}
			catch (std::exception& excSrc)
			{
				CExcBase::RegenExc("Failed to read from socket", excSrc);
				throw;
			}
		}

		std::streamsize CBaseSocketIP::WriteNotBlocking(const byte_t* dataPtr, size_t dataSize)
		{
			try
			{
				size_t sendSize;
				SendNotBlocking(dataPtr, dataSize, sendSize);

				return (std::streamsize)sendSize;
			}
			catch (std::exception& excSrc)
			{
				CExcBase::RegenExc("Failed to write non blocking to socket", excSrc);
				throw;
			}
		}

		std::streamsize CBaseSocketIP::ReadNotBlocking(byte_t* dataPtr, size_t dataSize)
		{
			try
			{
				size_t readSize;
				RecvNotBlocking(dataPtr, dataSize, readSize);

				return (std::streamsize)readSize;
			}
			catch (std::exception& excSrc)
			{
				CExcBase::RegenExc("Failed to read non blocking from socket", excSrc);
				throw;
			}
		}
	}
}