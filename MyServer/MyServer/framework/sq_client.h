/*
 * \file: sq_client.h
 * \brief: Created by hushouguo at 13:30:47 Mar 14 2012
 */
 
#ifndef __SQ_CLIENT_H__
#define __SQ_CLIENT_H__

#define SQ_CLIENT_CONNECT_TIMEOUT	10

struct sq_client : public sq_socket
{
	sq_client(socket_dispatch f);
	bool							connect(uint32_t ip, int port, uint32_t timeout = SQ_CLIENT_CONNECT_TIMEOUT);
	bool							connect(const char* ip, int port, uint32_t timeout = SQ_CLIENT_CONNECT_TIMEOUT);
};

#endif
