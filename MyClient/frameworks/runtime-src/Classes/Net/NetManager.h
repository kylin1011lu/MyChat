#ifndef __NET_MANAGER_H__
#define __NET_MANAGER_H__

#include "Definition/Define.h"
#include "Net/TCPClient.h"
#include "cocos2d.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"
#include "Common/Singleton.h"

#define NET_CONNECT_TIMEOUT						10

USING_NS_CC;
NS_MY_BEGIN
class NetManager :public Ref
{
public:
	NetManager() : c(NULL),lstate(NULL), status(eSocketIoClosed),connecttime(0.0f)
	{}
	~NetManager(){ CC_SAFE_DELETE(c); }
	bool									init(lua_State*L);
	bool									connectServer(const char* ip, int port);
	void									disconnectServer();
	void                                    sendmsg(int msgid,const char*encodemsg,int msglen);
	void									poll(float);
	bool									msgParse(MY_MSG_HEAD*msghead);

	void									onConnected();
	void									onConnectFailed();
	void									onConnectTimeout();
	void									onDisconnected();

	bool									isServerConnected();

protected:
	TCPClient*                              c;
	SocketStatus							status;
	float									connecttime;
	lua_State*								lstate;
};
NS_MY_END

#define sNetManager	mychat::Singleton<mychat::NetManager>::Instance()

#endif
