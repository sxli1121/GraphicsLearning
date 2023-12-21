#pragma once
#ifndef __GEOMETRIC_H__
#define __GEOMETRIC_H__
#include <windows.h>
#include "UtilityHelper.h"
#include "math3d/Matrix4x4.h"
#include "math3d/Vector2.h"
using namespace Math;

class Rect :public RECT
{
public:
	Rect() { memset(this, 0, sizeof(RECT)); }
	Rect(LONG _left, LONG _top, LONG _right, LONG _bottom)
	{
		left = _left;
		right = _right;
		top = _top;
		bottom = _bottom;
	}
	Rect& Set(LONG _left, LONG _top, LONG _right, LONG _bottom)
	{
		left = _left;
		right = _right;
		top = _top;
		bottom = _bottom;
		return *this;
	}

	int width() const { return right - left; }
	int height() const { return bottom - top; }
	POINT center() const { return { (right + left) / 2,(top + bottom) / 2 }; }

	bool PtInRect(int x, int y)const
	{
		return x >= left && x <= right && y >= top && y <= bottom;
	}

	//判断矩形是否相交
	bool CrossRect(const RECT& rect_other)const
	{
		return !(rect_other.left > right ||
			rect_other.right<left ||
			rect_other.top>bottom ||
			rect_other.bottom < top);
	}

	//求出相交的区域
	bool CrossRect(const RECT& rect_other,
		RECT& rect_cross)const//如果相交得到的相交区域
	{

		if (rect_other.left > right ||
			rect_other.right<left ||
			rect_other.top>bottom ||
			rect_other.bottom < top)
		{
			return false;
		}
		rect_cross.left = max(left, rect_other.left);
		rect_cross.right = min(right, rect_other.right);
		rect_cross.top = max(top, rect_other.top);
		rect_cross.bottom = min(bottom, rect_other.bottom);
		return true;
	}
};



class Triangle
{
public:
	union
	{
		struct
		{
			vec3f p1, p2, p3;
		};
		vec3f pts[3];
	};

	union   // 三角形的三个顶点的uv坐标
	{
		struct
		{
			vec2f uv1, uv2, uv3;
		};
		vec2f uv[3];
	};
	class MyTexture2D* ptexture; // 三角形贴图

	Triangle() :p1(vec3f::zero()), p2(vec3f::zero()), p3(vec3f::zero()) {}
	Triangle(const vec3f& _p1, const vec3f& _p2, const vec3f& _p3)
		:p1(_p1), p2(_p2), p3(_p3) {}


	// 检测看到的三角形是否是正面
	bool CheckFront(const vec3f& dir)
	{
		vec3f normal = (p3 - p2).Cross(p1 - p2);
		return normal.Dot(dir) < 0;  // 传进来的向量与平面的法向量点乘 <0 的时候才是看到的正面
	}


	// 三角形的矩阵变换
	Triangle& Transform(const Mat4x4f& mat)
	{
		p1 = p1 * mat;
		p2 = p2 * mat;
		p3 = p3 * mat;
		return *this;
	}

};


//平面类
class Plane3D
{
public:
	float a, b, c, d;  // 法线和常数项
	Plane3D()
	{
		vec3f t = vec3f::one();
		t.Normalize();
		a = t.x;
		b = t.y;
		c = t.z;
		d = 0;
	}

	//点法式构造平面
	Plane3D(const vec3f& o, const vec3f& n)
	{
		vec3f _n = n.normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * o.x + b * o.y + c * o.z);
	}
	//三点式构造平面(点的顺序规定为左手坐标系下顺时针的顺序)
	Plane3D(const vec3f& p1, const vec3f& p2, const vec3f& p3)
	{
		vec3f _n = (p3 - p2).Cross(p1 - p2).normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * p1.x + b * p1.y + c * p1.z);
	}
	//点法式构造平面
	void CreateON(const vec3f& o, const vec3f& n)
	{
		vec3f _n = n.normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * o.x + b * o.y + c * o.z);
	}
	//三点式构造平面(点的顺序规定为左手坐标系下顺时针的顺序)
	void Create3P(const vec3f& p1, const vec3f& p2, const vec3f& p3)
	{
		vec3f _n = (p3 - p2).Cross(p1 - p2).normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * p1.x + b * p1.y + c * p1.z);
	}

	//点到平面的距离
	float Distance(const vec3f& p) const
	{
		return a * p.x + b * p.y + c * p.z + d;
	}


	// 线段与平面求交点
	bool LineCrossPlane(const vec3f& p1, const vec3f& p2,  // 线段起终点
		vec3f& pCross,
		float* t = nullptr) const
	{
		vec3f linedelt = p2 - p1;
		// 计算线段两端距离线段与平面的交点 的 距离插值
		float disdiffset = a * linedelt.x + b * linedelt.y + c * linedelt.z;

		// 根据 两个端点的插值 判断与平面的关系
		if (IS_FLOAT_ZERO(disdiffset))
		{
			return false;  // 距离差等于0 代表直线平行与平面
		}

		// 如果p1p2在平面的两边 p2 - p1 就是距离平面数值上的距离(一个正一个负)
		float d1 = a * p1.x + b * p1.y + c * p1.z + d;
		float k = -d1 / disdiffset; // 因为disdiffset 是由p2-p1得到 所以此处使用-d1
		// 此时如果k 在0-1 之间代表点在两端 如果<0 表示在背面 >0 表示在正面
		pCross = p1 + linedelt * k;
		if (t != nullptr)
		{
			*t = k;
		}
		return true;
	}

protected:
private:
};


//视景体
class Frustum
{
public:
	/*
   4 ───5
   │╲   │╲
   │  0───1
   │  │ │ │
   7 --│- 6 │
	╲ │  ╲│
	   3───2
	近截面：0,1,2,3
	远截面：4,5,6,7
	假设6个面在左手坐标系下，每个面都朝外，每个面任选三个点顺时针构成平面
	plane_near		1,2,3
	plane_far		4,7,6
	plane_left		0,3,7
	plane_right		1,5,6
	plane_up		5,1,0
	plane_down		2,6,7
	*/

	Plane3D	plane_near;//	1, 2, 3
	Plane3D	plane_far;//	4, 7, 6
	Plane3D	plane_left;//	0, 3, 7
	Plane3D	plane_right;//	1, 5, 6
	Plane3D	plane_up;//		5, 1, 0
	Plane3D	plane_down;//	2, 6, 7

	//判断点是否在视景体内(点到6个面的距离都小于0)
	bool PtInFrustum(const vec3f& p) const
	{
		return plane_near.Distance(p) < 0 &&
			plane_far.Distance(p) < 0 &&
			plane_left.Distance(p) < 0 &&
			plane_right.Distance(p) < 0 &&
			plane_up.Distance(p) < 0 &&
			plane_down.Distance(p) < 0;
	}

	//判断三角形是否在视景体内部
	bool TriangleInFrustum(const Triangle& t) const
	{
		return PtInFrustum(t.p1) || PtInFrustum(t.p2) || PtInFrustum(t.p3);
	}
protected:
private:
};

#endif