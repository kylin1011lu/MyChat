/*
 * \file: sq_socket.cpp
 * \brief: Created by hushouguo at 15:17:29 Jul 18 2012
 */

#include "framework.h"

sq_byte_buffer::sq_byte_buffer()
{
	this->data = NULL;
	this->size = 0;
	this->current = 0;

}
sq_byte_buffer::sq_byte_buffer(size_t reserved)
{
	this->data = (char*)malloc(reserved);
	this->size = reserved;
	this->current = 0;
}
sq_byte_buffer::sq_byte_buffer(char* d, size_t len)
{
	this->data = (char*)malloc(len);
	memcpy(this->data, d, len);
	this->size = len;
	this->current = len;
}
sq_byte_buffer::~sq_byte_buffer()
{
	clear();
}
void sq_byte_buffer::resize(size_t newsize)
{
	if (newsize == 0) { newsize = this->size > 0 ? this->size*2 : 32; }
	if (this->size < newsize)
	{
		this->data = (char*)realloc(this->data, newsize);
		this->size = newsize;
	}
}
void sq_byte_buffer::clear()
{
	this->size = 0;
	this->current = 0;
	SQ_SAFE_FREE(this->data);
}
size_t sq_byte_buffer::remain()
{
	return this->size - this->current;
}

long volatile gnSktCnt = 0;

//------------------------------------------------------------------------------------
sq_socket::sq_socket(SOCKET fd, socket_dispatch f)
	: sq_network_object(fd), send_locker(0), last_recv_time(time(0)), dispatch(f)
{
	setblocking(fd, false);
	atomic_fetch_add(&gnSktCnt, 1);

	printf("++skt cnt %i..%i..\n", gnSktCnt, fd);
}
sq_socket::~sq_socket()
{ 
	closesocket(fd); 
	atomic_fetch_add(&gnSktCnt, -1);

	printf("--------------------------------------------------------------skt cnt %i..\n", gnSktCnt);
}

//void sss(nullmsg* pkMsg, MPacket& pkt, int cmd, int len)
//{
//	pkMsg->Serialize(pkt, false);
//}

bool handle_buffer(sq_socket* c)
{
	if (c->recv_buffer.current < PROTOCOL_LENGTH_BYTES) { return false; }

	int a = MY_MSG_HEAD_SIZE_INDEX;
	int b = MY_MSG_HEAD_SIZE;

	uint32_t msg_size = *((uint32_t*)(&c->recv_buffer.data[MY_MSG_HEAD_SIZE_INDEX]));
	if (c->recv_buffer.size < msg_size)
	{
		if (msg_size > MAX_NETWORK_PACKAGE_SIZE)
		{
			error_log("exceed package size limit:%u\n", msg_size);
		}
		else { c->recv_buffer.resize(); }
		return false;
	}

	if (c->recv_buffer.current < msg_size) { return false; } 

	char* pkBuf = c->recv_buffer.data;

	MY_MSG_HEAD*msghead = new MY_MSG_HEAD();
	memcpy(msghead, pkBuf, sizeof(MY_MSG_HEAD));

	mymsg* msg = NULL;
	switch (msghead->msgid)
	{
	case message::AccountRegisterRequest::ID:
		msg = new message::AccountRegisterRequest();
		msg->ParseFromArray(&pkBuf[MY_MSG_HEAD_SIZE], msg_size - MY_MSG_HEAD_SIZE);
		break;
	case message::UserLoginRequest::ID:
		msg = new message::UserLoginRequest();
		msg->ParseFromArray(&pkBuf[MY_MSG_HEAD_SIZE], msg_size - MY_MSG_HEAD_SIZE);
		break;
	case message::ChatRequest::ID:
		msg = new message::ChatRequest();
		msg->ParseFromArray(&pkBuf[MY_MSG_HEAD_SIZE], msg_size - MY_MSG_HEAD_SIZE);
		break;
	case message::ChatHistoryRequest::ID:
		msg = new message::ChatHistoryRequest();
		msg->ParseFromArray(&pkBuf[MY_MSG_HEAD_SIZE], msg_size - MY_MSG_HEAD_SIZE);
		break;
	default:
		error_log("%d,msg is found\n", msghead->msgid);
		break;
	}

	msghead->msg = msg;
	if (msghead)
	{
		c->dispatch(c, msghead);
	}
	else
	{
		error_log("msg is NULL\n");
	}

	if (c->recv_buffer.current > msg_size)
	{
		memmove(&c->recv_buffer.data[0], &c->recv_buffer.data[msg_size], c->recv_buffer.current - msg_size);
	}
	c->recv_buffer.current -= msg_size;

	return true;
}
void sq_socket::receive()
{
	while(1)
	{
		size_t size = this->recv_buffer.remain();
		if (size == 0) 
		{ 
			this->recv_buffer.resize();
			size = this->recv_buffer.remain();
		}

		ssize_t n = TEMP_FAILURE_RETRY(recv(this->fd, &this->recv_buffer.data[this->recv_buffer.current], size, MSG_DONTWAIT));
		
		if (n == 0) 
		{ 
			this->lost_connection(); 
			break; 
		} // The return value will be 0 when the peer has performed an orderly shutdown.
		if (n == SOCKET_ERROR)
		{
			if (interrupted()) { continue; }
			if (wouldblock())
			{
				/* on ET mode, MUST recv EAGAIN return */
				return;
			}
			this->lost_connection(); 
			break;
		}
		this->recv_buffer.current += n;
		this->last_recv_time = time(0);

		while(handle_buffer(this));
	}
}
void sq_socket::send()
{
	while(this->send_buffer.current > 0)
	{
		ssize_t n = TEMP_FAILURE_RETRY(::send(this->fd, &this->send_buffer.data[0], this->send_buffer.current, 0));
		if (n == 0) { this->lost_connection(); break; }
		if (n == SOCKET_ERROR)
		{
			if (interrupted()) { continue; }
			if (wouldblock())
			{
				/* on ET mode, MUST send EAGAIN return */
				return;
			}
			this->lost_connection(); break;
		}

		if (n > 0 && this->send_buffer.current > (size_t)n)
		{
			memmove(&this->send_buffer.data[0], &this->send_buffer.data[n], this->send_buffer.current - n);
		}
		this->send_buffer.current -= n;
	}
}

