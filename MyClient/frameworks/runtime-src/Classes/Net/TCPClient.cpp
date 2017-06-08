#include "TCPClient.h"
#include "Network.h"
#include "NetManager.h"
#include "Protobuf/common.pb.h"
#include "Common/Common.h"

NS_MY_BEGIN

SocketBuffer::SocketBuffer()
{
	this->data = NULL;
	this->size = 0;
	this->current = 0;
}
SocketBuffer::SocketBuffer(size_t reserved)
{
	this->data = (char*)malloc(reserved);
	this->size = reserved;
	this->current = 0;
}
SocketBuffer::SocketBuffer(char* d, size_t len)
{
	this->data = (char*)malloc(len);
	memcpy(this->data, d, len);
	this->size = len;
	this->current = len;
}
SocketBuffer::~SocketBuffer()
{
	clear();
}
void SocketBuffer::resize(size_t newsize)
{
	if (this->size < newsize)
	{
		//按4字节对齐
		newsize += 8;
		newsize &= ~0x07; //~按位取反

		void* p = realloc(this->data, newsize);
		CC_ASSERT(p != NULL);
		this->data = (char*)p;
		this->size = newsize;
	}
}
void SocketBuffer::clear()
{
	if (this->data)
	{
		free(this->data);
	}
	this->data = NULL;
	this->size = 0;
	this->current = 0;
}

//-----------------------------------------
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
static bool startup = false;
#endif
TCPClient::TCPClient()
:fd(INVALID_SOCKET)
,connected(false)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	if (!startup)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		startup = true;
	}
#endif
}

TCPClient::~TCPClient()
{
	sockclose();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	WSACleanup();
#endif
}

void TCPClient::sockclose()
{
	close(fd);
	fd = INVALID_SOCKET;
}

void TCPClient::setInetAddr(const char* addr, uint16_t port)
{
	strcpy(this->svraddr.ip,addr);
	this->svraddr.port = port;
}
bool TCPClient::init()
{
	if (fd != INVALID_SOCKET)
	{
		sockclose();
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == INVALID_SOCKET)
	{
		CCLOG("create socket:%d,%s error\n", errno, strerror(errno));
		return false;
	}
	//设置成非阻塞
	setblocking(fd, false);
    return true;
}

bool TCPClient::connect()
{
	if (fd == INVALID_SOCKET)
	{
		if (!init())
		{
			return false;
		}
	}

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(svraddr.port);
	inet_pton(AF_INET, svraddr.ip, &server.sin_addr.s_addr);
		
	int ret = ::connect(this->fd, (struct sockaddr *)&server, sizeof(server));

	if (ret == 0)
	{
		return true;
	}
	else
	{
#if( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
		int nError = WSAGetLastError();
		if (nError == WSAEWOULDBLOCK)
		{
			return true;
		}
		else
		{
			return false;
		}
#endif

#if( CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
		int nError = errno;
		if (ret == SOCKET_ERROR && nError == EINPROGRESS)
		{
			return true;
		}
		else
		{
			return false;
		}
#endif
	}
}

int	TCPClient::isConnected()
{
	if (this->fd < 0)
	{
		return -1;
	}

	fd_set	fdwrite;
	struct timeval tv;

	FD_ZERO(&fdwrite);
	FD_SET(this->fd, &fdwrite);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if (select((int)(this->fd + 1), NULL, &fdwrite, NULL, &tv) > 0)
	{
		if (FD_ISSET(this->fd, &fdwrite))
		{
#if( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )

			return eSocketConnected;

#elif( CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )

			int nError;
			socklen_t len = sizeof(nError);
			if (getsockopt(this->fd, SOL_SOCKET, SO_ERROR, &nError, &len) < 0)
			{
				return eSocketConnectFailed;
			}
			if (nError == ECONNREFUSED || nError == ETIMEDOUT)
			{
				return eSocketConnectFailed;
			}
			return eSocketConnected;

#endif
		}
	}
	return eSocketConnecting;
}

bool TCPClient::isReadable()
{
	if (fd == INVALID_SOCKET)
	{
		return false;
	}

	fd_set	fdread;
	struct timeval tv;

	FD_ZERO(&fdread);
	FD_SET(fd, &fdread);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if (select((int)(fd + 1), &fdread, NULL, NULL, &tv) > 0)
	{
		if (FD_ISSET(fd, &fdread))
		{
			return true;
		}
	}

	return false;
}

bool TCPClient::isWriteable()
{
	if (fd == INVALID_SOCKET)
	{
		return false;
	}

	fd_set	fdwrite;
	struct timeval tv;

	FD_ZERO(&fdwrite);
	FD_SET(fd, &fdwrite);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if (select((int)(fd + 1), NULL, &fdwrite, NULL, &tv) > 0)
	{
		if (FD_ISSET(fd, &fdwrite))
		{
			return true;
		}
	}

	return false;
}

