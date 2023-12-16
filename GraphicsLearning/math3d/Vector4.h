#pragma once
#ifndef __MATH_3D_VECTOR4_H__
#define __MATH_3D_VECTOR4_H__
#include <iostream>
#include <string>
#include <initializer_list>
#include "Universal.h"

namespace Math
{
	template<typename T>
	class Vector4
	{
		friend inline std::ostream& operator<<(std::ostream& o, const Vector4<T>& v);
		friend inline std::istream& operator>>(std::istream& i, Vector4<T>& v);
	public:
		T x, y, z, w;
		Vector4() :x(0), y(0), z(0), w(0) {}
		Vector4(T _x, T _y, T _z, T _w) :x(_x), y(_y), z(_y), w(_w) {}
		Vector4(std::initializer_list<T> lt)
		{
			x = 0;
			y = 0;
			z = 0;
			w = 0;
			int i = 0;
			for (auto& item : lt)
			{
				switch (i)
				{
				case 0:x = item; break;
				case 1:y = item; break;
				case 2:z = item; break;
				case 3:w = item; break;
				}
				++i;
			}
		}

		constexpr static const Vector4& one()
		{
			static Vector4<T> _one(1, 1, 1, 1);
			return _one;
		}
		constexpr static const Vector4& zero()
		{
			static Vector4<T> _zero(0, 0, 0, 0);
			return _zero;
		}

		//平方模长
		T sqrMagnitude() const
		{
			return x * x + y * y + z * z + w * w;
		}

		//模长
		T magnitude() const
		{
			return sqrt(x * x + y * y + z * z + w * w);
		}

