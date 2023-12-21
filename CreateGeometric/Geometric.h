#pragma once
#ifndef __GEOMETRIC_H__
#define __GEOMETRIC_H__
#include <windows.h>
#include "Matrix4x4.h"
class Rect : public RECT
{
public:
	Rect() { memset(this, 0, sizeof(RECT)); }
	Rect(LONG _left, LONG _top, LONG _right, LONG _bottom)
	{
		left = _left;
		top = _top;
		right = _right;
		bottom = _bottom;
	}
	void Set (LONG _left, LONG _top, LONG _right, LONG _bottom)
	{
		left = _left;
		top = _top;
		right = _right;
		bottom = _bottom;
	}

	int Width() { return right - left; }
	int Height() { return  bottom - top; }
	POINT center() { return {(left+right)/2,(top+bottom)/2};}
	int Width()const { return right - left; }
	int Height() const{ return  bottom - top; }
	POINT center() const{ return { (left + right) / 2,(top + bottom) / 2 }; }
	bool PtInRect(int x, int y)
	{
		return x >= left && x <= right && y >= top && y <= bottom;
	}
	bool PtInRect(int x, int y) const
	{
		return x >= left && x <= right && y >= top && y <= bottom;
	}

	//判断矩形是否相交
	bool CrossRect(const RECT& rect_other)
	{
		return !(rect_other.left > right ||
			rect_other.right< left ||
			rect_other.top> bottom ||
			rect_other.bottom < top);
	}
	bool CrossRect(const RECT& rect_other) const
	{
		return !(rect_other.left > right ||
			rect_other.right< left ||
			rect_other.top> bottom ||
			rect_other.bottom < top);
	}

	//求相交区域
	bool CrossRect(const RECT& rect_other,/*要求相交区域的另一个矩形*/ 
		RECT& rect_cross/*如果相交，得到的相交区域*/)
	{
		if (rect_other.left > right ||
			rect_other.right< left ||
			rect_other.top> bottom ||
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

	bool CrossRect(const RECT& rect_other,/*要求相交区域的另一个矩形*/
		RECT& rect_cross/*如果相交，得到的相交区域*/)const
	{
		if (rect_other.left > right ||
			rect_other.right< left ||
			rect_other.top> bottom ||
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
			vector3d p1, p2, p3;
		};
		vector3d pts[3];
	};
	Triangle() :p1(vector3d::Zero()), p2(vector3d::Zero()), p3(vector3d::Zero()) {}
	Triangle(const vector3d& _p1, const vector3d& _p2, const vector3d& _p3)
		:p1(_p1), p2(_p2), p3(_p3) {}

	//检测三角形是否正面朝向
	bool CheckFront(const vector3d& dir)
	{
		vector3d n = (p3 - p2).Cross(p1 - p2);
		return n.Dot(dir) < 0;
	}

	//三角形矩阵变换
	Triangle& Transform(const Matrix4x4& mat)
	{
		p1 = p1 * mat;
		p2 = p2 * mat;
		p3 = p3 * mat;
		return *this;
	}

	//三角形面积
	float Area()
	{
		vector3d a = p2 - p1;
		vector3d b = p3 - p1;
		return a.Cross(b).Length() * 0.5f;
	}
};


//平面类
class Plane3D
{
public:
	float a, b, c, d;
	Plane3D()
	{
		vector3d t = vector3d::One();
		t.Normalize();
		a = t.x;
		b = t.y;
		c = t.z;
		d = 0;
	}

	//点法式构造平面
	Plane3D(const vector3d& o, const vector3d& n)
	{
		vector3d _n = n.Normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * o.x + b * o.y + c * o.z);
	}
	//三点式构造平面(点的顺序规定为左手坐标系下顺时针的顺序)
	Plane3D(const vector3d& p1, const vector3d& p2, const vector3d& p3)
	{
		vector3d _n = (p3 - p2).Cross(p1 - p2).Normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * p1.x + b * p1.y + c * p1.z);
	}
	//点法式构造平面
	void CreateON(const vector3d& o, const vector3d& n)
	{
		vector3d _n = n.Normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * o.x + b * o.y + c * o.z);
	}
	//三点式构造平面(点的顺序规定为左手坐标系下顺时针的顺序)
	void Create3P(const vector3d& p1, const vector3d& p2, const vector3d& p3)
	{
		vector3d _n = (p3 - p2).Cross(p1 - p2).Normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * p1.x + b * p1.y + c * p1.z);
	}

	//点到平面的距离
	float Distance(const vector3d& p)
	{
		return a * p.x + b * p.y + c * p.z + d;
	}

	//线段和平面求交点
	bool LineCross(const vector3d& p1, const vector3d& p2,//线段的两端
		vector3d& pCross,//求到的交点
		float* t = nullptr)//P1P2交点处的比例值
	{
		vector3d linedelt = p2 - p1;
		//p1到平面的距离：d1 = a*p1.x+b*p1.y+c*p1.z+d
		//p2到平面的距离：d2 = a*p2.x+b*p2.y+c*p2.z+d
		//距离差 = d2 - d1=a*(p2.x-p1.x)+b*(p2.y-p1.y)+c*(p2.z-p1.z)	
		float dist = a * linedelt.x + b * linedelt.y + c * linedelt.z;

		if (IS_FLOAT_ZERO(dist))//直线平行平面或者直线就在平面上
		{
			return false;//不相交，返回假
		}

		float d1 = a * p1.x + b * p1.y + c * p1.z + d;
		float k = -d1 / dist;
		pCross = p1 + linedelt * k;//得到交点
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
	Plane3D	plane_up;//	5, 1, 0
	Plane3D	plane_down;//	2, 6, 7

	//判断点是否在视景体内(点到6个面的距离都小于0)
	bool PtInFrustum(const vector3d& p)
	{
		return plane_near.Distance(p) < 0 &&
			plane_far.Distance(p) < 0 &&
			plane_left.Distance(p) < 0 &&
			plane_right.Distance(p) < 0 &&
			plane_up.Distance(p) < 0 &&
			plane_down.Distance(p) < 0;
	}
protected:
private:
};
#endif