#pragma once
#ifndef __COMMON_DEFINE_H__
#define __COMMON_DEFINE_H__
#include <iostream>
#include "vector3d.h"
#include "Geometric.h"

#define SAFE_DELETE_ARRAY_PTR(t) if(t!=nullptr){delete [] t;t=nullptr;}
#define  CLIMP_2_ONE(t) ((t)<0?0:(t)>1?1:(t))

//一次性可以绘制的三角形数量最大值
#define MAX_TRANGLE_NUM (1000000)

//定义的32位的颜色数据
typedef struct stColor32
{
	union
	{
		unsigned char cc[4];
		int color32;
		struct
		{
			unsigned char b, g, r,a;
		};
	};

//#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

	stColor32() :color32(0) {}//成员初始化列表
	//可以实现整数的赋值
	stColor32(const int& that) 
	{
		r = 0x000000ff & that;
		g = ((0x0000ff00 & that) >> 8);
		b = ((0x00ff0000 & that) >> 16);
		a = 255;
	}
	//重载强制类型转换，可以让我们的颜色转换到对应的其他类型
	operator int() //让我们的颜色可以转换到int
	{ 
		unsigned char color[4] = { r,g,b,0 };//相当于调用RGB(r,g,b)
		return *((int*)color);
	}
	operator int() const
	{
		unsigned char color[4] = { r,g,b,0 };
		return *((int*)color);
	}

	//让向量转换为一个COLOR32，通过构造实现
	stColor32(const vector3d& v)
	{
		r = (unsigned char)(CLIMP_2_ONE(v.x) * 255);
		g = (unsigned char)(CLIMP_2_ONE(v.y) * 255);
		b = (unsigned char)(CLIMP_2_ONE(v.z) * 255);
		a = 255;
	}
	//使用强制类型转换重载
	operator vector3d() //让我们的颜色可以转换到vector3d,通过类型转换重载实现
	{
		return vector3d(r / 255.0f, g / 255.0f, b / 255.0f);
	}
	operator vector3d() const
	{
		return vector3d(r / 255.0f, g / 255.0f, b / 255.0f);
	}

	stColor32(int _a, int _r, int _g, int _b)
	{
		a = (unsigned char)_a;
		r = (unsigned char)_r;
		g = (unsigned char)_g;
		b = (unsigned char)_b;
	}

	stColor32(int _r, int _g, int _b)
	{
		a = 255;
		r = (unsigned char)_r;
		g = (unsigned char)_g;
		b = (unsigned char)_b;
	}

	void Set(int _a, int _r, int _g, int _b)
	{
		a = (unsigned char)_a;
		r = (unsigned char)_r;
		g = (unsigned char)_g;
		b = (unsigned char)_b;
	}

	void Set(int _r, int _g, int _b)
	{
		a = 255;
		r = (unsigned char)_r;
		g = (unsigned char)_g;
		b = (unsigned char)_b;
	}

	//取反色
	stColor32 InvColor()
	{
		return stColor32(255 - r, 255 - g, 255 - b);
	}

	stColor32 InvColor() const
	{
		return stColor32(255 - r, 255 - g, 255 - b);
	}


	static stColor32 Random()
	{
		static stColor32 color;
		color.Set(rand() % 256, rand() % 256, rand() % 256);
		return color;
	}

	static stColor32 Black() { return 0x00000000; }
	static stColor32 White() { return 0x00ffffff; }
	static stColor32 Gray() { return 0x007f7f7f; }
	static stColor32 Blue() { return 0x00ff0000; }
	static stColor32 Green() { return 0x0000ff00; }
	static stColor32 Red() { return 0x000000ff; }
	static stColor32 Yellow() { return 0x0000ffff; }
	static stColor32 Purple() { return 0x00ff00ff; }
	static stColor32 Cyan() { return 0x00ffff00; }
	static stColor32 Pink() { return 0x00c1b6ff; }
	static stColor32 Gold() { return 0x0000d7ff; }
}COLOR32, * LPCOLOR32, ** LPPCOLOR32;
#endif