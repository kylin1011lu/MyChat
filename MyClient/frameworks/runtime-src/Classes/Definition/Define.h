/*
* file: Define.h
* brief: Created by kylin
*/

#ifndef __DEFINE_H__
#define __DEFINE_H__

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "cocos2d.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"

#ifdef __cplusplus
#define NS_MY_BEGIN                     namespace mychat {
#define NS_MY_END                       }
#define USING_NS_MY                     using namespace mychat
#define NS_MY                           ::mychat
#else
#define NS_MY_BEGIN 
#define NS_MY_END 
#define USING_NS_MY 
#define NS_MY
#endif 


#define LUA_BEGIN_NAMESPACE(L, NAME)\
	do {\
	lua_getglobal(L, "_G"); \
	lua_pushstring(L, NAME); \
	lua_rawget(L, -2); \
	} while (0)

#define LUA_REGISTER(L, F, ROUTINE)\
	do {\
	lua_pushstring(L, F); \
	lua_pushcfunction(L, ROUTINE); \
	lua_rawset(L, -3); \
	} while (0)

#define LUA_END_NAMESPACE(L)\
	do {\
	lua_pop(L, -2); \
	} while (0)

#define LUA_REGISTER_NAMESPACE(L, NAMESPACE)\
	do {\
	lua_getglobal(L, "_G"); \
	lua_pushstring(L, NAMESPACE); \
	lua_newtable(L); \
	lua_rawset(L, -3); \
	lua_pop(L, 1); \
	} while (0)


static int executeFunction(int args)
{
	auto engine = cocos2d::LuaEngine::getInstance();
	cocos2d::LuaStack* stack = engine->getLuaStack();
	return stack->executeFunction(args);
}
#endif
