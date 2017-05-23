
#include "framework.h"
#include "server.h"
#include "sq_handler.h"
#include "sq_handler_user.h"

#define DO_MESSAGE_NUMBER			100
#define MIN_POLL_INTERVAL			50

static int pool_size = 0;

sq_handler** handler_pool = NULL;


sq_msg_handler::sq_msg_handler()
{

}

sq_msg_handler::~sq_msg_handler()
{

}

void sq_msg_handler::RegisterMsgHandler(DWORD dwMsgId, _CMsgHandler_* pkHandler)
{
#ifdef _DEBUG
	//map<DWORD, _CMsgHandler_*>::iterator it = m_kHandlerMap.find(dwMsgId);
	//assert(it == m_kHandlerMap.end());
#endif

	m_kHandlerMap.insert(std::make_pair(dwMsgId, pkHandler));
}

void sq_msg_handler::UnregisterMsgHandler(DWORD dwMsgId)
{
	std::map<DWORD, _CMsgHandler_*>::iterator it = m_kHandlerMap.find(dwMsgId);

	if (it != m_kHandlerMap.end())
	{
		delete (*it).second;
		m_kHandlerMap.erase(it);
	}
}

void sq_msg_handler::msg_parser(MY_MSG_HEAD* msg)
{
	std::map<DWORD, _CMsgHandler_*>::iterator it = m_kHandlerMap.find(msg->msgid);

	if (msg->msgid != 100)
		msg = msg;

	if (it != m_kHandlerMap.end())
	{
		//char szBuf[64];

		//sprintf(szBuf, "msg %i.++", msg->cmd);
		//::OutputDebugStringA(szBuf);

		(*it->second)(msg);

		//		::OutputDebugStringA("msg ----\n");
	}
}


void handler_runnable(void* p)
{
	sq_handler* handler = (sq_handler*)p;
	srand(time(NULL) * (handler->m_nIdx + 8));

	while (!sys_halt)
	{
		handler->poll();
	}
}

sq_handler::sq_handler()
{
	q = new sq_msg_queue;
	//	engine = new sq_lua_stack;
	//embed_poll_func = NULL;
	//session_client = NULL;
	user_client = NULL;
	poll_time = time(0);

	for (int i = 0; i < 24; i++)
		bHourCome[i] = false;

	for (int i = 0; i < nullmsg::MaxMsgId; i++)
		m_msgParseTicks[i] = 0;

	m_llMaxProcessTick = 0;
	//	this->pb = NULL;
	//	this->record = NULL;
}
sq_handler::~sq_handler()
{
	SQ_SAFE_DELETE(this->q);
	//	SQ_SAFE_DELETE(this->engine);
	//SQ_SAFE_DELETE(this->session_client);
	SQ_SAFE_DELETE(this->user_client);
	//	if (this->pb) { luapb_destroy(this->pb); }
	//	sq_record_destroy(this->record);
	//for (std::vector<sq_web_service*>::iterator i = web_services.begin(); i != web_services.end(); ++i)
	//{
	//	sq_web_service* web_service = *i;
	//	SQ_SAFE_DELETE(web_service);
	//}
	//web_services.clear();
}
bool sq_handler::init(int n)
{
	m_nIdx = n;

	switch (server->type)
	{
		case SHARD_SERVER_SESSION:
			{
				//if (!(this->record = sq_record_init())) { return false; }
			}
			break;
		case SHARD_SERVER_USER:
			{
				//if (!(this->record = sq_record_init())) { return false; }
				//if (!init_session_client(
				//	"root.user.connect_session.ip",
				//	"root.user.connect_session.domain",
				//	"root.user.connect_session.port")) {
				//	return false;
				//}

				//{
				//	XS_server_login_request kMsg;
				//	kMsg.shard_id = shard_app_id;
				//	kMsg.type = SHARD_SERVER_USER;
				//	kMsg.idx = n;

				//	session_client->sendmsg(&kMsg, 0);
				//}
				//	luaopen_cURL(this->engine->get_lua_state());
			}
			break;
	}

	return true;
}

