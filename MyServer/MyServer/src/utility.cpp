#include "framework.h"
#include "utility.h"

static unsigned int s_seed = (unsigned int)time((time_t*)0);

const char* createUUID()
{
	return randomString(8, true, false);
}

const char* randomString(size_t length, bool alpha, bool digit)
{
	static char buffer[960];

	if (!alpha && !digit)
	{
		buffer[0] = '\0';
		return buffer;
	}

	if (length >= (sizeof(buffer)-1)) { length = sizeof(buffer)-2; }
	for (uint32_t n = 0; n < length; ++n)
	{
		if (alpha && digit)
		{
			if (randomBetween(0, 1) == 0)
			{
				buffer[n] = randomBetween('a', 'z');
			}
			else
			{
				buffer[n] = randomBetween('0', '9');
			}
		}
		else if (alpha)
		{
			buffer[n] = randomBetween('a', 'z');
		}
		else
		{
			buffer[n] = randomBetween('0', '9');
		}
	}
	buffer[length] = '\0';
	return buffer;
}

int randomBetween(int min, int max)
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

uint32_t hashString(const char* s, size_t size)
{
	if (size == 0) { size = strlen(s); }
	uint32_t h = 0, g;
	char* end = (char*)s + size;
	while (s < end)
	{
		h = (h << 4) + *s++;
		if ((g = (h & 0xF0000000)))
		{
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}

uint32_t generateUUID()
{
	return hashString(createUUID());
}

unsigned long sql_mktime(const MYSQL_TIME tm)
{
	struct tm _tm;
	_tm.tm_hour		= tm.hour;
	_tm.tm_min		= tm.minute;
	_tm.tm_sec = tm.second;
	_tm.tm_year = tm.year - 1900;
	_tm.tm_mon = tm.month - 1;
	_tm.tm_mday = tm.day;

	return mktime(&_tm);
}