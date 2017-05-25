#include "framework.h"
#include "utility.h"

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