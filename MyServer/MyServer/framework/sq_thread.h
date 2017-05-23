/*
 * \file: sq_thread.h
 * \brief: Created by hushouguo at 17:12:55 Dec 03 2011
 */
 
#ifndef __SQ_THREAD_H__
#define __SQ_THREAD_H__

#if defined(WIN32)
#define pause()         
#else
#if (__i386__ || __i386 || __amd64__ || __amd64)
#define pause()         __asm__("pause")
#else
#define pause()
#endif
#endif

#if ((__GNUC__ > 4) || (__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
#define atomic_cmp_set(lock, old, set)	__sync_bool_compare_and_swap(lock, old, set)
#define atomic_fetch_add(value, add)	__sync_fetch_and_add(value, add)
#else
#define atomic_cmp_set(lock, old, set)	(InterlockedCompareExchangePointer(lock, (PVOID)set, (PVOID)old) == (PVOID)old)
//#error("GCC version is too low!")
#endif

#ifdef WIN32
#define atomic_cmp_set(lock, old, set)	(InterlockedCompareExchangePointer(lock, (PVOID)set, (PVOID)old) == (PVOID)old)
#define atomic_val_set(lock, set, old)	((PVOID)(LONG_PTR)InterlockedCompareExchange((LONG volatile *)lock, (LONG)(LONG_PTR)set, (LONG)(LONG_PTR)old))
#define atomic_fetch_add(value, add)	InterlockedExchangeAdd(value, add)
#define atomic_inc(value)				InterlockedIncrement(value)
#else
#define atomic_cmp_set(lock, old, set)	__sync_bool_compare_and_swap(lock, old, set)
#define atomic_val_set(lock, set, old)	__sync_val_compare_and_swap(lock, old, set)
#define atomic_fetch_add(value, add)	__sync_fetch_and_add(value, add)
#define atomic_inc(value)				__sync_add_and_fetch(value, 1)
#endif

#if defined(WIN32)
typedef volatile PVOID  sq_atomic;
#else
typedef volatile long 	sq_atomic;
#endif

void spinlock_lock(sq_atomic* locker, const char* prefix);
bool spinlock_trylock(sq_atomic* locker);
void spinlock_unlock(sq_atomic* locker);
//#define spinlock_trylock(locker)  (*(locker) == 0 && atomic_cmp_set(locker, 0, 1))
//#define spinlock_unlock(locker)   (*(locker) = 0)

void simplelock_lock(pthread_mutex_t* locker);
bool simplelock_trylock(pthread_mutex_t* locker);
void simplelock_unlock(pthread_mutex_t* locker);

typedef void (*runnable_t)(void*);

struct sq_thread
{
	sq_thread*					next;
	char						name[128];
	pthread_t		 			pid;
	pthread_attr_t  			attr;
	size_t 		 				stacksize;
	runnable_t	 				runnable;
	void*			 			p;
	bool			 			running;
};

sq_thread* sq_thread_create(runnable_t runnable, size_t stacksize, const char* name, void* p);

void thread_create(void* (*runnable_fun)(void*), size_t stacksize, void* p);

bool sq_thread_pool_insert(const char* name, runnable_t runnable, void* p);
void sq_thread_pool_exit();

#endif
