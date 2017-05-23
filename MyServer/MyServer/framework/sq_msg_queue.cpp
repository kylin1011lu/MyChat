/*
 * \file: sq_msg_queue.cpp
 * \brief: Created by hushouguo at Jun 23 2014 15:53:50
 */

#include "framework.h"

sq_msg_queue::sq_msg_queue()
{
	cache_locker = 0;
}
sq_msg_queue::~sq_msg_queue()
{
}
void sq_msg_queue::push_back(SOCKET s, MY_MSG_HEAD* msg)
{
	if (msg == NULL)
		msg = new MY_MSG_HEAD;

	msg->s = s;

	spinlock_lock(&cache_locker, "msg_queue::push_back::cache_pool");
	q.push_back((MY_MSG_HEAD*)msg);
	spinlock_unlock(&cache_locker);
}
void sq_msg_queue::pop_front(std::list<MY_MSG_HEAD*>& out)
{
	MY_MSG_HEAD* msg = NULL;

	spinlock_lock(&cache_locker, "msg_queue::pop_front");
	q.swap(out);
	spinlock_unlock(&cache_locker);
}
bool sq_msg_queue::empty()
{
	return q.empty();
}
void sq_msg_queue::release(MY_MSG_HEAD* msg)
{
	delete msg->msg;
	msg->msg = NULL;
	delete msg;
	msg = NULL;
}

