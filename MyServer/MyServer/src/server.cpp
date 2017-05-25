#include "framework.h"
#include "server.h"
#include "sq_handler.h"

#if defined(WIN32)
#include "getopt.h"
#define BUILD_VERSION __TIME__ 
#endif

#define MAX_FD	65535
#define CHECK_INIT(exp)	do { if (!exp) { return 1; } } while(0)

void g_listening_callback(sq_listening* listening, SOCKET connfd)
{
	sq_socket* c = server->socket_pool->allocate(connfd, server->socket_callback);
	assert(connfd < MAX_FD);
	if (server->conntable[connfd])
	{
		error_log("duplicate connfd:%d\n", connfd);
	}
	server->conntable[connfd] = c;

	server->epoll->insert(c);

	if (connfd > server->maxfd) { server->maxfd = connfd; }
}


void g_socket_callback(sq_socket* c, MY_MSG_HEAD* msghead)
{
	server->handler_dispatch(c->fd, msghead);
}
void g_sendmsg(SOCKET connfd, MY_MSG_HEAD* msg)
{
	if (server->conntable[connfd])
	{
		sq_socket* c = server->conntable[connfd];
		c->sendmsg(msg);
	}
	//else { error_log("not exist connfd:%d\n", connfd); }
}
void g_close(SOCKET connfd)
{
	if (server->conntable[connfd])
	{
		sq_socket* c = server->conntable[connfd];
		c->close_connection();
		//shard->epoll->remove(c);
	}
	else { error_log("not exist connfd:%d\n", connfd); }
}
static server_struct server_table[] =
{
	{
		SHARD_SERVER_USER,
		"user",
		0,
		NULL,
		0,
		"root.user.bind.ip",
		"root.user.bind.port",
		1,
		false,
		".logs",
		NULL,
		NULL,
		NULL,
		NULL,
		g_socket_callback,
		g_sendmsg,
		g_close,
		sq_handler_init,
		sq_handler_dispatch,
		sq_handler_update,
	},
	{
		SHARD_SERVER_NONE,
		NULL,
		0,
		NULL,
		0,
		NULL,
		NULL,
		0,
		false,
		"",
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	}
};
static network_address svraddr;

server_struct* server = NULL;
//int32_t shard_app_id = 0;
//
int bot_userid = 0;
char gsz_config_file[64] = "conf/config.xml";
//char service_export_ip[64] = "127.0.0.1";

void usage()
{
	printf("Usage: server [session|user|pay|oss|simclient|rank] [OPTIONS]\n");
	printf("         OPTIONS:  \n");
	printf("           -d: become daemon\n");
	printf("           -h: bind host address\n");
	printf("           -p: bind port\n");
	printf("           -n: specify thread number\n");
	printf("           -o: specify logger dir\n");
	printf("           -b: specify bot userid\n");
	printf("           -c: specify config file\n");
	printf("           -e: specify export host address\n");
}

bool parse_args(int argc, char* argv[])
{
	if (argc < 2)
	{
		usage();
		return false;
	}

	for (int i = 0; server_table[i].name != NULL; ++i)
	{
		if (strcasecmp(argv[1], server_table[i].name) == 0)
		{
			server = &server_table[i];
			break;
		}
	}

	if (!server)
	{
		printf("Unrecognized operating mode:%s\n", argv[1]);
		return false;
	}

	--argc, argv++;

	int c;
	while ((c = getopt(argc, argv, "dDh:p:n:o:b:c:e:")) != -1)
	{
		switch (c)
		{
		case 'd': case 'D': server->become_daemon = true; break;
		case 'h': server->ip = strdup(optarg); break;
		case 'p': server->port = atoi(optarg); break;
		case 'n': server->thread_num = atoi(optarg); break;
		case 'o': strncpy(server->log_dir, optarg, sizeof(server->log_dir)); break;
		case 'b': bot_userid = atoi(optarg); break;
		//case 'c': strncpy(gsz_config_file, optarg, sizeof(gsz_config_file)); break;
		//case 'e': strncpy(service_export_ip, optarg, sizeof(service_export_ip)); break;
		default: usage(); return false;
		}
	}

	return true;
}
bool parse_shard_conf()
{
	sq_xml_parser* xml_parser = new sq_xml_parser(gsz_config_file);

	xml_parser->make_registry(&conf, "root");

	SQ_SAFE_DELETE(xml_parser);

	//no cmd configure parameter, use configure file.
	if (!server->ip)
	{
		const char* ip = conf.get_value(server->conf_ip_key, (const char*)NULL);
		if (ip)
		{
			server->ip = strdup(ip);
		}
	}

	if (server->port == 0)
	{
		server->port = conf.get_value(server->conf_port_key, 0);
	}

	if (server->ip) { svraddr.ip = inet_addr(server->ip); }
	if (server->port) { svraddr.port = htons(server->port); }

	//shard_app_id = conf.get_value("root.shard.id", svraddr.ip + shard->port);

	//if (!server->conntable) { server->conntable = new __gnu_cxx::hash_map<SOCKET, sq_socket*>; }
	if (!server->conntable)
	{
		server->conntable = new sq_socket*[MAX_FD];
		memset(server->conntable, 0, sizeof(sq_socket*)*MAX_FD);
	}
	if (!server->epoll) { server->epoll = new sq_epoll; }
	if (!server->socket_pool) { server->socket_pool = new sq_object_pool<sq_socket>(); }

	return true;
}
bool init_listening()
{
	if (!server->listening && server->ip && server->port)
	{
		server->listening = new sq_listening(g_listening_callback);
		if (!server->listening->init(svraddr.ip, svraddr.port))
		{
			return false;
		}
		server->epoll->insert(server->listening);
	}
	return true;
}
bool split_logger_file(void* object, void* args)
{
	if (!sq_logger_init(server->log_dir, server->name, server->become_daemon))
	{
		return false;
	}

	debug_log("-------------- New Day! --------------\n");

	return true;
}

//bool bot_main();


int main(int argc, char* argv[])
{
#if defined(WIN32)
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	WSAStartup(wVersionRequested, &wsaData);

	_get_timezone(&sq_timezone);
#else
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);

	sq_timezone = tz.tz_minuteswest * -60;
#endif
	currtime.now();

	close_stderr(); /* avoid debug output double */

#if defined(LINUX)
	CHECK_INIT(sq_check_limits());
#endif

	CHECK_INIT(parse_args(argc, argv));

#if defined(LINUX)
	char* root = getenv("ZEBRA_ROOT");
	if (root)
	{
		chdir(root);
	}
#endif

	CHECK_INIT(parse_shard_conf());

	debug_log("build: %s, version: %s\n", BUILD_VERSION, conf.get_value("root.server.version", "UNKNOWN"));

	conf.print();

#if defined(LINUX)
	if (server->become_daemon) { daemon(1, 0); }
#endif

	if (bot_userid == 0)
	{
		//LoadAllTable();

		CHECK_INIT(sq_logger_init(server->log_dir, server->name, server->become_daemon));

		//crontab.registe("logger", "0 0 0 * * *", split_logger_file, NULL, NULL);

		CHECK_INIT(server->handler_init());

		CHECK_INIT(init_listening());

		while (!sys_halt)
		{
			currtime.now();
			server->epoll->poll();
			server->handler_update();
			//crontab.update();
			sched_yield();
		}

		sq_thread_pool_exit();
	}
	//else
		//bot_main();

	debug_log("System halt!\n");

	return 0;
}

