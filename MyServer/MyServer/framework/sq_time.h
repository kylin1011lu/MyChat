/*
 * \file: sq_time.h
 * \brief: Created by hushouguo at 17:59:54 Dec 26 2011
 */
 
#ifndef __SQ_TIME_H__
#define __SQ_TIME_H__

struct sq_time
{
	time_t	 						ts;
	sq_time();
//	sq_time(unsigned int sec, unsigned int msec);
	void 							now();
	unsigned int 					sec();
	//unsigned int 					msec();
	//unsigned int 					nsec();
	// ATTENTION: the result maybe is negative!
	int 							operator-(sq_time& t);
	bool 							operator>(sq_time& t);
	bool 							operator<(sq_time& t);
	void 							operator=(sq_time& t);
	unsigned int 					year();
	unsigned int 					month();
	unsigned int 					day();
	unsigned int 					hour();
	unsigned int 					minute();
	unsigned int 					second();
	unsigned int 					week();
	// ATTENTION: non-reentrants
	char* 							timestamp(const char* format = "%y/%02m/%02d %02H:%02M:%02S");
	static char* 					timestamp(time_t t, const char* format = "%y/%02m/%02d %02H:%02M:%02S");
	static char* 					current_timestamp(const char* format = "%y/%02m/%02d %02H:%02M:%02S");
};

#define SpanOfTime(time_diff, interval) ((time_diff) > 0 ? (time_diff) / (interval) : 0)

extern sq_time currtime;
extern long sq_timezone;

extern int DaysOfToday(time_t* tm = NULL, long tz = -1, int spans = (24 * 60 * 60));
extern int HoursOfToday(int hours = -1, long tz = -1);

#endif
