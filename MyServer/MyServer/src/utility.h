#ifndef __UTILITY_H__
#define __UTILITY_H__

const char* createUUID();
const char* randomString(size_t length, bool alpha, bool digit);
int randomBetween(int min, int max);
uint32_t hashString(const char* s, size_t size=0);

uint32_t generateUUID();

unsigned long sql_mktime(const MYSQL_TIME tm);

#endif