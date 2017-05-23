/*
 * \file: sq_timer.cpp
 * \brief: Created by hushouguo at 18:01:16 Dec 26 2011
 */

#include "framework.h"

void sq_timer::set(unsigned int inter)
{
	interval = inter;
	next();
}
unsigned int sq_timer::get()
{
	return interval;
}
void sq_timer::next() 
{ 
	scale.now(); 
}
bool sq_timer::operator()(sq_time& current)
{
	// ATTENTION: interval maybe overflow! change to negative
	if ((current - scale) >= (int)interval)
	{
		next();
		return true;
	}
	return false;
}

