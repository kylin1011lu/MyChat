/*
 * \file: protocol.h
 * \brief: Created by hushouguo at Apr 16 2013 14:33:33
 */
 
#include "framework.h"

_Msg_Creator_Entry_ gkMsgCreatorEntries[nullmsg::MaxMsgId];

///////////////////////////////////////////////////////////////////////////////////////////

_MSG_CLASS_INIT_::_MSG_CLASS_INIT_(WORD wMsgId, LPCSTR lpszClassName, nullmsg::MessageCreator* pkMsgCreator)
{ 
	if (wMsgId >= 0 && wMsgId < nullmsg::MaxMsgId && pkMsgCreator != NULL)
	{
		gkMsgCreatorEntries[wMsgId].lpszMsgClassName = lpszClassName;
		gkMsgCreatorEntries[wMsgId].pkCreator = pkMsgCreator;
	}
} 

const _Msg_Creator_Entry_* _GetMsgEntry_(WORD wMsgId)
{
	if (wMsgId >= 0 && wMsgId < nullmsg::MaxMsgId)
		return &gkMsgCreatorEntries[wMsgId];
	else
		return NULL;
}

nullmsg *nullmsg::CreateMessage(WORD wMsgId)
{
	nullmsg* pkMsg = NULL;

	if (pkMsg == NULL && wMsgId >= 0 && wMsgId < nullmsg::MaxMsgId && gkMsgCreatorEntries[wMsgId].pkCreator)
		pkMsg = (gkMsgCreatorEntries[wMsgId].pkCreator)();

	//assert(pkMsg);

	return pkMsg;
}

void nullmsg::DestroyMessage(nullmsg *pkMsg)
{
	delete pkMsg;
}

void MY_MSG_HEAD::DestroyMessage(message_head *msghead)
{
	delete msghead->msg;
	msghead->msg == NULL;
	delete msghead;
	msghead = NULL;
}
