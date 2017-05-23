/*
 * \file: sq_time.cpp
 * \brief: Created by hushouguo at 18:00:57 Dec 26 2011
 */

#include "framework.h"

sq_time::sq_time()
{
	//-lrt
	//clock_gettime(CLOCK_REALTIME, &ts);
	now();
}
//sq_time::sq_time(unsigned int sec, unsigned int msec)
//{
//	ts.tv_sec = sec;
//	ts.tv_nsec = msec*1000L*1000L;
//}

void sq_time::now()
{
	//clock_gettime(CLOCK_MONOTONIC, &ts);
	//clock_gettime(CLOCK_REALTIME, &ts);
	time(&ts);
}
unsigned int sq_time::sec()
{
	return ts;
}
//unsigned int sq_time::msec()
//{
//	return ts.tv_nsec/1000000L;
//}
//unsigned int sq_time::nsec()
//{
//	return ts.tv_nsec;
//}
int sq_time::operator-(sq_time& t)
{
	//return (this->sec() - t.sec())*1000L + (this->msec() - t.msec());
	return this->sec() - t.sec();
}
bool sq_time::operator>(sq_time& t)
{
	//return (this->sec() > t.sec()) || (this->sec() == t.sec() && this->nsec() > t.nsec());
	return this->sec() > t.sec();
}
bool sq_time::operator<(sq_time& t)
{
	//return (this->sec() < t.sec()) || (this->sec() == t.sec() && this->nsec() < t.nsec());
	return this->sec() < t.sec();
}
void sq_time::operator=(sq_time& t)
{
	//ts.tv_sec = t.sec();
	//ts.tv_nsec = t.nsec();
	ts = t.ts;
}
unsigned int sq_time::week()
{
	struct tm _tm;
	localtime_r(&ts, &_tm);
	return _tm.tm_wday;
}
unsigned int sq_time::year()
{
	struct tm _tm;
	localtime_r(&ts, &_tm);
	return _tm.tm_year+1900;
}
unsigned int sq_time::month()
{
	struct tm _tm;
	localtime_r(&ts, &_tm);
	return _tm.tm_mon+1;
}
unsigned int sq_time::day()
{
	struct tm _tm;
	localtime_r(&ts, &_tm);
	return _tm.tm_mday;
}
unsigned int sq_time::hour()
{
	struct tm _tm;
	localtime_r(&ts, &_tm);
	return _tm.tm_hour;
}
unsigned int sq_time::minute()
{
	struct tm _tm;
	localtime_r(&ts, &_tm);
	return _tm.tm_min;
}
unsigned int sq_time::second()
{
	struct tm _tm;
	localtime_r(&ts, &_tm);
	return _tm.tm_sec;
}
char* sq_time::timestamp(const char* format)
{
	static char _time_buffer[24];
	struct tm _tm;
	localtime_r(&ts, &_tm);
	strftime(_time_buffer, sizeof(_time_buffer), format, &_tm);
	return _time_buffer;
}
char* sq_time::timestamp(time_t t, const char* format)
{
	static char _time_buffer[24];
	struct tm _tm;
	localtime_r(&t, &_tm);
	strftime(_time_buffer, sizeof(_time_buffer), format, &_tm);
	return _time_buffer;
}
char* sq_time::current_timestamp(const char* format)
{
	static char _time_buffer[24];
	time_t cur = time((time_t*)0);
	struct tm _tm;
	localtime_r(&cur, &_tm);
	strftime(_time_buffer, sizeof(_time_buffer), format, &_tm);
	return _time_buffer;
}

sq_time currtime;

long sq_timezone = 0;

#define UTC_8	28800

int DaysOfToday(time_t* tm, long tz, int spans)
{
	time_t ltime = tm ? *tm : time(NULL);

	if (tz == -1)
		tz = sq_timezone;

	return (ltime + tz) / spans;
}

int HoursOfToday(int hours, long tz)
{
	time_t ltime = time(NULL);

	if (tz == -1)
		tz = sq_timezone;

	ltime += tz;

	int days = ltime / (24 * 60 * 60);

	if (hours < 0)
		hours = (ltime / (60 * 60)) % 24;

	return (days << 5) + hours;
}
