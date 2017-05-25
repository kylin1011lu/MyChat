#ifndef __SERVER_H__
#define __SERVER_H__

enum
{
	SHARD_SERVER_NONE = 0,
	SHARD_SERVER_SESSION = 1,
	SHARD_SERVER_PAY = 2,
	SHARD_SERVER_USER = 3,
	SHARD_SERVER_OSS = 4,
	SHARD_SERVER_SIMCLIENT = 5,
	SHARD_SERVER_RANK = 6,
};

struct server_struct
{
	int											type;
	const char* 								name;
	int											maxfd;
	const char*									ip;
	int											port;
	const char*									conf_ip_key;
	const char*									conf_port_key;
	int											thread_num;
	bool										become_daemon;
	char										log_dir[PATH_MAX];
	sq_listening*								listening;
	sq_epoll*									epoll;
	sq_object_pool<sq_socket>*					socket_pool;
	sq_socket**									conntable;
	sq_socket::socket_dispatch					socket_callback;
	void										(*sendmsg)(SOCKET, MY_MSG_HEAD*);
	void										(*close)(SOCKET);
	bool										(*handler_init)();
	void										(*handler_dispatch)(SOCKET, MY_MSG_HEAD*);
	void										(*handler_update)();
};

extern server_struct* server;
#endif // !_SERVER_H_
