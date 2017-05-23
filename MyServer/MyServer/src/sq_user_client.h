/*
 * \file: sq_user_client.h
 * \brief: Created by hushouguo at Aug 22 2014 10:40:36
 */
 
#ifndef __SQ_USER_CLIENT_H__
#define __SQ_USER_CLIENT_H__
 
struct sq_user_client
{
	sq_client*									c;
	network_address								user_address;
	time_t										retry_time;
	sq_user_client();
	SOCKET										fd();
	void										sendmsg(MY_MSG_HEAD* msg);
	bool										init(const char* ip, int port, const char* domain);
	bool										retry_user_server();
};

#endif
