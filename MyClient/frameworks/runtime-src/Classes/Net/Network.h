/*
* file: TCPClient.h
* brief: Created by kylin
*/
 
#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stdint.h>
#include <cstdlib>

#include "cocos2d.h"
#include "Definition/Define.h"

NS_MY_BEGIN
#ifndef WIN32
	#define SOCKET int
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

#ifdef WIN32
#define close	closesocket
#endif

#ifdef WIN32
typedef long ssize_t;
#endif

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID && CC_TARGET_PLATFORM != CC_PLATFORM_LINUX)
#define TEMP_FAILURE_RETRY(FUNCTION) FUNCTION
#endif

bool setblocking(SOCKET fd, bool block);
bool interrupted();
bool wouldblock();
bool connectionlost();

NS_MY_END
#endif
