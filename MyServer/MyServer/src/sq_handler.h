/*
 * \file: sq_handler.h
 * \brief: Created by hushouguo at Apr 18 2013 15:31:17
 */
 
#ifndef __SQ_HANDLER_H__
#define __SQ_HANDLER_H__

#include "sq_user_client.h"
#include "sq_record.h"


#define VILLAGE_FIELDS_SIZE 36

class _CMsgHandler_
{
public:
	virtual ~_CMsgHandler_(void) {}
	virtual void operator()(const MY_MSG_HEAD* pkMsg) = 0;
};

template<typename T>
class CObjMemberMsgHandler : public _CMsgHandler_
{
public:
	typedef void (T::*ObjMemFunPtr)(const MY_MSG_HEAD* pkMsg);

	~CObjMemberMsgHandler() {}

	CObjMemberMsgHandler(T* pObj, ObjMemFunPtr pObjMemberFunc)
		: m_pkObj(pObj), m_pkObjMemberFunc(pObjMemberFunc)
	{
	}

	virtual void operator()(const MY_MSG_HEAD* pkMsg)
	{
		assert(m_pkObj && m_pkObjMemberFunc);

		(m_pkObj->*m_pkObjMemberFunc)(pkMsg);
	}

public:
	T*				m_pkObj;
	ObjMemFunPtr	m_pkObjMemberFunc;
};

#define REGISTER_MSG_HANDLER(FuncClass, Func, MessageID)					\
	CObjMemberMsgHandler<FuncClass>* p##FuncClass##Func##Handle = \
	new CObjMemberMsgHandler<FuncClass>(this, &FuncClass::Func);			\
	RegisterMsgHandler(MessageID, \
	p##FuncClass##Func##Handle);

#define UNREGISTER_MSG(MessageID)											\
	UnregisterMsgHandler(MessageID);

struct sq_msg_handler
{
	sq_msg_handler();
	virtual ~sq_msg_handler();

	std::map<DWORD, _CMsgHandler_*> m_kHandlerMap;

	void RegisterMsgHandler(DWORD dwMsgId, _CMsgHandler_* pkHandler);
	void UnregisterMsgHandler(DWORD dwMsgId);
	void msg_parser(MY_MSG_HEAD* msg);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sq_task
{
	sq_task(float fIdleThreshold = 0.1, int nIdleCoef_UserNum = 500);
	virtual ~sq_task() { };

	enum task_process
	{
		tp_next,
		tp_skip,
		tp_break,
	};

	virtual bool timetick(float idle_coef) = 0;
	virtual void pre_task();
	virtual void post_task();

	float m_fIdleThreshold;
	int m_nIdleCoef_UserNum;

	int m_nProcessedCnt;

	//sq_handler* m_handler;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sq_handler : public sq_msg_handler
{
public:
	sq_msg_queue*								q;
	//sq_lua_stack*								engine;
	//sq_session_client*							session_client;
	sq_user_client*								user_client;
	time_t										poll_time;
	bool										bHourCome[24];
	//luapb*										pb;
	//sq_record*									record;
	//std::vector<sq_web_service*>				web_services;
	sq_handler();
	virtual ~sq_handler();
	virtual bool								init(int n);
	//bool										init_session_client(const char* conf_ip, const char* conf_domain, const char* conf_port);
	bool										init_user_client(const char* conf_ip, const char* conf_domain, const char* conf_port);
	//void										sendmsg(SOCKET connfd, nullmsg* msg);
	virtual void								poll();
	virtual void								onHourChanged(int hour);
	virtual void								connection_close(SOCKET s);
	//sq_web_service*								get_web_service(uint32_t reqid);

	//void										set_embed_poll_fun(_CEmbedPollFunc_* func);

protected:
	//_CEmbedPollFunc_							*embed_poll_func;
	char										m_szTemp[2048];
	unsigned long								m_msgParseTicks[nullmsg::MaxMsgId];

	int m_nIdx;
	uint64_t m_llMaxProcessTick;

	time_t m_tt;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	//virtual bool timetick(float idle_coef);
	//virtual void AddTask(sq_task* task, bool need_lock = true);
	//virtual bool ProcessTask(float idle_coef);

	std::list<sq_task*> m_tasks;

	friend void handler_runnable(void* p);
};


bool sq_handler_init();
void sq_handler_dispatch(SOCKET connfd, MY_MSG_HEAD* msg);
void sq_handler_update();

extern sq_handler** handler_pool;

#endif
