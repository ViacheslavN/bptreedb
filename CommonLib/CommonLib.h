#pragma once


#ifdef _WIN32
	#include <Winsock2.h>
	#include <ws2tcpip.h>
	#include <windows.h>
	#include <wininet.h>
	#include <process.h>
	#include <rpc.h>
	#include <Objbase.h>
	#include <bcrypt.h>

	#pragma comment(lib, "bcrypt.lib")
#else
	#include <stdio.h>
	#include <dirent.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <time.h>
	#include <wchar.h>
	#include <pthread.h>
	#include <mutex>
	#include <sys/ioctl.h>
	#include <sys/time.h>
	#include <sys/ioctl.h>
	#include <sys/types.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
//	#include <uuid/uuid.h>
#endif

#include <stdint.h>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <set>
#include <sstream>
#include <time.h>
#include <functional>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <iterator>
#include <cstring>
#include <stack>
#include <algorithm>



typedef uint8_t byte_t;



#ifdef _WIN32
	typedef int thread_id_t;
#else
	typedef pthread_t thread_id_t;
    	typedef struct tagSIZE
	{
		long        cx;
		long        cy;
	} SIZE, *PSIZE, *LPSIZE;
#endif