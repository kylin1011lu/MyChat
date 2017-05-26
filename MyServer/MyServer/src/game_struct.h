
#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

#define MAX_DB_NUMBER  2  //当前最大db数量

// 用户状态编码
struct USER_STATE
{
	enum
	{
		FORBIDDEN = 1,
		ONLINE = 2,
		OFFLINE = 3,
		UNKNOWN = 9,
	};
};


#endif