#include "server.h"

void sq_socket::lost_connection()
{
	if (!lost_flag)
	{
		alarm_log("lost_connection:%d...\n", fd);

		//shard->epoll->remove(this);

		this->lost_flag = true;
	}
}
void sq_socket::close_connection()
{
	alarm_log("close_connection:%d...\n", fd);

	this->close_flag = true;
}

char szFileName[32];
int idx = 0;

void save_data(unsigned char* data, int len, const char* filename)
{
	if (filename != NULL)
		strcpy(szFileName, filename);
	else
		sprintf(szFileName, "tmp/save %i.dat", idx++);

	FILE* fd = fopen(szFileName, "wb+");
	fwrite(data, len, 1, fd);
	fclose(fd);
}

void sq_socket::sendmsg(MY_MSG_HEAD* msg)
{
	char buf[MAX_NETWORK_PACKAGE_SIZE];
	memset(buf, 0, sizeof(buf));

	int msglen = msg->msg->ByteSize();
	char *data = new char[msglen];
	msg->msg->SerializeToArray(data, msglen);

	int headlen = MY_MSG_HEAD_SIZE;
	msg->size = msglen + headlen;

	memcpy(buf, msg, headlen);
	memcpy(buf + headlen, data, msglen);

	assert(msg->size < MAX_NETWORK_PACKAGE_SIZE);
	if (!this->active()) { return; }	
	//printf("..............send %i.\n", msg->cmd);

	char* lpszPayloadBuf = &buf[0];
	uint32_t nPayloadLen = msg->size;

	assert(nPayloadLen + 20 < MAX_NETWORK_PACKAGE_SIZE);

	unsigned char* send_buffer = NULL;
	size_t send_size = 0;
	if (this->send_buffer.current > 0)
	{
		this->send_buffer.resize(this->send_buffer.current + nPayloadLen);
		memcpy(&this->send_buffer.data[this->send_buffer.current], lpszPayloadBuf, nPayloadLen);
		this->send_buffer.current += nPayloadLen;

		send_buffer = (unsigned char*)&this->send_buffer.data[0];
		send_size = this->send_buffer.current;
	}
	else
	{
		send_buffer = (unsigned char*)lpszPayloadBuf;
		send_size = nPayloadLen;
	}

	//error_log("send data len:%02x %02x %02x %02x %02x %02x %02x %02x\n", 
	//	send_buffer[send_size - 1], send_buffer[send_size - 2],
	//	send_buffer[send_size - 3], send_buffer[send_size - 4],
	//	send_buffer[send_size - 5], send_buffer[send_size - 6],
	//	send_buffer[send_size - 7], send_buffer[send_size - 8]);

	//save_data(send_buffer, send_size);

	size_t size = 0;
	while(send_size > size)
	{
		ssize_t n = TEMP_FAILURE_RETRY(::send(this->fd, (const char*)send_buffer + size, send_size - size, 0));

		if (n == 0) { this->lost_connection(); return; }
		if (n == SOCKET_ERROR)
		{
			if (interrupted()) { continue; }
			if (wouldblock())
			{
				/* on ET mode, MUST send EAGAIN return */
				break;
			}
			this->lost_connection(); return;
		}
		size += n;
	}

	if (this->send_buffer.current > 0)
	{
		if (size > 0 && this->send_buffer.current > size)
		{
			memmove(&this->send_buffer.data[0], &this->send_buffer.data[size], this->send_buffer.current - size);
		}
		this->send_buffer.current -= size;
	}
	else
	{
		if (nPayloadLen > size)
		{
			size_t remain_size = nPayloadLen - size;
			this->send_buffer.resize(this->send_buffer.current + remain_size);
			memcpy(&this->send_buffer.data[this->send_buffer.current], (BYTE*)lpszPayloadBuf + size, remain_size);
			this->send_buffer.current += remain_size;
		}
	}

	if (this->send_buffer.current >= SQ_SEND_BUFFER_SIZE_MAX) { this->lost_connection(); }

}

