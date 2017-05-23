/*
 * \file: sq_client.cpp
 * \brief: Created by hushouguo at 13:30:50 Mar 14 2012
 */

#include "framework.h"

sq_client::sq_client(socket_dispatch f)
	: sq_socket(socket(AF_INET, SOCK_STREAM, 0), f)
{
}
bool sq_client::connect(uint32_t ip, int port, uint32_t timeout)
{
	struct sockaddr_in serveraddr;

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = ip;
	serveraddr.sin_port = port;

	setblocking(this->fd, false);
	if (::connect(this->fd, (sockaddr*)&serveraddr, sizeof(struct sockaddr_in)) < 0)
	{
		fd_set fdread;
		fd_set fdwrite;
		timeval tv;
	
		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_SET(this->fd, &fdread);
		FD_SET(this->fd, &fdwrite);
	
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
	
		int retval = select(this->fd+1, &fdread, &fdwrite, NULL, &tv);
		if (retval < 0)
		{
			error_log("select operate failure:%d,%s\n", errno, strerror(errno));
			closesocket(this->fd);
			return false;
		}
		else if (retval == 0) 
		{
			error_log("connect timeout, unreachable server %s:%d\n", inet_ntoa(*(struct in_addr*)&ip), ntohs(port));
			closesocket(this->fd);
			return false;
		}
		else
		{
			if (FD_ISSET(this->fd, &fdwrite))
			{
				if (FD_ISSET(this->fd, &fdread))
				{
					//TODO: NOT SURE, as a failure case
					error_log("connect exception, unreachable server %s:%d\n", inet_ntoa(*(struct in_addr*)&ip), ntohs(port));
					closesocket(this->fd);
					return false;
				}
				else
				{
					//debug_log("connect success\n");
				}
			}
			else
			{
				error_log("connect failure, unreachable server %s:%d\n", inet_ntoa(*(struct in_addr*)&ip), ntohs(port));
				closesocket(this->fd);
				return false;
			}
		}
	}
	return true;
}
bool sq_client::connect(const char* ip, int port, uint32_t timeout)
{
	return this->connect(inet_addr(ip), htons(port), timeout);
}
