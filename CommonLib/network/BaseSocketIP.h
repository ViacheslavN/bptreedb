#pragma once

#include "../crypto/IDataCipher.h"
#include "../stream/io.h"
#include "SocketAddr.h"

#ifdef _WIN32
#include "../exception/WinSockExc.h"
namespace CommonLib
{
	typedef CWinSockExc CSocketExeption;
}

#else
#include "../exception/PosixExc.h"
#include "CommonLib.h"

namespace CommonLib
{
	typedef CPosixExc CSocketExeption;
}
#endif



#ifdef _WIN32

#define  CheckSocketRetVal(retVal, msg) \
	if(retVal == SOCKET_ERROR) \
			throw CWinSockExc(msg);
#else

#define  CheckSocketRetVal(retVal, msg) \
	if(retVal == -1) \
			throw CPosixExc(msg, errno);

#endif

#ifdef _WIN32
typedef SOCKET SocketType;
#else
typedef int SocketType;
#endif

namespace CommonLib
{
	namespace network
	{
 
		typedef std::shared_ptr<class CBaseSocketIP> CSocketIPPtr;

		class CBaseSocketIP : public io::IWriteRead, io::IWriteReadNotBlocking
		{
			public:
				enum OpenType
				{
					SOCKSTREAM,
					SOCKDGRAM
				};


				CBaseSocketIP();
				CBaseSocketIP(SocketType socket);

				virtual ~CBaseSocketIP();

				virtual std::streamsize Write(const byte_t* dataPtr, size_t dataSize);
				virtual std::streamsize Read(byte_t* dataPtr, size_t dataSize);

				virtual std::streamsize WriteNotBlocking(const byte_t* dataPtr, size_t dataSize);
				virtual std::streamsize ReadNotBlocking(byte_t* dataPtr, size_t dataSize);

				void OpenSocket(OpenType type);
				void Close();
				void Ioctl(long cmd, unsigned long* arg);
				void Setsockopt(int level,int optname, const char*  optval, int optlen);

				void SetSocketBlockMode(bool block);
				void SetKeepAlive(bool enable, unsigned long time, unsigned long interval);
				void SetNoDelay(bool enable);

				void Send(const byte_t* data, size_t count, long timeout = 0);
				void Recv(byte_t* data, size_t count, long timeout = 0);
				bool SendNotBlocking(const byte_t* data, size_t count, size_t& sent);
				bool RecvNotBlocking(byte_t* data, size_t count, size_t& recvdata);

#ifdef _WIN32
				void SelecEvent(HANDLE hEvent, long networkEvents);

#endif
			protected:
                SocketType GetSocketExc();

			protected:
                SocketType m_socket;
 
		};
	}
}