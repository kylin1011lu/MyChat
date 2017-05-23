/*
 * \file: sq_logger.h
 * \brief: Created by hushouguo at 17:11:19 Dec 03 2011
 */
 
#ifndef __SQ_LOGGER_H__
#define __SQ_LOGGER_H__

typedef enum
{
	BLACK = 30,	RED = 31, 		GREEN = 32, 	BROWN = 33, BLUE = 34, 		MAGENTA = 35, 	CYAN = 36, GREY = 37,
	LRED = 41, 	LGREEN = 42,	YELLOW = 43,	LBLUE = 44,	LMAGENTA = 45,	LCYAN = 46,		WHITE = 47
}COLOR;

#if !defined(STDIN_FILENO) || !defined(STDOUT_FILENO) || !defined(STDERR_FILENO)
	#define	STDIN_FILENO			0
	#define	STDOUT_FILENO			1
	#define	STDERR_FILENO			2
#endif

#ifdef LINUX
	#define	close_stderr() do { dup2(open("/dev/null", O_RDONLY), STDERR_FILENO); } while(0)
	#define close_stdout() do { dup2(open("/dev/null", O_RDONLY), STDOUT_FILENO); } while(0)
	#define close_stdin()  do { dup2(open("/dev/null", O_RDONLY), STDIN_FILENO ); } while(0)
#endif
#ifdef WIN32
	#define close_stderr() do { dup2(open("nul", O_RDONLY), STDERR_FILENO); } while(0)
	#define close_stdout() do { dup2(open("nul", O_RDONLY), STDOUT_FILENO); } while(0)
	#define close_stdin()  do { dup2(open("nul", O_RDONLY), STDIN_FILENO ); } while(0)
	#define snprintf _snprintf
#endif

#define LOGGER_BUFFER_SIZE		2048

#define panic(fmt, ...)\
	do {\
		char __buf__[LOGGER_BUFFER_SIZE];\
		snprintf(__buf__, sizeof(__buf__), "panic:[%s] [%s:%d] "fmt, sq_time::current_timestamp("%y/%m/%d %H:%M:%S"), __FILE__, __LINE__, ##__VA_ARGS__);\
		sq_setcolor(STDOUT_FILENO, LBLUE);\
		write(STDOUT_FILENO, __buf__, strlen(__buf__));\
		sq_resetcolor(STDOUT_FILENO);\
		write(STDERR_FILENO, __buf__, strlen(__buf__));\
		exit(0); \
	}while(0)

#define error_log(fmt, ...)\
	do {\
		char __buf__[LOGGER_BUFFER_SIZE];\
		snprintf(__buf__, sizeof(__buf__), "[%s] [ERR] [%s:%d] "fmt, sq_time::current_timestamp("%y/%m/%d %H:%M:%S"), __FILE__, __LINE__, ##__VA_ARGS__);\
		sq_setcolor(STDOUT_FILENO, LRED);\
		write(STDOUT_FILENO, __buf__, strlen(__buf__));\
		sq_resetcolor(STDOUT_FILENO);\
		write(STDERR_FILENO, __buf__, strlen(__buf__));\
	}while(0)

#define alarm_log(fmt, ...)\
	do {\
		char __buf__[LOGGER_BUFFER_SIZE];\
		snprintf(__buf__, sizeof(__buf__), "[%s] [ALM] [%s:%d] "fmt, sq_time::current_timestamp("%y/%m/%d %H:%M:%S"), __FILE__, __LINE__, ##__VA_ARGS__);\
		sq_setcolor(STDOUT_FILENO, YELLOW);\
		write(STDOUT_FILENO, __buf__, strlen(__buf__));\
		sq_resetcolor(STDOUT_FILENO);\
		write(STDERR_FILENO, __buf__, strlen(__buf__));\
	}while(0)
		
#define debug_log(fmt, ...)\
	do {\
		char __buf__[LOGGER_BUFFER_SIZE];\
		snprintf(__buf__, sizeof(__buf__), "[%s] "fmt, sq_time::current_timestamp("%y/%m/%d %H:%M:%S"), ##__VA_ARGS__);\
		write(STDOUT_FILENO, __buf__, strlen(__buf__));\
	}while(0)

#define lua_error_log(fmt, ...)\
	do {\
		char __buf__[LOGGER_BUFFER_SIZE];\
		snprintf(__buf__, sizeof(__buf__), "[%s] [LUA ERR] [%s:%d] "fmt, sq_time::current_timestamp("%y/%m/%d %H:%M:%S"), __FILE__, __LINE__, ##__VA_ARGS__);\
		sq_setcolor(STDOUT_FILENO, LRED);\
		write(STDOUT_FILENO, __buf__, strlen(__buf__));\
		sq_resetcolor(STDOUT_FILENO);\
		write(STDERR_FILENO, __buf__, strlen(__buf__));\
	}while(0)

#define lua_alarm_log(fmt, ...)\
	do {\
		char __buf__[LOGGER_BUFFER_SIZE];\
		snprintf(__buf__, sizeof(__buf__), "[%s] [LUA ALM] [%s:%d] "fmt, sq_time::current_timestamp("%y/%m/%d %H:%M:%S"), __FILE__, __LINE__, ##__VA_ARGS__);\
		sq_setcolor(STDOUT_FILENO, YELLOW);\
		write(STDOUT_FILENO, __buf__, strlen(__buf__));\
		sq_resetcolor(STDOUT_FILENO);\
		write(STDERR_FILENO, __buf__, strlen(__buf__));\
	}while(0)
		
#define lua_debug_log(fmt, ...)\
	do {\
		char __buf__[LOGGER_BUFFER_SIZE];\
		snprintf(__buf__, sizeof(__buf__), "[%s] [LUA] "fmt, sq_time::current_timestamp("%y/%m/%d %H:%M:%S"), ##__VA_ARGS__);\
		write(STDOUT_FILENO, __buf__, strlen(__buf__));\
	}while(0)

#define note(fmt, ...)\
	do {\
		char __buf__[LOGGER_BUFFER_SIZE];\
		snprintf(__buf__, sizeof(__buf__), fmt, ##__VA_ARGS__);\
		write(STDOUT_FILENO, __buf__, strlen(__buf__));\
	}while(0)

void sq_setcolor(int fd, COLOR color);
void sq_resetcolor(int fd);

bool sq_logger_init(const char* dir, const char* prefix, bool daemon);
void sq_logger_destroy();

#endif
