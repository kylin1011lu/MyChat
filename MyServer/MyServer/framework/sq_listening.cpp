/*
 * \file: sq_listening.cpp
 * \brief: Created by hushouguo at 01:45:15 Jan 03 2020
 */

#include "framework.h"

#define TCPWINDOW               512*KB

sq_listening::sq_listening(listening_dispatch f)
	: sq_network_object(socket(AF_INET, SOCK_STREAM, 0)), dispatch(f)
{
}
void sq_listening::receive()
{
	while (!sys_halt)
	{
		struct sockaddr_in clientaddr;
		socklen_t cliaddrlen = sizeof(clientaddr);
		SOCKET connfd = accept(this->fd, (struct sockaddr *)&clientaddr, &cliaddrlen);
		if (connfd < 0 || connfd == -1) 
		{
			if (wouldblock()) { break; }
			if (interrupted())
			{
				continue;
			}
			error_log("accept:%d,%s\n", errno, sq_strerror(errno)); break;
		}

		size_t size = TCPWINDOW;
		if (setsockopt(connfd, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(size)) < 0)
		{
			error_log("setsockopt SO_RCVBUF error:%d,%s\n", errno, sq_strerror(errno));
			::close(connfd); continue;
		}

		if (setsockopt(connfd, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(size)) < 0)
		{
			error_log("setsockopt SO_SNDBUF error:%d,%s\n", errno, sq_strerror(errno));
			::close(connfd); continue;
		}

		this->dispatch(this, connfd);
	}
}
void sq_listening::send()
{
}
void sq_listening::lost_connection()
{
}
void sq_listening::close_connection()
{
}
bool sq_listening::init(uint32_t ip, int port)
{
	int on;
	struct sockaddr_in localaddr;

	on = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) < 0)
	{
		error_log("setsockopt REUSEADDR error:%d,%s\n", errno, sq_strerror(errno));
		return false;
	}

	if (!setblocking(this->fd, false))
	{
		return false;
	}
	
	memset(&localaddr, 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = ip;
	localaddr.sin_port = port;

	if (::bind(this->fd, (struct sockaddr*)&localaddr, sizeof(localaddr)) < 0)
	{
		error_log("bind address %s(%d) error:%d,%s\n", inet_ntoa(*(struct in_addr*)&ip), ntohs(port), errno, sq_strerror(errno));
		return false;
	}

	if (listen(this->fd, SOMAXCONN) < 0)
	{
		error_log("listen on %s(%d) error:%d,%s\n", inet_ntoa(*(struct in_addr*)&ip), ntohs(port), errno, sq_strerror(errno));
		return false;
	}

	debug_log("listening on %s:%d\n", inet_ntoa(*(struct in_addr*)&ip), ntohs(port));

	return true;
}
bool sq_listening::init(const char* ip, int port)
{
	return this->init(inet_addr(ip), htons(port));
}

