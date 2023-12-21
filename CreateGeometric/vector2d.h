#pragma once
#ifndef __VECTOR_2D_H__
#define __VECTOR_2D_H__
#define _USE_MATH_DEFINES
#include <cmath>//math.h
#include <cassert>//断言头文件
#define M_PI       3.14159265358979323846   // pi
//1e-6：使用的科学计数法，也就是1x10^(-6)  0.000001
#define IS_FLOAT_ZERO(f) (abs(f)<1e-6)

class vector2d
{
public:
	float x, y;
	//构造
	vector2d() :x(0), y(0) {}//使用初始化列表全部构造为0，也即是一个0向量
	vector2d(float _x,float _y) :x(_x), y(_y) {}
	vector2d(int _x, int _y) :x((float)_x), y((float)_y) {}
	vector2d(int _x, float _y) :x((float)_x), y(_y) {}
	vector2d(float _x, int _y) :x(_x), y((float)_y) {}

	//向量乘除（和数字）
	vector2d operator * (float r)
	{
		return vector2d(x*r, y*r);
	}
	vector2d operator / (float r)
	{
		//断言检查：在debug的模式下，对断言中的条件进行判断
		//如果条件为真，什么都不发生
		//如果条件为加，抛出断言异常
		assert(!IS_FLOAT_ZERO(r));
		return vector2d(x/r, y/r);
	}
	vector2d operator * (float r) const
	{
		return vector2d(x*r, y*r);
	}
	vector2d operator / (float r) const
	{
		//断言检查：在debug的模式下，对断言中的条件进行判断
		//如果条件为真，什么都不发生
		//如果条件为加，抛出断言异常
		assert(!IS_FLOAT_ZERO(r));
		return vector2d(x / r, y / r);
	}
	vector2d& operator *= (float r)
	{
		x *= r; y *= r;
		return *this;
	}
	vector2d& operator /= (float r)
	{
		//断言检查：在debug的模式下，对断言中的条件进行判断
		//如果条件为真，什么都不发生
		//如果条件为加，抛出断言异常
		assert(!IS_FLOAT_ZERO(r));
		x /= r; y /= r;
		return *this;
	}
	vector2d operator - ()
	{
		return vector2d(-x, -y);
	}
	vector2d operator - ()const
	{
		return vector2d(-x, -y);
	}
	//模长
	float Length()
	{
		return sqrtf(x*x+y*y);
	}
	//平方长度
	float SQLength()
	{
		return x * x + y * y;
	}
	float Length() const
	{
		return sqrtf(x*x + y * y);
	}
	//平方长度
	float SQLength() const
	{
		return x * x + y * y;
	}
	//求当前向量的单位向量
	vector2d Normalized()
	{
		float len = Length();
		return (*this) / len;
	}
	vector2d Normalized()const
	{
		float len = Length();
		return (*this) / len;
	}
	//将向量本身单位化
	vector2d Normalize()
	{
		float len = Length();
		return (*this) /= len;
	}
	//向量加减点乘叉乘（和向量）
	vector2d operator + (const vector2d& that)
	{
		return vector2d(x + that.x, y + that.y);
	}
	vector2d operator - (const vector2d& that)
	{
		return vector2d(x - that.x, y - that.y);
	}
	vector2d operator + (const vector2d& that) const
	{
		return vector2d(x + that.x, y + that.y);
	}
	vector2d operator - (const vector2d& that) const
	{
		return vector2d(x - that.x, y - that.y);
	}
	vector2d& operator += (const vector2d& that)
	{
		x += that.x;
		y += that.y;
		return *this;
	}
	vector2d& operator -= (const vector2d& that)
	{
		x -= that.x;
		y -= that.y;
		return *this;
	}
	float Dot(const vector2d& that)
	{
		return x * that.x + y * that.y;
	}
	float Dot(const vector2d& that)const
	{
		return x * that.x + y * that.y;
	}
	float Cross(const vector2d& that)
	{
		return x * that.y - that.x*y;
	}
	float Cross(const vector2d& that)const
	{
		return x * that.y - that.x * y;
	}
	//投影
	vector2d Projection(const vector2d& n)
	{
		float dotvn = x * n.x + y * n.y;
		float sqlen = n.x*n.x + n.y*n.y;
		return n * dotvn / sqlen;
	}
	vector2d Projection(const vector2d& n)const
	{
		float dotvn = x * n.x + y * n.y;
		float sqlen = n.x * n.x + n.y * n.y;
		return n * dotvn / sqlen;
	}
	//求夹角
	//结果是弧度
	float Angle_r(const vector2d& that)
	{
		float lenab = this->Length() * that.Length();
		float dotab = this->Dot(that);
		assert(!IS_FLOAT_ZERO(lenab));
		float cosvalue = dotab / lenab;
		return acosf(cosvalue);
	}
	float Angle_r(const vector2d& that)const
	{
		float lenab = this->Length() * that.Length();
		float dotab = this->Dot(that);
		assert(!IS_FLOAT_ZERO(lenab));
		float cosvalue = dotab / lenab;
		return acosf(cosvalue);
	}
	//结果是度数
	float Angle_d(const vector2d& that)
	{
		float ra = Angle_r(that);
		return float(ra / M_PI)*180.0f;
	}
	float Angle_d(const vector2d& that) const
	{
		float ra = Angle_r(that);
		return float(ra / M_PI) * 180.0f;
	}
	static vector2d Up(){return {0,1.0f};}
	static vector2d Down(){return {0,-1.0f};}
	static vector2d Left(){return {-1.0f,0};}
	static vector2d Right(){return {1.0f,0};}
	static vector2d Zero(){return {0,0};}
	static vector2d Rand() {
		return vector2d(rand() % 100 - 50, rand() % 100 - 50).Normalized();
	}
	static vector2d Lerp(const vector2d& from, const vector2d& to,float t)
	{
		return from + (to - from) * t;
	}
};
#endif
