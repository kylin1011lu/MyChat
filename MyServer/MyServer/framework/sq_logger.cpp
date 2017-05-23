/*
 * \file: sq_logger.c
 * \brief: Created by hushouguo at 17:11:22 Dec 03 2011
 */

#include "framework.h"
 
void sq_setcolor(int fd, COLOR color)
{
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "\x1b[%d%sm", color >= LRED ? (color - 10) : color, color >= LRED ? ";1" : "");
	write(fd, buffer, strlen(buffer));
}
void sq_resetcolor(int fd)
{
	const char* s = "\x1b[0m";
	write(fd, s, strlen(s));
}
#ifdef LINUX
int create_folder(const char* path)
{
	char name[1024];

	snprintf(name, sizeof(name), "%s", path);
	int i,len = strlen(name);
	if (len > 0 && name[len-1] != '/')
	{
		strcat(name, "/");
	}

	len = strlen(name);

	for (i = 1; i < len; i++)
	{
		if (name[i] == '/')
		{
			name[i] = 0;
			if (access(name, X_OK) != 0)
			{
				if (access(name, F_OK) == 0)
				{
					panic("directory:%s exist, but can't be executed\n", name);
					return -1;
				}
				if (mkdir(name, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) == -1)
				{
					panic("cannot mkdir:%s,%d,%s\n", name, errno, sq_strerror(errno));
					return -1;
				}
			}
			name[i] = '/';
		}
	}
	return 0;
}
const char* sq_replace_env(char* __out_buf, uint32_t size, const char* src, char cr)
{
	char __src[1024];
	strncpy(__src, src, sizeof(__src));

	memset(__out_buf, 0, size);

	char* pfirst = (char*)__src;
	char* p = pfirst;
	while((p = strchr(pfirst, cr)) != NULL)
	{
		char* pp = p+1;
		char __buf[1024];
		uint32_t n = 0;
		while(*pp != '\0' && (isalnum(*pp) || *pp == '_')) 
		{
			__buf[n++] = *pp++;
		}
		__buf[n] = '\0';

		char* env = getenv(__buf);

		*p = '\0';
		strncat(__out_buf, pfirst, size);

		if (env)
		{
			strncat(__out_buf, env, size);
		}
		else
		{
			char __tmp[8];
			snprintf(__tmp, sizeof(__tmp), "%c", cr);

			strncat(__out_buf, __tmp, size);
			strncat(__out_buf, __buf, size);
		}

		pfirst = pp;
	}

	strncat(__out_buf, pfirst, size);

	return __out_buf;
}
/* pattern:
   %p: Process id
   %e: Executable name
   %y: Year
   %M: Month
   %d: Day
   %h: Hour
   %m: Minute
   %s: Second
   %S: Shard name
   %i: service Id
   %P: service Port
   %v: built Version
*/
void sq_fullname(const char* pattern, char* outbuf, uint32_t outlen)
{
	//save old env variables
	const char* p = getenv("p");
	const char* e = getenv("e");
	const char* y = getenv("y");
	const char* M = getenv("M");
	const char* d = getenv("d");
	const char* h = getenv("h");
	const char* m = getenv("m");
	const char* s = getenv("s");
	const char* S = getenv("S");
	const char* i = getenv("i");
	const char* P = getenv("P");
	const char* v = getenv("v");

	//setup env variables
	char pp[32];
	snprintf(pp, sizeof(pp), "%u", getpid());
	setenv("p", (const char*)pp, 1);

	setenv("e", "UNKNOWN", 1);//todo: `e` env unavailable

	char yy[32];
	snprintf(yy, sizeof(yy), "%04u", currtime.year());
	setenv("y", (const char*)yy, 1);

	char MM[32];
	snprintf(MM, sizeof(MM), "%02u", currtime.month());
	setenv("M", (const char*)MM, 1);

	char dd[32];
	snprintf(dd, sizeof(dd), "%02u", currtime.day());
	setenv("d", (const char*)dd, 1);

	char hh[32];
	snprintf(hh, sizeof(hh), "%02u", currtime.hour());
	setenv("h", (const char*)hh, 1);

	char mm[32];
	snprintf(mm, sizeof(mm), "%02u", currtime.minute());
	setenv("m", (const char*)mm, 1);

	char ss[32];
	snprintf(ss, sizeof(ss), "%02u", currtime.second());
	setenv("s", (const char*)ss, 1);

	setenv("S", "UNKNOWN", 1);

	setenv("i", "UNKNOWN", 1);

	setenv("P", "UNKNOWN", 1);

	setenv("v", "UNKNOWN", 1);

	sq_replace_env(outbuf, outlen, pattern, '%');

	//reset old env variables
	setenv("p", p, 1);
	setenv("e", e, 1);
	setenv("y", y, 1);
	setenv("M", M, 1);
	setenv("d", d, 1);
	setenv("h", h, 1);
	setenv("m", m, 1);
	setenv("s", s, 1);
	setenv("S", S, 1);
	setenv("i", i, 1);
	setenv("P", P, 1);
	setenv("v", v, 1);
}
static bool create_utf8_encoding_file(const char* filename)
{
	int fd = -1;

	umask(0);

	bool exist = access(filename, F_OK) != -1;

	if ((fd = open(filename, O_CREAT|O_APPEND|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP)) < 0)
	{
		error_log("cannot open file:%s,%d,%s\n", filename, errno, sq_strerror(errno));
		return false;
	}

	if (!exist)
	{
		char header[3] = {0xef, 0xbb, 0xbf};  // UTF-8 file header
		write(fd, header, sizeof(header));
	}

	close(fd);

	return true;
}
#endif
bool sq_logger_init(const char* dir, const char* prefix, bool daemon)
{
#if defined(LINUX)
	if (create_folder(dir)) { return false; }

	if (daemon)
	{
		const char* fullname = currtime.timestamp("%Y%m%d");

		char fullpath[960];
		snprintf(fullpath, sizeof(fullpath), "%s/%s.%s", dir, prefix, fullname);

		if (!create_utf8_encoding_file(fullpath)) { return false; }

		int fd = open(fullpath, O_CREAT|O_APPEND|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP);
		if (fd < 0)
		{
			error_log("cannot open file:%s,%d,%s\n", fullpath, errno, sq_strerror(errno));
			return false;
		}

		/* create normal logger file */
		if (fd != STDOUT_FILENO)
		{
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}

		/* create error logger file */
		snprintf(fullpath, sizeof(fullpath), "%s/%s.%s.error", dir, prefix, fullname);

		if (!create_utf8_encoding_file(fullpath)) { return false; }

		fd = open(fullpath, O_CREAT|O_APPEND|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP);
		if (fd < 0)
		{
			error_log("cannot open file:%s,%d,%s\n", fullpath, errno, sq_strerror(errno));
			return false;
		}

		if (fd != STDERR_FILENO)
		{
			dup2(fd, STDERR_FILENO);
			close(fd);
		}
	}
#endif

	return true;
}
void sq_logger_destroy()
{
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}
