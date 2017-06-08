#include "cocos2d.h"
#include "MyLuaRegister.h"
#include "Definition/Define.h"
#include "Protobuf/common.pb.h"
#include "Net/NetManager.h"

int cci_sendmsg(lua_State* L)
{
	int args = lua_gettop(L);
	if (args < 3)
	{
		CCLOG("cci_sendmsg parameter lack:%d", args);
		return 0;
	}

	if (!lua_isnumber(L, -args))
	{
		CCLOG("cci_sendmsg parameter error:%s", lua_typename(L, lua_type(L, -args)));
		return 0;
	}

	if (!lua_isstring(L, -(args - 1)))
	{
		CCLOG("cci_sendmsg parameter error:%s", lua_typename(L, lua_type(L, -(args - 1))));
		return 0;
	}

	if (!lua_isnumber(L, -(args-2)))
	{
		CCLOG("cci_sendmsg parameter error:%s", lua_typename(L, lua_type(L, -args)));
		return 0;
	}

	int cmd = lua_tointeger(L, -args);
	const char* content = lua_tostring(L, -(args - 1));
	int msglen = lua_tointeger(L, -(args - 2));

	sNetManager.sendmsg(cmd, content,msglen);

	return 0;
}

/* bool connectServer(ip, port) */
static int cci_connectServer(lua_State* L)
{
	int args = lua_gettop(L);
	if (args < 2)
	{
		CCLOG("cci_connectServer parameter lack:%d", args);
		return 0;
	}

	if (!lua_isstring(L, -args))
	{
		CCLOG("cci_connectServer parameter error:%s", lua_typename(L, lua_type(L, -args)));
		return 0;
	}

	if (!lua_isnumber(L, -(args - 1)))
	{
		CCLOG("cci_connectServer parameter error:%s", lua_typename(L, lua_type(L, -(args - 1))));
		return 0;
	}

	const char* ip = lua_tostring(L, -args);
	int port = lua_tointeger(L, -(args - 1));
	bool retcode = sNetManager.connectServer(ip, port);

	lua_pushboolean(L, retcode ? 1 : 0);

	return 1;
}

int my_lua_module_register(lua_State* L)
{
	//зЂВс
	LUA_BEGIN_NAMESPACE(L, "cci");
	//LUA_REGISTER(L, "domainLookup", cci_domainLookup);
	LUA_REGISTER(L, "connectServer", cci_connectServer);
	//LUA_REGISTER(L, "disconnectServer", cci_disconnectServer);
	LUA_REGISTER(L, "sendmsg", cci_sendmsg);
	//LUA_REGISTER(L, "jsonEncode", cci_jsonEncode);
	//LUA_REGISTER(L, "jsonDecode", cci_jsonDecode);
	//LUA_REGISTER(L, "createUUID", cci_createUUID);
	//LUA_REGISTER(L, "getUserid", cci_getUserid);
	//LUA_REGISTER(L, "setUserid", cci_setUserid);
	//LUA_REGISTER(L, "getAccount", cci_getAccount);
	//LUA_REGISTER(L, "setAccount", cci_setAccount);
	//LUA_REGISTER(L, "hashString", cci_hashString);
	//LUA_REGISTER(L, "hashUserid", cci_hashUserid);
	//LUA_REGISTER(L, "now", cci_now);
	//LUA_REGISTER(L, "setServerTime", cci_setServerTime);
	LUA_END_NAMESPACE(L);

	return 0;
}