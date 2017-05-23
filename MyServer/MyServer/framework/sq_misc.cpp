/*
 * \file: sq_misc.cpp
 * \brief: Created by hushouguo at 17:44:33 Dec 26 2011
 */

#ifdef LINUX
#include <netdb.h>
#include <sys/socket.h>
#endif

#include "framework.h"

#define	SYS_ERRNO	128

struct sq_string
{
	uint32_t				size;
	char*					data;
	sq_string() { size = 0; data = NULL; }
	sq_string(char* s) { size = strlen(s) + 1; memcpy(data = (char*)malloc(size), s, size); }
	sq_string(const char* s) { size = strlen(s) + 1; memcpy(data = (char*)malloc(size), s, size); }
	sq_string(void* d, uint32_t n) { size = n; memcpy(data = (char*)malloc(size), d, size); }
	~sq_string() { if (data) { free(data); } }
};
static sq_string*	errlist;

#ifdef LINUX
__attribute__((constructor)) static void sq_strerror_init()
{
	int err;

	errlist = (sq_string*)malloc(SYS_ERRNO * sizeof(sq_string));
	if (!errlist)
	{
		error_log("not enough memory\n");
		return;
	}

	for (err = 0; err < SYS_ERRNO; ++err)
	{
		char* msg = strerror(err);
		size_t size = strlen(msg)+1;
		errlist[err].size = size;
		errlist[err].data = (char*)malloc(size);
		memcpy(errlist[err].data, msg, size);
	}
}
#endif
#if 0
__attribute__((constructor)) static void sq_monitor_init()
{
	const char* s_addr = "0.0.0.0";
	sq_monitor* monitor = new sq_monitor;
	for (unsigned short s_port = 2001; s_port <= 2012; ++s_port)
	{
		if (monitor->open(s_addr, s_port))
		{
			return;
		}
	}
	error_log("without monitor\n");
}
#endif

#ifdef LINUX
static void sq_signal_handler(int s)
{
	debug_log("Signal: %d received\n", s);
	switch (s)
	{
		case SIGHUP: case SIGUSR1: case SIGUSR2: break;
		case SIGINT: case SIGTERM: case SIGQUIT: sys_halt = true; break;
		default: assert(false); break;
	}
	signal(s, sq_signal_handler);
}
__attribute__((constructor)) static void sq_signal_init()
{
	//for (int i = SIGHUP; i <= SIGUNUSED; i++)
	//	signal(i, sq_signal_handler);

	signal(SIGHUP, sq_signal_handler);
	signal(SIGINT, sq_signal_handler);
	signal(SIGTERM, sq_signal_handler);
	signal(SIGQUIT, sq_signal_handler);
	signal(SIGUSR1, sq_signal_handler);
	signal(SIGUSR2, sq_signal_handler);
}
bool sq_check_limits()
{
	struct rlimit limit;
	if (getrlimit(RLIMIT_CORE, &limit))
	{
		error_log("cannot get RLIMIT_CORE:%s\n", strerror(errno));
		return false;
	}
	if (limit.rlim_max != RLIM_INFINITY)
	{
		alarm_log("RLIMIT_CORE:max %u too small, you should run `ulimit -c` to setup\n", (uint32_t)limit.rlim_max);
	}
	if (limit.rlim_cur != RLIM_INFINITY)
	{
		alarm_log("RLIMIT_CORE:cur %u too small, you should run `ulimit -c` to setup\n", (uint32_t)limit.rlim_cur);
	}

	if (getrlimit(RLIMIT_NOFILE, &limit))
	{
		error_log("cannot get RLIMIT_NOFILE:%s\n", strerror(errno));
		return false;
	}	
	//debug_log("RLIMIT_NOFILE: rlim_cur:%u, rlim_max:%u\n", (uint32_t)limit.rlim_cur, (uint32_t)limit.rlim_max);
	if (limit.rlim_max < 10000)
	{
		alarm_log("RLIMIT_NOFILE:%u too small, you should run `ulimit -n` to setup\n", (uint32_t)limit.rlim_max);
	}
	
	if (getrlimit(RLIMIT_STACK, &limit))
	{
		error_log("cannot get RLIMIT_STACK:%s\n", strerror(errno));
		return false;
	}
	//debug_log("RLIMIT_STACK: rlim_cur:%u, rlim_max:%u\n", (uint32_t)limit.rlim_cur/1024, (uint32_t)limit.rlim_max/1024);
	if (limit.rlim_max < 100*MB)
	{
		alarm_log("RLIMIT_STACK:%u too small, you should run `ulimit -s` to setup\n", (uint32_t)limit.rlim_max);
	}

	return true;
}
#endif
const char* sq_strerror(int err)
{
	if (err >= 0 && err < SYS_ERRNO)
	{
		return errlist[err].data;
	}
	return "invalid errno";
}
char* sq_reverse(char* s, size_t size)
{
	for (size_t i = 0; i < size/2; ++i)
	{
		char c = s[i];
		s[i] = s[size-i-1];
		s[size-i-1] = c;
	}
	return s;
}

volatile bool sys_halt = false;
volatile rid_t sys_id = 0;

static uint64_t sys_start_time = get_clock_time();
static const char* light_key = "@^2A-PK.645*&a=PP\\\"'y#";

