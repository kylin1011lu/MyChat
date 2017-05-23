/*
 * \file: sq_socket.h
 * \brief: Created by hushouguo at 15:15:14 Jul 18 2012
 */
 
#ifndef __SQ_SOCKET_H__
#define __SQ_SOCKET_H__

#define SQ_SEND_BUFFER_SIZE_MAX 	1*MB
#define PROTOCOL_LENGTH_BYTES		4

struct sq_byte_buffer
{
	char*							data;
	size_t							size;
	size_t							current;
	sq_byte_buffer();
	sq_byte_buffer(size_t reserved);
	sq_byte_buffer(char* d, size_t len);
	~sq_byte_buffer();
	void							resize(size_t newsize = 0);
	void							clear();
	size_t							remain();
};

struct sq_socket : public sq_network_object
{
	typedef void (*socket_dispatch)(sq_socket*, MY_MSG_HEAD*);

	sq_atomic						send_locker;
	uint32_t						last_recv_time;
	socket_dispatch					dispatch;
	sq_byte_buffer					recv_buffer;
	sq_byte_buffer					send_buffer;

	sq_socket(SOCKET fd, socket_dispatch f);
	~sq_socket();
	void							receive();
	void							send();
	void							lost_connection();
	void							close_connection();
	void							sendmsg(MY_MSG_HEAD* msg);
};

void save_data(unsigned char* data, int len, const char* filename = NULL);

#endif
