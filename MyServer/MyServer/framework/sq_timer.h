/*
 * \file: sq_timer.h
 * \brief: Created by hushouguo at 18:01:13 Dec 26 2011
 */
 
#ifndef __SQ_TIMER_H__
#define __SQ_TIMER_H__

// base on millisecond
struct sq_timer
{
	sq_time							scale;
	unsigned int 					interval;
	sq_timer(unsigned int inter) : interval(inter)
	{
		next();
	}
	void 							set(unsigned int inter);
	unsigned int 					get();
	void 							next();
	bool 							operator()(sq_time& current);
};

#endif
