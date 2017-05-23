/*
 * \file: sq_user_client.cpp
 * \brief: Created by hushouguo at Aug 22 2014 10:41:11
 */

#include "framework.h"
#include "server.h"
#include "sq_user_client.h"

#define DEF_RETRY_USER_SERVER_INTERVAL	10

sq_user_client::sq_user_client()
{
	c = NULL;
	retry_time = 0;
}
SOCKET sq_user_client::fd()
{
	return c ? c->fd : 0;
}
void sq_user_client::sendmsg(MY_MSG_HEAD* msg)
{
	if (c)
	{
		c->sendmsg(msg);
	}
	else { error_log("user client unready\n"); }
}
bool sq_user_client::init(const char* ip, int port, const char* domain)
{
	if (!ip || port == 0)
	{
		error_log("invalid ip:%s or port:%d\n", ip, port);
		return false;
	}

	user_address.ip = inet_addr(ip);
	user_address.port = htons(port);

	if (user_address.ip == 0)
	{
		if (!domain)
		{
			error_log("ip and domain MUST config one\n");
			return false;
		}

		ip = domain_lookup(domain);
		if (!ip)
		{
			error_log("user bind domain:%s config error\n", domain);
			return false;
		}

		//alarm_log("use domain:%s to connect user server:%s\n", domain, ip);
		//
		user_address.ip = inet_addr(ip);
	}
	
	if (!retry_user_server())
	{
		error_log("unreachable user:%s:%d\n", ip, port);
		return false;
	}
	
	return true;
}
bool sq_user_client::retry_user_server()
{
	time_t now = time(0);
	if ((now - retry_time) >= DEF_RETRY_USER_SERVER_INTERVAL)
	{
		c = new sq_client(server->socket_callback);
		if (!c->connect(user_address.ip, user_address.port))
		{
			SQ_SAFE_DELETE(c);

			//error_log("unreachable user:%s:%d\n", ip, port);
			retry_time = now;

			return false;
		}
		server->epoll->insert(c);

		debug_log("attach to user server\n");
	}
	return true;
}
