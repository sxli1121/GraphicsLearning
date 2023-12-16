#pragma once
#ifndef __MY_SINGLETON_TEMPLATE_H__
#define __MY_SINGLETON_TEMPLATE_H__


template<typename T>
class Singleton
{
protected:
	Singleton() = default;
public:
	static T& Instance()
	{
		static T instance;//调用的是T类型的构造函数，T类型的构造函数又是私有的，所以通过友元的方式达成调用
		//对于T类型来讲，除了T类型自己以外只有Singleton<T> 类型可以访问T的构造函数
		return instance;
	}
};

#define  PRIVATE_OPEN(t) friend class Singleton<t>;
//使用宏定义来实现T类型的单例特性
//屏蔽构造、拷贝、同类赋值等函数

#define PRIVATE_CONSTRUCTOR_AND_COPY(t) \
private:\
	t(const t&)=delete;\
	t& operator = (const t&)=delete;\
	t() = default;\
	friend class Singleton<t>;


#define PRIVATE_COPY(t) \
private:\
	t(const t&)=delete;\
	t& operator = (const t&)=delete;\
	friend class Singleton<t>;


#endif