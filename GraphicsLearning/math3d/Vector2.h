#pragma once
#ifndef __MATH_3D_VECTOR2_H__
#define __MATH_3D_VECTOR2_H__
#include <iostream>
#include <string>
#include <initializer_list>
#include "Universal.h"

namespace Math
{
	template<typename T>
	class Vector2
	{
		friend inline std::ostream& operator<<(std::ostream& o, const Vector2<T>& v);
		friend inline std::istream& operator>>(std::istream& i, Vector2<T>& v);

	public:
		T x, y;
		//构造
		Vector2() :x(0), y(0) {}
		Vector2(T _x, T _y) :x(_x), y(_y) {}
		//列表初始化赋值
		Vector2(std::initializer_list<T> lt)
		{
			x = 0;
			y = 0;
			int i = 0;
			for (auto& item : lt)
			{
				switch (i)
				{
				case 0:x = item; break;
				case 1:y = item; break;
				}
				++i;
			}
		}

		//方向向量
		constexpr static const Vector2& right()
		{
			static Vector2<T> _right(1, 0);
			return _right;
		}
		constexpr static const Vector2& left()
		{
			static Vector2<T> _left(-1, 0);
			return _left;
		}
		constexpr static const Vector2& up()
		{
			static Vector2<T> _up(0, 1);
			return _up;
		}
		constexpr static const Vector2& down()
		{
			static Vector2<T> _down(0, -1);
			return _down;
		}
		constexpr static const Vector2& one()
		{
			static Vector2<T> _one(1, 1);
			return _one;
		}
		constexpr static const Vector2& zero()
		{
			static Vector2<T> _zero(0, 0);
			return _zero;
		}

		//平方模长
		T sqrMagnitude() const
		{
			return x * x + y * y;
		}

		//模长
		T magnitude() const
		{
			return sqrt(x * x + y * y);
		}

