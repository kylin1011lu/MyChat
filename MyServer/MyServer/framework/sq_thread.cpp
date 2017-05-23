/*
 * \file: sq_thread.cpp
 * \brief: Created by hushouguo at 17:12:57 Dec 03 2011
 */

#include "framework.h"
 
volatile uint32_t sys_cpus = 1;

void spinlock_lock(sq_atomic* locker, const char* prefix)
{
	uint32_t i, count = 0, n = 0;
	for (;;++count) 
	{
		if (*locker == 0 && atomic_cmp_set(locker, 0, 1)) 
		{
			//if (count > 0) { alarm_log("spinlock wait count:%d, prefix:%s\n", count, prefix); }
			return; 
		}
		if (sys_cpus > 1)
		{
			for (n = 1; n < 16; n <<= 1) 
			{
				for (i = 0; i < n; i++) 
				{
					pause(); 
				}				 
				if (*locker == 0 && atomic_cmp_set(locker, 0, 1)) 
				{ 
					//if (count > 0) { alarm_log("spinlock wait count:%d, prefix:%s\n", count, prefix); }
					return; 
				}
			}
			//alarm_log("spinlock wait for lock, cpus:%d\n", sys_cpus);
		}
		sched_yield();
	}
}
bool spinlock_trylock(sq_atomic* locker)
{
	return (*(locker) == 0 && atomic_cmp_set(locker, 0, 1));
}
void spinlock_unlock(sq_atomic* locker)
{
	//atomic_cmp_set(locker, 1, 0);
	*locker = 0;
}
void simplelock_lock(pthread_mutex_t* locker)
{
	if (pthread_mutex_lock(locker) < 0)
	{
		error_log("pthread_mutex_lock:%d,%s\n", errno, sq_strerror(errno));
	}
}
bool simplelock_trylock(pthread_mutex_t* locker)
{
	int ret;
	if ((ret = pthread_mutex_trylock(locker)) < 0)
	{
		if (ret != EBUSY)
		{
			error_log("pthread_mutex_trylock:%d,%s\n", errno, sq_strerror(errno));
		}
		return false;
	}
	return true;
}
void simplelock_unlock(pthread_mutex_t* locker)
{
	if (pthread_mutex_unlock(locker) < 0)
	{
		error_log("pthread_mutex_unlock:%d,%s\n", errno, sq_strerror(errno));
	}
}

static void* static_runnable(void* p)
{
	sq_thread* thread = (sq_thread*)p;

	//debug_log("thread:%s is running with stack size:%u\n", thread->name, (uint32_t)thread->stacksize);
	debug_log("thread:%s is running\n", thread->name);
	thread->runnable(thread->p);
	debug_log("thread:%s is terminated\n", thread->name);

	thread->running = false;
	pthread_exit(NULL);

	return NULL;
}

static sq_thread* thread_pool = NULL;

sq_thread* sq_thread_create(runnable_t runnable, size_t stacksize, const char* name, void* p)
{
	sq_thread* thread = (sq_thread*)malloc(sizeof(sq_thread));

	if (pthread_attr_init(&thread->attr) < 0)
	{
		error_log("pthread_attr_init:%d,%s\n", errno, sq_strerror(errno));
		free(thread); 
		return NULL;
	}

	pthread_attr_setdetachstate(&thread->attr, PTHREAD_CREATE_DETACHED);

	if (pthread_attr_setstacksize(&thread->attr, stacksize) < 0)
	{
		error_log("pthread_attr_setstacksize:%d,%s\n", errno, sq_strerror(errno));
		free(thread); 
		return NULL;
	}

	if (pthread_attr_getstacksize(&thread->attr, &thread->stacksize) < 0)
	{
		error_log("pthread_attr_getstacksize:%d,%s\n", errno, sq_strerror(errno));
		free(thread);
		return NULL;
	}

	strncpy(thread->name, name, sizeof(thread->name));
	/*thread->stacksize = stacksize;*/
	thread->runnable = runnable;
	thread->p = p;
	thread->running = true;

	if (pthread_create(&thread->pid, &thread->attr, static_runnable, thread))
	{
		error_log("pthread_create:%d,%s\n", errno, sq_strerror(errno));
		free(thread); 
		return NULL;
	}

	return thread;
}

void thread_create(void* (*thread_fun)(void*), size_t stacksize, void* p)
{
	pthread_t		 			pid;
	pthread_attr_t  			attr;

	if (pthread_attr_init(&attr) < 0)
	{
		error_log("pthread_attr_init:%d,%s\n", errno, sq_strerror(errno));
		return ;
	}

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if (stacksize)
	{
		if (pthread_attr_setstacksize(&attr, stacksize) < 0)
		{
			error_log("pthread_attr_setstacksize:%d,%s\n", errno, sq_strerror(errno));
			return;
		}

		if (pthread_attr_getstacksize(&attr, &stacksize) < 0)
		{
			error_log("pthread_attr_getstacksize:%d,%s\n", errno, sq_strerror(errno));
			return;
		}
	}

	if (pthread_create(&pid, &attr, thread_fun, p))
	{
		error_log("pthread_create:%d,%s\n", errno, sq_strerror(errno));
		return;
	}
}

#if defined(LINUX)
__attribute__((constructor)) static void sq_thread_pool_init()
{
	/* note: fetch number of cpu */
	int i = sysconf(_SC_NPROCESSORS_CONF);
	if (i < 0)
	{
		error_log("cannot fetch _SC_NPROCESSORS_CONF value\n");
		sys_cpus = 1;
	}
	else
	{
		sys_cpus = i;
	}
	//debug_log("detect %d processor\n", sys_cpus);
}
#endif
bool sq_thread_pool_insert(const char* name, runnable_t runnable, void* p)
{
	sq_thread* thread = sq_thread_create(runnable, 16*MB, name, p);
	if (!thread)
	{
		error_log("cannot create thread:%d,%s\n", errno, sq_strerror(errno));
		return false;
	}
	thread->next = thread_pool;
	thread_pool = thread;
	return true;
}
void sq_thread_pool_exit()
{
	sq_thread *thread = thread_pool, *temp = NULL;
	while(thread)
	{
		if (thread->running)
		{
			sched_yield();
			//usleep(100); 
			continue; 
		}
		temp = thread;
		thread = thread->next;
		free(temp);
	}
	thread_pool = NULL;
}