MY_MSG_HEAD* TCPClient::handleRecvBuffer(SocketBuffer* socketBuffer)
{
	//读取缓存里面不足消息头的大小，不具备解析条件
	if (socketBuffer->current < sizeof(MY_MSG_HEAD))
	{
		return NULL;
	}

	uint32_t msgSize = *(uint32_t*)&socketBuffer->data[MY_MSG_HEAD_SIZE_INDEX];
	if (socketBuffer->size < msgSize)
	{
		if (msgSize > MAX_NETWORK_PACKAGE_SIZE)
		{
			CCLOG("exceed package size limit:%u\n", msgSize);
			return NULL;
		}

		socketBuffer->resize(msgSize);
		return NULL;
	}

	if (socketBuffer->current < msgSize)
	{
		return NULL;
	}

	MY_MSG_HEAD* msghead = (MY_MSG_HEAD*)malloc(msgSize);
	memcpy(msghead, &socketBuffer->data[0], msgSize);
	
	if (socketBuffer->current > msgSize)
	{
		//碎包
		memmove(&socketBuffer->data[0], &socketBuffer->data[msgSize], socketBuffer->current - msgSize);
	}
	socketBuffer->current -= msgSize;

	return msghead;
}

MY_MSG_HEAD*	TCPClient::receive()
{
	MY_MSG_HEAD* msghead = handleRecvBuffer(&recv_buffer);
	if (msghead != NULL)
	{
		return msghead;
	}

	while (true)
	{
		int leftsize = recv_buffer.size - recv_buffer.current;
		if (leftsize == 0)
		{
			leftsize = MAX_NETWORK_PACKAGE_SIZE;
			recv_buffer.resize(MAX_NETWORK_PACKAGE_SIZE);
		}

		size_t size = TEMP_FAILURE_RETRY(::recv(fd, &recv_buffer.data[recv_buffer.current], leftsize, 0));
		if (size == 0)
		{
			CCLOG("Connection lost:%d,%s\n", errno, strerror(errno));
			sNetManager.disconnectServer();
			return NULL;
		}
		else if (size == SOCKET_ERROR)
		{
			if (interrupted()) 
			{ 
				continue; 
			}
			if (wouldblock())
			{
				/* on ET mode, MUST recv EAGAIN return */
				return NULL;
			}
			CCLOG("Connection error:%d,%s\n", errno, strerror(errno));
			sNetManager.disconnectServer();
			return NULL;
		}

		recv_buffer.current += size;
		msghead = handleRecvBuffer(&recv_buffer);
		if (msghead)
		{
			return msghead;
		}
	}
	return NULL;
}
void TCPClient::sendMsg(int msgid, const char*encodemsg,int msglen)
{
	char buf[MAX_NETWORK_PACKAGE_SIZE];
	memset(buf, 0, sizeof(buf));

	int headlen = sizeof(MY_MSG_HEAD);

	MY_MSG_HEAD msghead;
	msghead.size = msglen + headlen;
	msghead.msgid = msgid;
	msghead.userid = getUserid();

	memcpy(buf, &msghead, headlen);
	memcpy(buf+headlen,encodemsg,msglen);

	send_buffer.resize(send_buffer.current + msghead.size);
	memcpy(&send_buffer.data[send_buffer.current], buf, msghead.size);
	send_buffer.current += msghead.size;
	
}
void TCPClient::sendout()
{
	while (this->send_buffer.current > 0)
	{
		ssize_t n = tsend(&send_buffer.data[0], send_buffer.current);
		if (n == SOCKET_ERROR)
		{
			CCLOG("sendout error:%d,%s\n", errno, strerror(errno));
			sNetManager.disconnectServer();
			break;
		}

		size_t size = (size_t)n;
		if (size == 0)
		{
			/* would block */
			break;
		}

		if (send_buffer.current > size)
		{
			memmove(&send_buffer.data[0], &send_buffer.data[size], send_buffer.current - size);
		}

		send_buffer.current -= size;
	}
}

size_t TCPClient::tsend(void* data, size_t size)
{
	size_t sendsize = 0;
	while (size > sendsize)
	{
#ifdef WIN32
		ssize_t n = TEMP_FAILURE_RETRY(::send(fd, (const char*)data + sendsize, size - sendsize, 0));
#else
		ssize_t n = TEMP_FAILURE_RETRY(::send(fd, (uint8_t*)data + sendsize, size - sendsize, 0));
#endif

		if (n == 0) { return SOCKET_ERROR; }
		if (n == SOCKET_ERROR)
		{
			if (interrupted()) { continue; }
			if (wouldblock())
			{
				/* on ET mode, MUST send EAGAIN return */
				return sendsize;
			}
			return SOCKET_ERROR;
		}

		sendsize += n;
	}
	return sendsize;
}
NS_MY_END
