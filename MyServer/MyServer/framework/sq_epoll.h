/*
 * \file: sq_epoll.h
 * \brief: Created by hushouguo at 23:23:03 Jan 02 2020
 */
 
#ifndef __SQ_EPOLL_H__
#define __SQ_EPOLL_H__

#define SELECTOR_EVENT_NUMBER	8192
#define SELECTOR_EVENT_TIMEOUT	100

struct sq_epoll
{
	int						epollid;
#if defined(WIN32)
	fd_set					m_fdsetRead, m_fdsetWrite, m_fdsetExcept;
	void					SetFdSet();
	std::vector<sq_network_object*>		m_vecNetObject;
#else
	struct epoll_event 		events[SELECTOR_EVENT_NUMBER];
#endif
	uint64_t m_llMaxProcessTick;

	sq_epoll();
	~sq_epoll();
	bool					insert(sq_network_object* c);
	bool					remove(sq_network_object* c);
	void					poll();
};

#endif