void sq_handler::poll()
{
	int nLoopCnt = 0;
	std::list<MY_MSG_HEAD*> msg_list;
	std::list<MY_MSG_HEAD*>::iterator it;

	uint64_t llProcessTick = GetTickCount64();

	while (nLoopCnt < 10 && !sys_halt)
	{
		for (int i = 0; i < 24; i++)
		{
			if (bHourCome[i])
			{
				bHourCome[i] = false;

				onHourChanged(i);
			}
		}

		this->q->pop_front(msg_list);

		if (msg_list.empty()) { break; }

		for (it = msg_list.begin(); it != msg_list.end(); it++)
		{
			MY_MSG_HEAD* msg = *it;

			unsigned long long lStartTick = GetTickCount64();

			//debug_log("process msg %i.\n", msg->cmd);
			if (msg->msgid == 0)
			{
				this->connection_close(msg->s);
			}
			else
			{
				this->msg_parser(msg);
			}

			//debug_log("process msg %i ok\n", msg->cmd);

			lStartTick = GetTickCount64() - lStartTick;

			if (m_msgParseTicks[msg->msgid] < lStartTick)
				m_msgParseTicks[msg->msgid] = lStartTick;

			MY_MSG_HEAD::DestroyMessage(msg);
		}

		msg_list.clear();
		nLoopCnt++;
	}

	llProcessTick = GetTickCount64() - llProcessTick;

	if (llProcessTick > m_llMaxProcessTick)
		m_llMaxProcessTick = llProcessTick;
	else
	{
		float fIdleCoef = 1;

		if (m_llMaxProcessTick != 0)
			fIdleCoef = ((float)(m_llMaxProcessTick - llProcessTick)) / m_llMaxProcessTick;

		if (fIdleCoef > 0.6)
		{
			assert(fIdleCoef <= 1);

			sq_sleep(MIN_POLL_INTERVAL * fIdleCoef);

			time_t now = time(0);

			if (m_tt != now)
			{
				m_tt = now;
				//debug_log("idx %i, process time %llu, max process time %llu.\n", m_nIdx, llProcessTick, m_llMaxProcessTick);
			}
		}
	}
}

void sq_handler::onHourChanged(int hour)
{
}
void sq_handler::connection_close(SOCKET s)
{

}

//-----------------------------------------------------------------------------------------------
bool sq_handler_init()
{
	if (server->thread_num <= 0) { return false; }

	pool_size = server->thread_num;
	handler_pool = (sq_handler**)malloc(pool_size*sizeof(sq_handler*));

	for (int n = 0; n < pool_size && !sys_halt; ++n)
	{
		sq_handler* handler = NULL;

		switch (server->type)
		{
		case SHARD_SERVER_SESSION:
			//handler = new sq_handler_session;
			break;
		case SHARD_SERVER_USER:
			handler = new sq_handler_user;
			break;
		default:
			break;
		}

		handler_pool[n] = handler;

		if (!handler->init(n))
		{
			return false;
		}
		sq_thread_pool_insert("handler", handler_runnable, handler);
	}

	return !sys_halt;
}

void sq_handler_dispatch(SOCKET connfd, MY_MSG_HEAD* msg)
{
	if (!msg)
	{
		for (int n = 0; n < pool_size; ++n)
		{
			handler_pool[n]->q->push_back(connfd, NULL);
		}
	}
	else
	{
		if (msg->userid == -1)
		{
			for (int n = 0; n < pool_size; ++n)
			{
				handler_pool[n]->q->push_back(connfd, msg);
			}
		}
		else
		{
			handler_pool[msg->userid%pool_size]->q->push_back(connfd, msg);
		}
	}
}

void CheckNextDay()
{
	static int nHourNow = 0;
	int nHourNext = DaysOfToday(NULL, sq_timezone - 1, 60 * 60) % 24;

	if (nHourNow != nHourNext)
	{
		nHourNow = nHourNext;

		if (nHourNow == 0)
			sq_logger_init(server->log_dir, server->name, server->become_daemon);

		for (int n = 0; n < pool_size; ++n)
			handler_pool[n]->bHourCome[nHourNow] = true;
	}
}

void sq_handler_update()
{
	switch (server->type)
	{
		case SHARD_SERVER_SESSION:
		{
			CheckNextDay();
			for (int n = 0; n <= server->maxfd; ++n)
			{
				sq_socket* c = server->conntable[n];
				if (!c) { continue; }

				if (!c->active())
				{
					if (c->close_flag)
					{
						alarm_log("close connection:%d\n", c->fd);
						server->epoll->remove(c);
					}
					else if (c->lost_flag)
					{
						alarm_log("lost connection:%d\n", c->fd);
					}
					else
					{
						//alarm_log("expire connection:%d, elapse time:%ds > %ds\n", c->fd, nowtime - c->last_recv_time, value);
					}

					server->conntable[n] = NULL;

					server->handler_dispatch(c->fd, NULL);// notify connection close

					server->socket_pool->release(c);
				}
			}
		}
		break;
		case SHARD_SERVER_USER:
		{
			CheckNextDay();
			uint32_t nowtime = time(0);
			uint32_t value = 30000;//conf.get_value("root.user.heartbeat.seconds", 5U) * conf.get_value("root.user.heartbeat.max_miss_num", 5U);
			for (int n = 0; n <= server->maxfd; ++n)
			{
				sq_socket* c = server->conntable[n];
				if (!c) { continue; }

				if (!c->active() || (nowtime - c->last_recv_time) > value)
					//if (!c->active())
				{
					if (c->close_flag)
					{
						alarm_log("close connection:%d\n", c->fd);
						server->epoll->remove(c);
					}
					else if (c->lost_flag)
					{
						alarm_log("lost connection:%d\n", c->fd);
					}
					else
					{
						alarm_log("expire connection:%d, elapse time:%ds > %ds\n", c->fd, nowtime - c->last_recv_time, value);
					}

					server->conntable[n] = NULL;

					server->handler_dispatch(c->fd, NULL);// notify connection close

					server->socket_pool->release(c);
				}
			}
		}
		break;
	}
}