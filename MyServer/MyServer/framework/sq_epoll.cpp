/*
 * \file: sq_epoll.cpp
 * \brief: Created by hushouguo at 23:23:06 Jan 02 2020
 */

#include "framework.h"

sq_epoll::sq_epoll()
{
	m_llMaxProcessTick = 0;

#if defined(WIN32)
	FD_ZERO(&m_fdsetRead);
	FD_ZERO(&m_fdsetWrite);
	FD_ZERO(&m_fdsetExcept);
#else
	memset(this->events, 0, sizeof(this->events));
	this->epollid = epoll_create(SELECTOR_EVENT_NUMBER);
	assert(this->epollid >= 0);
#endif
}
sq_epoll::~sq_epoll()
{
#if defined(WIN32)
#else
	close(this->epollid);
#endif
}
bool sq_epoll::insert(sq_network_object* c)
{
#if defined(WIN32)
	m_vecNetObject.push_back(c);
#else
	struct epoll_event ev;
	ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
	ev.data.ptr = c;
	if (epoll_ctl(this->epollid, EPOLL_CTL_ADD, c->fd, &ev) < 0)
	{
		error_log("epoll_insert:%d,%d,%s\n", c->fd, errno, sq_strerror(errno));
		return false;
	}
#endif
	return true;
}
bool sq_epoll::remove(sq_network_object* c)
{
#if defined(WIN32)
	std::vector<sq_network_object*>::iterator it = std::find(m_vecNetObject.begin(), m_vecNetObject.end(), c);
	if (it != m_vecNetObject.end())
		m_vecNetObject.erase(it);
#else
	shutdown(c->fd, SHUT_RDWR);

	struct epoll_event ev;
	ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
	ev.data.ptr = c;
	if (epoll_ctl(this->epollid, EPOLL_CTL_DEL, c->fd, &ev) < 0)
	{
		error_log("epoll_remove:%d,%d,%s\n", c->fd, errno, sq_strerror(errno));
		return false;
	}
#endif
	return true;
}

#if defined(WIN32)
void sq_epoll::SetFdSet()
{
	FD_ZERO(&m_fdsetRead);
//	FD_ZERO(&m_fdsetWrite);
	FD_ZERO(&m_fdsetExcept);

	std::vector<sq_network_object*>::iterator it = m_vecNetObject.begin();
	for (; it != m_vecNetObject.end(); it++)
	{
		sq_network_object* c = *it;

		if (c->active())
		{
			FD_SET(c->fd, &m_fdsetRead);
	//		FD_SET(c->fd, &m_fdsetWrite);
			FD_SET(c->fd, &m_fdsetExcept);
		}
	}
}
#endif

void sq_epoll::poll()
{
	uint64_t llProcessTick = GetTickCount64();

#if defined(WIN32)
	int nEvent;

	struct timeval tv = { 0, 100 };

	for(SetFdSet(); (nEvent = select(0, &m_fdsetRead, NULL, &m_fdsetExcept, &tv)) > 0; SetFdSet())
	{
		std::vector<sq_network_object*> vecNetObject = m_vecNetObject;
		std::vector<sq_network_object*>::iterator it = vecNetObject.begin();
		for (; it != vecNetObject.end(); it++)
		{
			sq_network_object* c = *it;

			if (c->active())
			{
				if (FD_ISSET(c->fd, &m_fdsetRead))
					c->receive();
			}
			//else
			//{
			//	c->lost_connection();
			//	remove(c);
			//}
		}
	}
#else
	int milliseconds = 100;
	int fds = TEMP_FAILURE_RETRY(epoll_wait(this->epollid, this->events, SELECTOR_EVENT_NUMBER, milliseconds));
	if (fds < 0)
	{
		error_log("epoll_wait error:%d,%s\n", errno, sq_strerror(errno));
		return;
	}

	for (int idx = 0; idx < fds; ++idx)
	{
		sq_network_object* c = (sq_network_object*)this->events[idx].data.ptr;
		if (this->events[idx].events & EPOLLERR)
		{
			//error_log("epoll exception:%d,%d,%s\n", this->epollid, errno, sq_strerror(errno));
			c->lost_connection();
		}
		else
		{
			if (c->active())
			{
				if (this->events[idx].events & EPOLLIN) { c->receive(); }
				if (this->events[idx].events & EPOLLOUT) { /* c->send();*/ }
			}
			//else 
			//{ 
			//	c->lost_connection(__LINE__);
			//	this->remove(c);
			//}
		}
	}
#endif

	llProcessTick = GetTickCount64() - llProcessTick;

	if (llProcessTick > m_llMaxProcessTick)
		m_llMaxProcessTick = llProcessTick;
	else
	{
		float fIdleCoef = 1;

		if (m_llMaxProcessTick != 0)
			fIdleCoef = ((float)(m_llMaxProcessTick - llProcessTick)) / m_llMaxProcessTick;

		if (fIdleCoef > 0.8)
			sq_sleep(50 * fIdleCoef); 
	}
}

