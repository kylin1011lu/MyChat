#ifndef __UTILITY_H__
#define __UTILITY_H__

static uint32_t s_seed = ::time(0);

const char* createUUID();
const char* randomString(size_t length, bool alpha, bool digit);
int randomBetween(int min, int max);
uint32_t hashString(const char* s, size_t size=0);

uint32_t generateUUID();

#endif