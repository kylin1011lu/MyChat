 
#ifndef __NETWORK_H__
#define __NETWORK_H__

#ifdef LINUX
	#define SOCKET_ERROR -1
	#define INVALID_SOCKET -1
#endif

#ifndef SHUT_RD
	#define SHUT_RD 0
#endif

#ifndef SHUT_WR
	#define SHUT_WR 1
#endif

#ifndef SHUT_RDWR
	#define SHUT_RDWR 2
#endif

#ifdef LINUX
#define ERRNO   errno
#endif
#ifdef WIN32
#define ERRNO   WSAGetLastError()
#endif

//#define MAX_NETWORK_PACKAGE_SIZE	4*KB

/*
struct sq_network_address
{
	uint32_t					ip;
	uint16_t					port;
};
*/

bool setblocking(SOCKET fd, bool block);
bool interrupted();
bool wouldblock();
bool connectionlost();

#endif
