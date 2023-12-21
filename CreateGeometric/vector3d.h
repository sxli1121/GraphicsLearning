#pragma once
#ifndef __VECTOR_3D_H__
#define __VECTOR_3D_H__
#define _USE_MATH_DEFINES
#include <cmath>//math.h
#include <cassert>//断言头文件
#define M_PI       3.14159265358979323846   // pi
//1e-6：使用的科学计数法，也就是1x10^(-6)  0.000001
#define IS_FLOAT_ZERO(f) (abs(f)<1e-6)
#define D2R(angle_d) ((angle_d)*0.017453292519943295769)
#define R2D(angle_r) ((angle_r)*57.2957795130823208768463)

class vector3d
{
public:
	union
	{
		struct  
		{
			float x, y, z;
		};
		struct  
		{
			float r, g, b;
		};
	};

	//构造函数
	vector3d() :x(0), y(0), z(0) {}//使用初始化列表构造全部为0，即一个0向量
	vector3d(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}

	//向量数乘数除
	vector3d operator * (float v)
	{
		return vector3d(x * v, y * v, z * v);
	}
	vector3d operator * (float v) const
	{
		return vector3d(x * v, y * v, z * v);
	}
	vector3d operator / (float v)
	{
		//在debug模式下，对断言中的条件进行判断
		//如果条件为真，什么都不发生
		//如果条件为假，抛出断言异常
		assert(!IS_FLOAT_ZERO(v));
		return vector3d(x / v, y / v, z / v);
	}
	vector3d operator / (float v) const
	{
		assert(!IS_FLOAT_ZERO(v));
		return vector3d(x / v, y / v, z / v);
	}
	vector3d& operator *= (float v)
	{
		x *= v; y *= v; z *= v;
		return *this;
	}
	vector3d& operator /= (float v)
	{
		//在debug模式下，对断言中的条件进行判断
		//如果条件为真，什么都不发生
		//如果条件为假，抛出断言异常
		assert(!IS_FLOAT_ZERO(v));
		x /= v, y /= v, z /= v;
		return *this;
	}

	//向量的加减
	vector3d operator + (const vector3d& v)
	{
		return vector3d(x + v.x, y + v.y, z + v.z);
	}
	vector3d operator + (const vector3d& v) const
	{
		return vector3d(x + v.x, y + v.y, z + v.z);
	}
	vector3d operator - (const vector3d& v)
	{
		return vector3d(x - v.x, y - v.y, z - v.z);
	}
	vector3d operator - (const vector3d& v) const
	{
		return vector3d(x - v.x, y - v.y, z - v.z);
	}
	vector3d& operator += (const vector3d& v)
	{
		x += v.x; y += v.y;z += v.z;
		return *this;
	}
	vector3d& operator -= (const vector3d& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	vector3d& operator *= (const vector3d& v)
	{
		x *= v.x; y *= v.y; z *= v.z;
		return *this;
	}

	vector3d operator - ()
	{
		return vector3d(-x,-y,-z);
	}

	vector3d operator - () const
	{
		return vector3d(-x, -y, -z);
	}

	//模长
	float Length()
	{
		return sqrtf(x * x + y * y + z * z);
	}
	float Length()const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	//平方长度
	float SQLength()
	{
		return (x * x + y * y + z * z);
	}
	float SQLength()const
	{
		return (x * x + y * y + z * z);
	}

	//求当前向量的单位向量
	vector3d Normalized()
	{
		float len = sqrtf(x * x + y * y + z * z);
		return (*this) / len;
	}
	vector3d Normalized()const
	{
		float len = sqrtf(x * x + y * y + z * z);
		return (*this) / len;
	}
	//单位化当前向量
	vector3d& Normalize()
	{
		float len = sqrtf(x * x + y * y + z * z);
		return (*this) /= len;
	}
	//点乘 |A|·|B|*cos(θ)
	float Dot(const vector3d& that)
	{
		return x * that.x + y * that.y + z * that.z;
	}
	float Dot(const vector3d& that)const
	{
		return x * that.x + y * that.y + z * that.z;
	}
	//叉乘|A|·|B|*sin(θ)
	vector3d Cross(const vector3d& that)
	{
		return vector3d(
		y*that.z - z*that.y,
		z*that.x - x*that.z,
		x*that.y - y*that.x);
	}
	vector3d Cross(const vector3d& that)const
	{
		return vector3d(
			y * that.z - z * that.y,
			z * that.x - x * that.z,
			x * that.y - y * that.x);
	}
	//向量的投影
	vector3d Projection(const vector3d& n)
	{
		float fvdot = x * n.x + y * n.y + z * n.z;//v.dot(n)
		float nsqlent = (n.x * n.x + n.y * n.y + n.z * n.z);//|n|^2
		return n * fvdot / nsqlent;
	}
	vector3d Projection(const vector3d& n)const
	{
		float fvdot = x * n.x + y * n.y + z * n.z;//v.dot(n)
		float nsqlent = (n.x * n.x + n.y * n.y + n.z * n.z);//|n|^2
		return n * fvdot / nsqlent;
	}

	//求夹角
	//结果是弧度
	float Angle_r(const vector3d& n)
	{
		//两个向量模长的乘积：|A|*|B|
		float lenab = sqrtf(x * x + y * y + z * z) 
			* sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
		float fdotv = x * n.x + y * n.y + z * n.z;
		assert(!IS_FLOAT_ZERO(lenab));
		float cosvalue = fdotv / lenab;//余弦值
		return acosf(cosvalue);//调用反三角函数
	}
	float Angle_r(const vector3d& n) const
	{
		//两个向量模长的乘积：|A|*|B|
		float lenab = sqrtf(x * x + y * y + z * z)
			* sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
		float fdotv = x * n.x + y * n.y + z * n.z;
		assert(!IS_FLOAT_ZERO(lenab));
		float cosvalue = fdotv / lenab;//余弦值
		return acosf(cosvalue);//调用反三角函数
	}
	//结果是度数
	float Angle_d(const vector3d& n)
	{
		float ra = Angle_r(n);
		return R2D(ra);
	}
	float Angle_d(const vector3d& n) const
	{
		float ra = Angle_r(n);
		return R2D(ra);
	}

	static vector3d Up() { return vector3d(0,1.0f,0); }
	static vector3d Down() {return vector3d(0, -1.0f, 0);}
	static vector3d Left() {return vector3d(-1.0f, 0, 0);}
	static vector3d Right() {return  vector3d(1.0f, 0, 0);}
	static vector3d Forward() {return vector3d(0, 0, 1.0f);}
	static vector3d Back() {return vector3d(0, 0, -1.0f);}
	static vector3d Zero() {return vector3d(0, 0, 0);}
	static vector3d One() {return vector3d(1.0f, 1.0f, 1.0f);}
	static vector3d Rand()
	{
		return vector3d(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50).Normalized();
	}
	static vector3d Lerp(const vector3d& from, const vector3d& to, float t)
	{
		return from + (to - from) * t;
	}
};

#endif