#include "NetManager.h"
#include "protocol.h"


NS_MY_BEGIN

bool NetManager::connectServer(const char* ip, int port)
{
	CCLOG("try to connect to server:%s,%d\n", ip, port);
	if (status != eSocketConnected && status != eSocketConnecting)
	{
		c->setInetAddr(ip, port);
		if (c->connect())
		{
			CCLOG("connecting to login server:%s,%d\n", ip, port);
			status = eSocketConnecting;
		}
	}
	return false;
}

bool NetManager::isServerConnected()
{
	return status == eSocketConnected;
}

void NetManager::disconnectServer()
{
	if (status == eSocketConnected)
	{
		c->sockclose();
		status = eSocketDisconnected;
		onDisconnected();
	}
}

bool NetManager::init(lua_State*L)
{
	lstate = L;
	c = new TCPClient();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(NetManager::poll), this, 0.0f, kRepeatForever, 0.0f, false);
	return true;
}

void NetManager::sendmsg(int msgid, const char*encodemsg,int msglen)
{	
	if (isServerConnected())
	{
		c->sendMsg(msgid, encodemsg,msglen);
	}
	else
	{
		CCLOG("############## NETWORK UNAVAILABLE ############");
	}
}

void NetManager::poll(float dt)
{
	//×´Ì¬ÅÐ¶Ï
	if (status == eSocketConnecting)
	{
		switch (c->isConnected())
		{
			case eSocketConnected:
				status = eSocketConnected;
				onConnected();
				break;
			case eSocketConnectFailed:
				c->sockclose();
				status = eSocketCreateFailed;
				onConnectFailed();
				break;
			case eSocketConnecting:
				if (connecttime > NET_CONNECT_TIMEOUT)
				{
					c->sockclose();
					status = eSocketDisconnected;
					onConnectTimeout();
					connecttime = 0.0f;
				}
				else
				{
					connecttime += dt;
				}
				break;
			default:
				break;
		}
	}
	else if (status == eSocketConnected)
	{
		while (c->isReadable())
		{
			MY_MSG_HEAD* msghead = c->receive();
			if (msghead)
			{
				CCLOG("receive msg:%d, id:0x%llx\n", msghead->msgid, msghead->userid);
				msgParse(msghead);
			}
			else
			{
				break;
			}
		}

		if (c->isWriteable())
		{
			c->sendout();
		}

	}

}

bool NetManager::msgParse(MY_MSG_HEAD*msghead)
{
	int msgsize = msghead->size - sizeof(MY_MSG_HEAD);
	char *msg = new char[msgsize];
	memset(msg, 0, msgsize);

	memcpy(msg, (char*)msghead + sizeof(MY_MSG_HEAD), msgsize);

	lua_State* L = lstate;
	lua_getglobal(L, "msgParser");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		CCLOG("NetManager::msgParse error");
		return false;
	}
	lua_pushinteger(L, msghead->msgid);
	lua_pushlstring(L, msg, msgsize);

	executeFunction(2);
	lua_pop(L, 0);

	delete[] msg;
	free(msghead);

	return true;
}

void NetManager::onConnected()
{
	CCLOG("NetManager::onConnected\n");
	lua_State* L = lstate;
	lua_getglobal(L, "onConnected");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return ;
	}
	
	executeFunction(0);
	lua_pop(L, 0);
	return;
}
void NetManager::onConnectFailed()
{
	CCLOG("NetManager::onConnectFailed\n");
}
void NetManager::onConnectTimeout()
{
	CCLOG("NetManager::onConnectTimeout\n");
}

void NetManager::onDisconnected()
{
	CCLOG("NetManager::onDisconnected\n");
}

NS_MY_END
