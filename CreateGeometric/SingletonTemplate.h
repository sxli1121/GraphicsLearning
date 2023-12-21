#pragma once
#ifndef __SINGLETON_TEMPLATE_H__
#define __SINGLETON_TEMPLATE_H__

template<typename T>
class Singleton
{
public:
	static T& Instance()
	{
		static T instance;
		return instance;
	}
};

#define  PRIVATE_OPEN(t) friend class Singleton<t>;
//使用宏定义来将一些代码段进行定义
#define  PRIVATE_COPY(t)	private:\
t(const t&) = delete;\
t& operator = (const t&)=delete;\
friend class Singleton<t>;

#define PRIVATE_COPY_AND_CONSTRUCTOR(t)private:\
t(){}\
t(const t&) = delete;\
t& operator = (const t&)=delete;\
friend class Singleton<t>;
#endif