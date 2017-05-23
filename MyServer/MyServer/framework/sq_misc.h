/*
 * \file: sq_misc.h
 * \brief: Created by hushouguo at 17:44:30 Dec 26 2011
 */
 
#ifndef __SQ_MISC_H__
#define __SQ_MISC_H__

const char* sq_strerror(int err);

uint64_t get_start_time();
uint64_t get_clock_time();

/* "%y-%02m-%02d %02H:%02M:%02S" */
char* timestamp(time_t t, const char* format);

char* sq_reverse(char* s, size_t size);

uint32_t random_value();
float random_float();
int random_between(int min, int max);	// [min, max]
float random_float_between(float min, float max); //[min, max]

bool sq_check_limits();

const char* random_string(size_t length, bool alpha, bool digit);

void* encrylight(void* p, size_t size);
void* decrylight(void* p, size_t size);

//const char* generate_uuid();

const char* domain_lookup(const char* domain);

bool check_one_day(time_t timeA, time_t timeB);

const char* sq_escape_string(const char* str, char c);

bool isnumber(const char* s, size_t len = 0);
void sq_sleep(int nMilliseconds);
#if defined(LINUX)
uint64_t GetTickCount64();
#endif

extern volatile bool sys_halt;
extern volatile rid_t sys_id;

#endif
