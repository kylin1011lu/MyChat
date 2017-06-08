/*
* \file: Singleton.h
* \brief: Created by kylin
*/

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include "Definition/Define.h"
NS_MY_BEGIN

template <typename T> 
class Singleton
{
public:
	static T& Instance()
	{
		if (!instance)
		{
			setInstance(new T);
		}
		return *instance;
	}
	static T* getInstance()
	{
		if (!instance)
		{
			setInstance(new T);
		}
		return instance;
	}
	static void setInstance(T* t)
	{
		instance = t;
	}
protected:
	Singleton() {}
	~Singleton()
	{
		if (instance)
		{
			delete getInstance();
		}
	}
private:
	Singleton(const Singleton &);
	Singleton& operator=(const Singleton &);

	static T *instance;
};

template <typename T> T* Singleton<T>::instance = NULL;

NS_MY_END

#define NEW_INSTANCE(T, ...) mychat::Singleton<T>::setInstance(new T(__VA_ARGS__))
#define DELETE_INSTANCE(T)	delete mychat::Singleton<T>::getInstance()


#endif
