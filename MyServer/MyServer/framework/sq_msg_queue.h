/*
 * \file: sq_msg_queue.h
 * \brief: Created by hushouguo at Jun 23 2014 15:53:48
 */
 
#ifndef __SQ_MSG_QUEUE_H__
#define __SQ_MSG_QUEUE_H__

struct sq_msg_queue
{
	std::list<MY_MSG_HEAD*>				q;
	sq_atomic									cache_locker;

	sq_msg_queue();
	~sq_msg_queue();

	void										push_back(SOCKET s, MY_MSG_HEAD* msg);
	void										pop_front(std::list<MY_MSG_HEAD*>& out);
	bool										empty();
	void										release(MY_MSG_HEAD* msg);
};

#endif
