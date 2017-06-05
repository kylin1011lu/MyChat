/*
 * \file: sq_handler.h
 * \brief: Created by hushouguo at Apr 18 2013 15:31:17
 */
 
#ifndef __SQ_HANDLER_USER_H__
#define __SQ_HANDLER_USER_H__

#include "sq_handler.h"

struct game_user_table
{
	game_user_table();
	~game_user_table();

	SOCKET connfd;
	int64_t userid;
	int user_state;
};

struct sq_handler_user : public sq_handler
{
	sq_handler_user();
	~sq_handler_user();
	bool init(int n);

	virtual void		poll();

	void				do_acct_register(const MY_MSG_HEAD* msg);
	void				do_user_login(const MY_MSG_HEAD* msg);
	void				do_chat_request(const MY_MSG_HEAD* msg);
	void				do_chat_history_request(const MY_MSG_HEAD* msg);
private:
	sq_record*			m_record;
	sq_record_entry*	m_record_entry;
};

struct sq_user_task : public sq_task
{
	sq_user_task(float fIdleThreshold = 0.1, int nIdleCoef_UserNum = 500);
	virtual ~sq_user_task();


	void RelateTask(sq_user_task* task);

protected:


private:
	std::vector<sq_user_task*> m_vecRelate;
};

struct sq_DailyQuestTask : public sq_user_task
{

};

struct sq_ClearRefreshCntTask : public sq_user_task
{
	sq_ClearRefreshCntTask(int today)
	{
		m_today = today;
	}



	int m_today;
};

struct sq_StoreRefreshTask : public sq_user_task
{
	sq_StoreRefreshTask(int hours)
	{
		m_refresh_time = hours;
	}


	int m_refresh_time;
};

#endif
