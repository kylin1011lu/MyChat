#include "Common.h"
#include "cocos2d.h"

const char* KEY_USERID = "USERID";


uint32_t getUserid()
{
	return cocos2d::UserDefault::getInstance()->getIntegerForKey(KEY_USERID, 0);
}
void setUserid(uint32_t userid)
{
	cocos2d::UserDefault::getInstance()->setIntegerForKey(KEY_USERID, userid);
}