		//取单位向量
		Vector4 normalized() const
		{
			T mag = sqrt(x * x + y * y + z * z + w * w);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				return Vector4<T>(0, 0, 0, 0);
			}
			return Vector4(x / mag, y / mag, z / mag, w/mag);
		}
		//是否零向量
		bool isZero() const
		{
			return abs(x) <= kEpsilon<T>() &&
				abs(y) <= kEpsilon<T>() &&
				abs(z) <= kEpsilon<T>() &&
				abs(w) <= kEpsilon<T>();
		}
		//单位化
		Vector4 Normalize()
		{
			T mag = sqrt(x * x + y * y + z * z + w * w);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				return Vector4<T>(0, 0, 0, 0);
			}
			x /= mag;
			y /= mag;
			z /= mag;
			w /= mag;
			return *this;
		}
		//点乘
		T Dot(const Vector4<T>& to) const
		{
			return x * to.x + y * to.y + z * to.z + w * to.w;
		}

		//求两点距离
		T Distance(const Vector4<T>& to) const
		{
			return sqrt((x - to.x) * (x - to.x)
				+ (y - to.y) * (y - to.y)
				+ (z - to.z) * (z - to.z)
				+ (w - to.w) * (w - to.w));
		}

		//两点的距离平方
		T SqrDistance(const Vector4<T>& to) const
		{
			return (x - to.x) * (x - to.x)
				+ (y - to.y) * (y - to.y)
				+ (z - to.z) * (z - to.z)
				+ (w - to.w) * (w - to.w);
		}

		//插值
		Vector4 Lerp(const Vector4<T>& to, float t) const
		{
			//from + (to - from)*t
			//from*(1-t) + to*t;
			float onemt = 1 - t;
			return Vector4<T>(x * onemt + to.x * t,
				y * onemt + to.y * t,
				z * onemt + to.z * t,
				w * onemt + to.w * t);
		}

		//向量的投影
		Vector4 Projection(const Vector4<T> n) const
		{
			//向量投影
			//https://www.zhihu.com/question/321053242?ivk_sa=1024320u

			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y + z * n.z + w * n.w;
			T smg = n.x * n.x + n.y * n.y + n.z * n.z + n.w * n.w;
			T s = dot / smg;
			return Vector4<T>(n.x * s, n.y * s, n.z * s);
		}

		//返回向量投影的单位向量方向
		Vector4 ProjectionDir(const Vector4<T> n) const
		{
			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y + z * n.z + w * n.w;
			T smg = n.x * n.x + n.y * n.y + n.z * n.z + n.w * n.w;
			T s = dot / smg;
			return Vector4<T>(n.x * s, n.y * s, n.z * s).normalized();
		}

		//取负
		Vector4 operator -() const
		{
			return Vector4(-x, -y, -z, -w);
		}

		//向量加法
		Vector4 operator + (const Vector4<T>& b) const
		{
			return Vector4<T>(x + b.x, y + b.y, z + b.z, w + b.w);
		}

		//向量减法
		Vector4 operator - (const Vector4<T>& b) const
		{
			return Vector4<T>(x - b.x, y - b.y, z - b.z, w - b.w);
		}

		//向量数乘
		Vector4 operator * (float d) const
		{
			return Vector4<T>(x * d, y * d, z * d, w * d);
		}

		//向量与向量相乘，规定x*x,y*y
		Vector4 operator *(const Vector4<T>& b) const
		{
			return Vector4<T>(x * b.x, y * b.y, z * b.z, w * b.w);
		}

		//向量数除
		Vector4 operator /(float d) const
		{
			if (abs(d) <= kEpsilon<T>())
			{
				return zero();
			}
			return Vector4<T>(x / d, y / d, z / d, w / d);
		}

		//向量加法
		Vector4<T>& operator += (const Vector4<T>& b)
		{
			x += b.x;
			y += b.y;
			z += b.z;
			w += b.w;
			return *this;
		}

		//向量减法
		Vector4<T>& operator -= (const Vector4<T>& b)
		{
			x -= b.x;
			y -= b.y;
			z -= b.z;
			w -= b.w;
			return *this;
		}

		//向量数乘
		Vector4<T>& operator *= (float d)
		{
			x *= d;
			y *= d;
			z *= d;
			w *= d;
			return *this;
		}

		//向量与向量相乘，规定x*x,y*y
		Vector4<T>& operator *=(const Vector4<T>& b)
		{
			x *= b.x;
			y *= b.y;
			z *= b.z;
			w *= b.w;
			return *this;
		}

		//向量数除
		Vector4<T>& operator /= (float d)
		{
			if (abs(d) <= kEpsilon<T>())
			{
				x = 0;
				y = 0;
				z = 0;
				w = 0;
			}
			else
			{
				x /= d;
				y /= d;
				z /= d;
				w /= d;

			}
			return *this;
		}

		//相等判断
		bool operator ==(const Vector4<T>& rhs) const
		{
			return abs(x - rhs.x) <= kEpsilon<T>() &&
				abs(y - rhs.y) <= kEpsilon<T>() &&
				abs(z - rhs.z) <= kEpsilon<T>() &&
				abs(w - rhs.w) <= kEpsilon<T>();
		}

		//不等判断
		bool operator !=(const Vector4<T>& rhs) const
		{
			return !(abs(x - rhs.x) <= kEpsilon<T>() &&
				abs(y - rhs.y) <= kEpsilon<T>() &&
				abs(z - rhs.z) <= kEpsilon<T>() &&
				abs(w - rhs.w) <= kEpsilon<T>());
		}

		bool operator < (const Vector3<T>& r) const
		{
			return ToString() < r.ToString();
		}

		std::string ToString() const
		{
			char buff[100] = {};
			sprintf_s(buff, 100, "(%f,%f,%f,%f)", x, y, z, w);
			return buff;
		}
	};

	//数左乘向量
	template<typename T>
	inline Vector4<T> operator * (float d, const Vector4<T>& v)
	{
		return Vector4<T>(v.x * d, v.y * d, v.z * d, v.w * d);
	}

	inline std::ostream& operator<<(std::ostream& o, const Vector4<float>& v)
	{
		o << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
		return o;
	}

	inline std::ostream& operator<<(std::ostream& o, const Vector4<double>& v)
	{
		o << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
		return o;
	}


	inline std::istream& operator>>(std::istream& i, Vector4<float>& v)
	{
		i >> v.x >> v.y >> v.z >> v.w;
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Vector4<double>& v)
	{
		i >> v.x >> v.y >> v.z >> v.w;
		return i;
	}

	typedef Vector4<float> vec4f;
	typedef Vector4<double> vec4d;
}
#endif