		//取单位向量
		Vector2 normalized() const
		{
			T mag = sqrt(x * x + y * y);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				return Vector2<T>(0, 0);
			}
			return Vector2(x / mag, y / mag);
		}
		//是否零向量
		bool isZero() const
		{
			return abs(x) <= kEpsilon<T>() && abs(y) <= kEpsilon<T>();
		}
		//单位化
		Vector2 Normalize()
		{
			T mag = sqrt(x * x + y * y);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				return Vector2<T>(0, 0);
			}
			x /= mag;
			y /= mag;
			return *this;
		}
		//点乘
		T Dot(const Vector2<T>& to) const
		{
			return x * to.x + y * to.y;
		}
		//叉乘
		T Cross(const Vector2<T>& to) const
		{
			return x * to.y - to.x * y;
		}
		//求两点距离
		T Distance(const Vector2<T>& to) const
		{
			return sqrt((x - to.x) * (x - to.x) + (y - to.y) * (y - to.y));
		}
		//两点的距离平方
		T SqrDistance(const Vector2<T>& to) const
		{
			return (x - to.x) * (x - to.x) + (y - to.y) * (y - to.y);
		}

		//求正交向量
		Vector2 Perpendicular() const
		{
			//对于正 Y 轴向上的 2D 坐标系来说，结果始终沿逆时针方向旋转 90 度。
			if (abs(x) <= kEpsilon<T>())
			{
				return Vector2(-y, x);
			}
			return Vector2(y, -x);
		}

		//向量夹角
		T AngleRadian(const Vector2<T>& to) const
		{
			T mags = sqrt(x * x + y * y) * sqrt(to.x * to.x + to.y * to.y);
			if (mags <= kEpsilon<T>())
			{
				return 0;
			}
			//dot
			T dot = x * to.x + y * to.y;
			return acos(dot / mags);
		}
		T AngleDegree(const Vector2<T>& to) const
		{
			T mags = sqrt(x * x + y * y) * sqrt(to.x * to.x + to.y * to.y);
			if (mags <= kEpsilon<T>())
			{
				return 0;
			}
			//dot
			T dot = x * to.x + y * to.y;
			return static_cast<T>(R2D(acos(dot / mags)));
		}

		//带符号夹角
		T SignedAngleRadian(const Vector2<T>& to) const
		{
			/*
			* 0~360度角度正弦和余弦的符号关系
			*            0~90	90~180  180~270  270~360
			点乘得到 cos  +       -        -        +
			叉乘得到 sin  +       +        -        -
			*/
			//dot
			//T dot = x * to.x + y * to.y;
			//cross
			T cross = x * to.y - to.x * y;

			T angle = AngleRadian(to);
			if (cross < 0)
			{
				angle = -angle;
			}
			return angle;
		}
		T SignedAngleDegree(const Vector2<T>& to) const
		{
			/*
			* 0~360度角度正弦和余弦的符号关系
			*            0~90	90~180  180~270  270~360
			点乘得到 cos  +       -        -        +
			叉乘得到 sin  +       +        -        -
			*/
			//dot
			//T dot = x * to.x + y * to.y;
			//cross
			T cross = x * to.y - to.x * y;

			T angle = AngleDegree(to);
			if (cross < 0)
			{
				angle = -angle;
			}
			return angle;
		}
		//插值
		Vector2 Lerp(const Vector2<T>& to, float t)
		{
			//from + (to - from)*t
			//from*(1-t) + to*t;
			float onemt = 1 - t;
			return Vector2<T>(x * onemt + to.x * t, y * onemt + to.y * t);
		}

		//向量的投影
		Vector2 Projection(const Vector2<T> n) const
		{
			//向量投影
			//https://www.zhihu.com/question/321053242?ivk_sa=1024320u

			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y;
			T smg = n.x * n.x + n.y * n.y;
			T s = dot / smg;
			return Vector2<T>(n.x * s, n.y * s);
		}

		//返回向量投影的单位向量方向
		Vector2 ProjectionDir(const Vector2<T> n) const
		{
			//向量投影
			//https://www.zhihu.com/question/321053242?ivk_sa=1024320u

			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y;
			T smg = n.x * n.x + n.y * n.y;
			T s = dot / smg;
			return Vector2<T>(n.x * s, n.y * s).normalized();
		}

		//计算反射向量
		Vector2 Reflect(const Vector2<T>& normal) const
		{
			if (isZero() || normal.isZero()) { return Vector2<T>(0, 0); }
			Vector2 n = normal.Perpendicular();
			Vector2 pt = Projection(n) * 2;
			return pt - *this;
		}

		//计算反射向量方向
		Vector2 ReflectDir(const Vector2<T>& normal) const
		{
			if (isZero() || normal.isZero()) { return Vector2<T>(0, 0); }
			Vector2 n = normal.Perpendicular();
			Vector2 pt = Projection(n) * 2;
			return (pt - *this).normalized();
		}

		//取负
		Vector2 operator -() const
		{
			return Vector2(-x, -y);
		}

		//向量加法
		Vector2 operator + (const Vector2<T>& b) const
		{
			return Vector2<T>(x + b.x, y + b.y);
		}

		//向量减法
		Vector2 operator - (const Vector2<T>& b) const
		{
			return Vector2<T>(x - b.x, y - b.y);
		}

		//向量数乘
		Vector2 operator * (float d) const
		{
			return Vector2<T>(x * d, y * d);
		}

		//向量与向量相乘，规定x*x,y*y
		Vector2 operator *(const Vector2<T>& b) const
		{
			return Vector2<T>(x * b.x, y * b.y);
		}

		//向量数除
		Vector2 operator /(float d) const
		{
			if (abs(d) <= kEpsilon<T>())
			{
				return zero();
			}
			return Vector2<T>(x / d, y / d);
		}

		//向量
		Vector2 operator /(const Vector2<T>& b) const
		{
			if (abs(b.x) <= kEpsilon<T>() || abs(b.y) <= kEpsilon<T>())
			{
				return zero();
			}
			return Vector2<T>(x / b.x, y / b.y);
		}

		//向量加法
		Vector2<T>& operator += (const Vector2<T>& b)
		{
			x += b.x;
			y += b.y;
			return *this;
		}

		//向量减法
		Vector2<T>& operator -= (const Vector2<T>& b)
		{
			x -= b.x;
			y -= b.y;
			return *this;
		}
		Vector2<T>& operator *= (float d)
		{
			x *= d;
			y *= d;
			return *this;
		}

		//向量与向量相乘，规定x*x,y*y
		Vector2<T>& operator *= (const Vector2<T>& b)
		{
			x *= b.x;
			y *= b.y;
			return *this;
		}

		//向量数除
		Vector2<T>& operator /= (float d)
		{
			if (abs(d) <= kEpsilon<T>())
			{
				x = 0; y = 0;
			}
			else
			{
				x /= d;
				y /= d;
			}
			return *this;
		}

		//向量
		Vector2<T>& operator /= (const Vector2<T>& b)
		{
			if (abs(b.x) <= kEpsilon<T>() || abs(b.y) <= kEpsilon<T>())
			{
				x = 0; y = 0;
			}
			else
			{
				x /= b.x;
				y /= b.y;
			}
			return *this;
		}

		//相等判断
		bool operator ==(const Vector2<T>& rhs) const
		{
			return abs(x - rhs.x) <= kEpsilon<T>() && abs(y - rhs.y) <= kEpsilon<T>();
		}

		//不等判断
		bool operator !=(const Vector2<T>& rhs) const
		{
			return !(abs(x - rhs.x) <= kEpsilon<T>() && abs(y - rhs.y) <= kEpsilon<T>());
		}

		bool operator < (const Vector2<T>& r) const
		{
			return ToString() < r.ToString();
		}

		std::string ToString() const
		{
			char buff[100] = {};
			sprintf_s(buff, 100, "(%f,%f)", x, y);
			return buff;
		}
	};

	//数左乘向量
	template<typename T>
	inline Vector2<T> operator * (float d, const Vector2<T>& v)
	{
		return Vector2<T>(v.x * d, v.y * d);
	}

	inline std::ostream& operator<<(std::ostream& o, const Vector2<float>& v)
	{
		o << "(" << v.x << "," << v.y << ")";
		return o;
	}

	inline std::ostream& operator<<(std::ostream& o, const Vector2<double>& v)
	{
		o << "(" << v.x << "," << v.y << ")";
		return o;
	}


	inline std::istream& operator>>(std::istream& i, Vector2<float>& v)
	{
		i >> v.x >> v.y;
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Vector2<double>& v)
	{
		i >> v.x >> v.y;
		return i;
	}

	typedef Vector2<float> vec2f;
	typedef Vector2<double> vec2d;
}
#endif
