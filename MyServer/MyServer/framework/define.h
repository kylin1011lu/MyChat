/*
 * \file: define.h
 * \brief: Created by hushouguo at 17:10:45 Dec 03 2011
 */
 
#ifndef __DEFINE_H__
#define __DEFINE_H__

#if !defined(__x86_64__) || !defined(__LP64__)
//#error("ONLY RUN ON X86_64 PLATFORM!")
#endif

#if 0
typedef long long  						int64_t;
typedef int         					int32_t;
typedef short       					int16_t;
typedef char							int8_t;
typedef unsigned long long				uint64_t;
typedef unsigned int  					uint32_t;
typedef unsigned short  				uint16_t;
typedef unsigned char   				uint8_t;
#endif

typedef unsigned char					BYTE;

#if !defined(__cplusplus)
typedef int								bool;
#define true							1
#define false							0
#endif

typedef unsigned int					sid_t;
typedef unsigned int					uid_t;
typedef unsigned long long				cid_t;
typedef unsigned short					rid_t;

//#define min(a,b) 						(((a) > (b)) ? (b) : (a))
//#define max(a,b) 						(((a) > (b)) ? (a) : (b))

#define LF     							(byte_t)10
#define CR     							(byte_t)13
#define CRLF   							"\x0d\x0a"

//#define PI							3.14159265358979323846f

#define	KB								1024U
#define MB								1048576U
#define GB								1073741824U
#define TB								1099511627776ULL

#define MINUTE							60U
#define HOUR							3600U
#define DAY								86400U

#define INVALID_ADDRESS					0
#define INVALID_PORT					0

#ifndef PATH_MAX
	#define PATH_MAX					255
#endif

#if !defined(TEMP_FAILURE_RETRY)
#define TEMP_FAILURE_RETRY(EXEC)		EXEC
#endif

#if !defined(MSG_DONTWAIT)
#define MSG_DONTWAIT					0
#endif

#if !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL					0
#endif

#define SQ_ASSERT(exp)					do { if (!(exp)) panic("assert:%s\n", #exp); } while(0)

#define SQ_SAFE_DELETE(p)				do { if(p) { delete (p); (p) = 0; } } while(0)
#define SQ_SAFE_DELETE_ARRAY(p)			do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define SQ_SAFE_FREE(p)					do { if(p) { free(p); (p) = 0; } } while(0)

#endif
