
#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

#define MAX_DB_NUMBER  2  //��ǰ���db����

// �û�״̬����
struct USER_STATE
{
	enum
	{
		FORBIDDEN	= 1,
		ONLINE		= 2,
		OFFLINE		= 3,
		READY		= 4,
		UNKNOWN		= 9,
	};
};


#endif