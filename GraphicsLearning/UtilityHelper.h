#pragma once
#ifndef __UTILITY_HELPER_H__
#define __UTILITY_HELPER_H__

#define  SAFE_DELETE_ARRAY_PTR(t) if(t!=nullptr){delete [] t;t=nullptr;}
#define  CLIMP_2_ONE(t)	((t)<0?0:(t)>1?1:(t))
#include <iostream>
#include "./math3d/Vector3.h"

using namespace Math;

//1e-6��ʹ�õĿ�ѧ��������Ҳ����1x10^(-6)  0.000001
#define IS_FLOAT_ZERO(f) (abs(f)<1e-6)
//һ���Կ��Ի��Ƶ��������������ֵ
#define MAX_TRANGLE_NUM (1000000)

//��UV��װ���䵽0-1��Χ
#define  UVREPEAT(t) ((t)-floor(t))

namespace Utility
{
	//�����32λ����ɫ���� 
	 /*
	windows�ж����RGB��
	#define RGB(r,g,b)

	((COLORREF)(((BYTE)(r)|
	((WORD)((BYTE)(g))<<8))|
	(((DWORD)(BYTE)(b))<<16)))
	*/
	//�����32λ����ɫ����

	// ��һ��int ������ɫ ������ 00 b g r ------ �����ڴ�ĸ�λ�ʹ� ��� r g b 00


	typedef struct stColor32
	{
		union
		{
			unsigned char cc[4];
			int color32;
			struct
			{
				unsigned char b, g, r, a;
			};
		};

		stColor32() :color32(0) {}//��Ա��ʼ���б�
		//����ʵ�������ĸ�ֵ
		stColor32(const int& that) :color32(that) {
			r = 0x000000ff & that;
			g = (0x0000ff00 & that) >> 8;
			b = (0x00ff0000 & that) >> 16;
			a = 255;
		}
		//����ǿ������ת�������������ǵ���ɫת������Ӧ����������
		operator int() {
			unsigned char color[4] = { r,g,b,0 };
			return *((int*)color);
		}
		operator int() const {
			unsigned char color[4] = { r,g,b,0 };
			return *((int*)color);
		}

		//��Color32ת��Ϊvec3f
		operator vec3f()
		{
			return vec3f(r / 255.0f, g / 255.0f, b / 255.0f);
		}
		operator vec3f() const
		{
			return vec3f(r / 255.0f, g / 255.0f, b / 255.0f);
		}

		//��vec3fֱ�Ӹ�ֵ������
		stColor32(const vec3f& v)
		{
			r = (unsigned char)(CLIMP_2_ONE(v.x) * 255);
			g = (unsigned char)(CLIMP_2_ONE(v.y) * 255);
			b = (unsigned char)(CLIMP_2_ONE(v.z) * 255);
			a = 255;
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

}
#endif