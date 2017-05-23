
#include "framework.h"

bool setblocking(SOCKET fd, bool block)
{
#ifdef LINUX
	int opts = fcntl(fd, F_GETFL);

	if (opts < 0)
	{
		error_log("fcntl(%d):%d,%s\n", fd, errno, sq_strerror(errno));
		return false;
	}
	if (block)
	{
		opts &= ~(O_NONBLOCK);
	}
	else
	{
		opts |= O_NONBLOCK;
	}
	if (fcntl(fd, F_SETFL, opts) < 0)
	{
		error_log("fcntl(%d):%d,%s\n", fd, errno, sq_strerror(errno));
		return false;
	}
#endif
#ifdef WIN32
	unsigned long b = block ? 0 : 1;
	if (ioctlsocket(fd, FIONBIO, &b) != 0)
	{
		error_log("ioctlsocket error:%d\n", WSAGetLastError());
		return false;
	}
#endif
	return true;
}
bool interrupted()
{
#ifdef WIN32
	return WSAGetLastError() == WSAEINTR;
#else
	return errno == EINTR;
#endif	
}
#if 0
static bool nobuffers()
{
#ifdef WIN32
	int error = WSAGetLastError();
	return error == WSAENOBUFS || error == WSAEFAULT;
#else
	return errno == ENOBUFS;
#endif	
}
#endif
bool wouldblock()
{
#ifdef WIN32
	int error = WSAGetLastError();
	return error == WSAEWOULDBLOCK || error == WSA_IO_PENDING || error == ERROR_IO_PENDING;
#else
	return errno == EAGAIN || errno == EWOULDBLOCK;
#endif	
}
bool connectionlost()
{
#ifdef WIN32
	int error = WSAGetLastError();
	return error == WSAECONNRESET || error == WSAESHUTDOWN || error == WSAENOTCONN || error == WSAECONNABORTED;
#else
	return errno == ECONNRESET || errno == ENOTCONN || errno == ESHUTDOWN || errno == ECONNABORTED || errno == EPIPE;
#endif	
}

