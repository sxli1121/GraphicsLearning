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

	//�жϾ����Ƿ��ཻ
	bool CrossRect(const RECT& rect_other)const
	{
		return !(rect_other.left > right ||
			rect_other.right<left ||
			rect_other.top>bottom ||
			rect_other.bottom < top);
	}

	//����ཻ������
	bool CrossRect(const RECT& rect_other,
		RECT& rect_cross)const//����ཻ�õ����ཻ����
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

	union   // �����ε����������uv����
	{
		struct
		{
			vec2f uv1, uv2, uv3;
		};
		vec2f uv[3];
	};
	class MyTexture2D* ptexture; // ��������ͼ

	Triangle() :p1(vec3f::zero()), p2(vec3f::zero()), p3(vec3f::zero()) {}
	Triangle(const vec3f& _p1, const vec3f& _p2, const vec3f& _p3)
		:p1(_p1), p2(_p2), p3(_p3) {}


	// ��⿴�����������Ƿ�������
	bool CheckFront(const vec3f& dir)
	{
		vec3f normal = (p3 - p2).Cross(p1 - p2);
		return normal.Dot(dir) < 0;  // ��������������ƽ��ķ�������� <0 ��ʱ����ǿ���������
	}


	// �����εľ���任
	Triangle& Transform(const Mat4x4f& mat)
	{
		p1 = p1 * mat;
		p2 = p2 * mat;
		p3 = p3 * mat;
		return *this;
	}

};


//ƽ����
class Plane3D
{
public:
	float a, b, c, d;  // ���ߺͳ�����
	Plane3D()
	{
		vec3f t = vec3f::one();
		t.Normalize();
		a = t.x;
		b = t.y;
		c = t.z;
		d = 0;
	}

	//�㷨ʽ����ƽ��
	Plane3D(const vec3f& o, const vec3f& n)
	{
		vec3f _n = n.normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * o.x + b * o.y + c * o.z);
	}
	//����ʽ����ƽ��(���˳��涨Ϊ��������ϵ��˳ʱ���˳��)
	Plane3D(const vec3f& p1, const vec3f& p2, const vec3f& p3)
	{
		vec3f _n = (p3 - p2).Cross(p1 - p2).normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * p1.x + b * p1.y + c * p1.z);
	}
	//�㷨ʽ����ƽ��
	void CreateON(const vec3f& o, const vec3f& n)
	{
		vec3f _n = n.normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * o.x + b * o.y + c * o.z);
	}
	//����ʽ����ƽ��(���˳��涨Ϊ��������ϵ��˳ʱ���˳��)
	void Create3P(const vec3f& p1, const vec3f& p2, const vec3f& p3)
	{
		vec3f _n = (p3 - p2).Cross(p1 - p2).normalized();
		a = _n.x;
		b = _n.y;
		c = _n.z;
		d = -(a * p1.x + b * p1.y + c * p1.z);
	}

	//�㵽ƽ��ľ���
	float Distance(const vec3f& p) const
	{
		return a * p.x + b * p.y + c * p.z + d;
	}


	// �߶���ƽ���󽻵�
	bool LineCrossPlane(const vec3f& p1, const vec3f& p2,  // �߶����յ�
		vec3f& pCross,
		float* t = nullptr) const
	{
		vec3f linedelt = p2 - p1;
		// �����߶����˾����߶���ƽ��Ľ��� �� �����ֵ
		float disdiffset = a * linedelt.x + b * linedelt.y + c * linedelt.z;

		// ���� �����˵�Ĳ�ֵ �ж���ƽ��Ĺ�ϵ
		if (IS_FLOAT_ZERO(disdiffset))
		{
			return false;  // ��������0 ����ֱ��ƽ����ƽ��
		}

		// ���p1p2��ƽ������� p2 - p1 ���Ǿ���ƽ����ֵ�ϵľ���(һ����һ����)
		float d1 = a * p1.x + b * p1.y + c * p1.z + d;
		float k = -d1 / disdiffset; // ��Ϊdisdiffset ����p2-p1�õ� ���Դ˴�ʹ��-d1
		// ��ʱ���k ��0-1 ֮������������ ���<0 ��ʾ�ڱ��� >0 ��ʾ������
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


//�Ӿ���
class Frustum
{
public:
	/*
   4 ������5
   ���v   ���v
   ��  0������1
   ��  �� �� ��
   7 --��- 6 ��
	�v ��  �v��
	   3������2
	�����棺0,1,2,3
	Զ���棺4,5,6,7
	����6��������������ϵ�£�ÿ���涼���⣬ÿ������ѡ������˳ʱ�빹��ƽ��
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

	//�жϵ��Ƿ����Ӿ�����(�㵽6����ľ��붼С��0)
	bool PtInFrustum(const vec3f& p) const
	{
		return plane_near.Distance(p) < 0 &&
			plane_far.Distance(p) < 0 &&
			plane_left.Distance(p) < 0 &&
			plane_right.Distance(p) < 0 &&
			plane_up.Distance(p) < 0 &&
			plane_down.Distance(p) < 0;
	}

	//�ж��������Ƿ����Ӿ����ڲ�
	bool TriangleInFrustum(const Triangle& t) const
	{
		return PtInFrustum(t.p1) || PtInFrustum(t.p2) || PtInFrustum(t.p3);
	}
protected:
private:
};

#endif