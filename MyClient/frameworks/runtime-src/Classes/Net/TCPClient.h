/*
 * file: TCPClient.h
 * brief: Created by kylin 
 */
 
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "cocos2d.h"
#include "Definition/Define.h"
#include "protocol.h"
#include "Network.h"

NS_MY_BEGIN

struct SocketBuffer
{
	char*						data;
	size_t						size;
	size_t						current;
	SocketBuffer();
	SocketBuffer(size_t reserved);
	SocketBuffer(char* d, size_t len);
	~SocketBuffer();
	void						resize(size_t newsize);
	void						clear();
};

enum SocketStatus
{
	eSocketConnected = 1,
	eSocketConnecting = 2,
	eSocketDisconnected = 3,
	eSocketConnectFailed = 4,
	eSocketConnectTimeout = 5,
	eSocketReceive = 6,
	eSocketIoClosed = 0,
	eSocketIoError = -1,
	eSocketCreateFailed = -2,
};

class TCPClient
{
public:
	TCPClient();
	~TCPClient();
	void						setInetAddr(const char* addr, uint16_t port);
	bool						init();
	bool						connect();
	MY_MSG_HEAD*				handleRecvBuffer(SocketBuffer* socketBuffer);
	MY_MSG_HEAD*				receive();

	size_t						tsend(void* data, size_t size);
	void						sendout();
	void						sendMsg(int msgid, const char*encodemsg,int msglen);

	bool						isReadable();
	bool						isWriteable();
	void						sockclose();
	int							isConnected();

protected:
	SOCKET						fd;

	SocketBuffer				recv_buffer;
	SocketBuffer				send_buffer;

	network_address				svraddr;
	bool						connected;
};

NS_MY_END
#endif