uint64_t get_start_time()
{
	return sys_start_time;
}
uint64_t get_clock_time()
{
    time_t tm;
	time(&tm);
  //  if (clock_gettime(CLOCK_MONOTONIC, &tm) == -1) 
  //  {
		//error_log("clock_gettime error:%d,%s\n", errno, sq_strerror(errno));
		//return 0;
  //  }
    //return (uint64_t)(tm.tv_sec * 1000 + tm.tv_nsec / 1000000);
	return tm;
}
//uint32_t get_clock_nsec()
//{
//    timespec tm;
//    if (clock_gettime(CLOCK_REALTIME, &tm) == -1) 
//    {
//		return 0;
//    }
//    return tm.tv_nsec;
//}
char* timestamp(time_t t, const char* format)
{
	static char _time_buffer[24];
	struct tm _tm;
	localtime_r(&t, &_tm);
	strftime((char*)_time_buffer, sizeof(_time_buffer), format, &_tm);
	return _time_buffer;
}
static unsigned int s_seed = (unsigned int)time((time_t*)0);
//static unsigned int s_seed = get_clock_nsec();
uint32_t random_value()
{
	static volatile uint32_t startime = (uint32_t)time(0);
	/* startime as seed */
	return rand_r((uint32_t*)&startime);
}
float random_float()
{
	return (float)random_value()/double(RAND_MAX);
}
int random_between(int min, int max)
{
	s_seed = 214013 * s_seed + 2531011;

	if (min < max)
	{
		return min + (s_seed ^ s_seed >> 15) % (max - min + 1);
	}
	else
	{
		return max + (s_seed ^ s_seed >> 15) % (min - max + 1);
	}
}
float random_float_between(float min, float max)
{
	s_seed = 214013 * s_seed + 2531011;

	if (min < max)
	{
		return min + (s_seed >> 16) * (1.0f/65535.0f) * (max - min);
	}
	else
	{
		return max + (s_seed >> 16) * (1.0f/65535.0f) * (min - max);
	}
}
void* encrylight(void* p, size_t size)
{
	char *s = (char*)p, *key = (char*)light_key;

	while(size-- > 0)
	{
		*s++ ^= *key++;
		if (*key == '\0') { key = (char*)light_key; }
	}

	return p;
}
void* decrylight(void* p, size_t size)
{
	return encrylight(p, size);
}
/*
const char* generate_uuid()
{
	static char __static_uuid[96];

	uuid_t uuid;
	uuid_generate(uuid);

	snprintf(__static_uuid, sizeof(__static_uuid), "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
			uuid[0], uuid[1], uuid[2], uuid[3],
			uuid[4], uuid[5],
			uuid[6], uuid[7],
			uuid[8], uuid[9],
			uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]
			);

	return __static_uuid;
}
*/
const char* domain_lookup(const char* domain)
{
	static char __address_buf[32];
	struct hostent* p = gethostbyname(domain);
	if (!p)
	{
		error_log("lookup domain:%s failure\n", domain);
		return NULL;
	}

	switch (p->h_addrtype)
	{
		case AF_INET:
			{
				char** pp = p->h_addr_list;
				for (; *pp; ++pp)
				{
					//debug_log("\tip:%s\n", inet_ntop(p->h_addrtype, *pp, __address_buf, sizeof(__address_buf)));
					return inet_ntop(p->h_addrtype, *pp, __address_buf, sizeof(__address_buf));
				}
			}
			break;
		case AF_INET6: 
			error_log("NOT SUPPORT IPV6 ADDRESS\n"); break;
		default: error_log("UNKNOWN ADDRESS FORMAT\n"); break;
	}

	return NULL;	
}
bool check_one_day(time_t timeA, time_t timeB)
{
	struct tm tmA, tmB;

	localtime_r(&timeA, &tmA);
	localtime_r(&timeB, &tmB);

	return tmA.tm_year == tmB.tm_year && tmA.tm_mon == tmB.tm_mon && tmA.tm_mday == tmB.tm_mday;
}
const char* random_string(size_t length, bool alpha, bool digit)
{
	static char buffer[960];

	if (!alpha && !digit) 
	{ 
		buffer[0] = '\0';
		return buffer; 
	}

	if (length >= (sizeof(buffer) - 1)) { length = sizeof(buffer) - 2; }
	for (uint32_t n = 0; n < length; ++n)
	{
		if (alpha && digit)
		{
			if (random_between(0,1) == 0)
			{
				buffer[n] = random_between('a', 'z');
			}
			else
			{
				buffer[n] = random_between('0', '9');
			}
		}
		else if (alpha)
		{
			buffer[n] = random_between('a', 'z');
		}
		else
		{
			buffer[n] = random_between('0', '9');
		}
	}
	buffer[length] = '\0';
	return buffer;
}
const char* sq_escape_string(const char* str, char c)
{
	static int n = 0;
	static char buffer[65535];

	n = 0;
	const char* p = str;
	
	while (*p != '\0')
	{
		if (*p == c) 
		{ 
			buffer[n++] = '\\'; 
		}
		buffer[n++] = *p;
		++p;
	}

	buffer[n] = '\0';

	return buffer;
}
bool isnumber(const char* s, size_t len)
{
	if (len == 0) { len = strlen(s); }
	for (size_t n = 0; n < len; ++n)
	{
		if (!isdigit(s[n])) { return false; }
	}
	return true;
}
void sq_sleep(int nMilliseconds)
{
#if defined(WIN32)
	Sleep(nMilliseconds);
#else
	struct timespec req;
	req.tv_sec = nMilliseconds / 1000;
	req.tv_nsec = (nMilliseconds % 1000) * 1000 * 1000;
	nanosleep(&req, NULL);
#endif
}
#if defined(LINUX)
uint64_t GetTickCount64()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec * 1000 + ts.tv_nsec / (1000 * 1000));
}
#endif
