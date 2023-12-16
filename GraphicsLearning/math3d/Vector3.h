#pragma once
#ifndef __MATH_3D_VECTOR3_H__
#define __MATH_3D_VECTOR3_H__
#include <iostream>
#include <string>
#include <initializer_list>
#include "Universal.h"

namespace Math
{
	template<typename T>
	class Vector3
	{
		friend inline std::ostream& operator<<(std::ostream& o, const Vector3<T>& v);
		friend inline std::istream& operator>>(std::istream& i, Vector3<T>& v);
	public:
		T x, y, z;
		Vector3() :x(0), y(0), z(0) {}
		Vector3(T _x, T _y, T _z) :x(_x), y(_y), z(_z) {}
		Vector3(std::initializer_list<T> lt)
		{
			x = 0;
			y = 0;
			z = 0;
			int i = 0;
			for (auto& item : lt)
			{
				switch (i)
				{
				case 0:x = item; break;
				case 1:y = item; break;
				case 2:z = item; break;
				}
				++i;
			}
		}

		//方向向量
		constexpr static const Vector3& right()
		{
			static Vector3<T> _right(1, 0, 0);
			return _right;
		}
		constexpr static const Vector3& left()
		{
			static Vector3<T> _left(-1, 0, 0);
			return _left;
		}
		constexpr static const Vector3& up()
		{
			static Vector3<T> _up(0, 1, 0);
			return _up;
		}
		constexpr static const Vector3& down()
		{
			static Vector3<T> _down(0, -1, 0);
			return _down;
		}
		constexpr static const Vector3& forward()
		{
			static Vector3<T> _forward(0, 0, 1);
			return _forward;
		}
		constexpr static const Vector3& back()
		{
			static Vector3<T> _back(0, 0, -1);
			return _back;
		}
		constexpr static const Vector3& one()
		{
			static Vector3<T> _one(1, 1, 1);
			return _one;
		}
		constexpr static const Vector3& zero()
		{
			static Vector3<T> _zero(0, 0, 0);
			return _zero;
		}

		//平方模长
		T sqrMagnitude() const
		{
			return x * x + y * y + z * z;
		}

		//模长
		T magnitude() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		//取单位向量
		Vector3 normalized() const
		{
			T mag = sqrt(x * x + y * y + z * z);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				return Vector3<T>(0, 0, 0);
			}
			return Vector3(x / mag, y / mag, z / mag);
		}
		//是否零向量
		bool isZero() const
		{
			return abs(x) <= kEpsilon<T>() && abs(y) <= kEpsilon<T>() && abs(z) <= kEpsilon<T>();
		}
		//单位化
		Vector3 Normalize()
		{
			T mag = sqrt(x * x + y * y + z * z);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				return Vector3<T>(0, 0, 0);
			}
			x /= mag;
			y /= mag;
			z /= mag;
			return *this;
		}
		//点乘
		T Dot(const Vector3<T>& to) const
		{
			return x * to.x + y * to.y + z * to.z;
		}
		//叉乘
		Vector3<T> Cross(const Vector3<T>& to) const
		{
			return Vector3<T>(y * to.z - z * to.y, z * to.x - x * to.z, x * to.y - y * to.x);
		}
		//求两点距离
		T Distance(const Vector3<T>& to) const
		{
			return sqrt((x - to.x) * (x - to.x) + (y - to.y) * (y - to.y) + (z - to.z) * (z - to.z));
		}
		//两点的距离平方
		T SqrDistance(const Vector3<T>& to) const
		{
			return (x - to.x) * (x - to.x) + (y - to.y) * (y - to.y) + (z - to.z) * (z - to.z);
		}

		//向量夹角
		T AngleRadian(const Vector3<T>& to) const
		{
			//double radian_angle = Math.Atan2(Cross(from, to).magnitude, Dot(from, to));
			//如果需要角度在0到360之间就取下下面注释。
			//if (Cross(from, to).z < 0)//{//    radian_angle = 2 * 3.1415926535f - radian_angle;//}
			//0到180度return (float)(radian_angle * 180 / 3.1415926535f);


			T mags = sqrt(x * x + y * y + z * z) * sqrt(to.x * to.x + to.y * to.y + to.z * to.z);
			if (mags <= kEpsilon<T>())
			{
				return 0;
			}
			//dot
			T dot = x * to.x + y * to.y + z * to.z;
			return acos(dot / mags);
		}
		T AngleDegree(const Vector3<T>& to) const
		{
			T mags = sqrt(x * x + y * y + z * z) * sqrt(to.x * to.x + to.y * to.y + to.z * to.z);
			if (mags <= kEpsilon<T>())
			{
				return 0;
			}
			//dot
			T dot = x * to.x + y * to.y + z * to.z;
			return static_cast<T>(R2D(acos(dot / mags)));
		}

		//向量夹角
		T SignedAngleRadian(const Vector3<T>& to) const
		{
			T angle = AngleRadian(to);
			if (Cross(to).y < 0)
			{
				angle = -angle;
			}
			return angle;
		}
		T SignedAngleDegree(const Vector3<T>& to) const
		{
			T angle = AngleDegree(to);
			if (Cross(to).y < 0)
			{
				angle = -angle;
			}
			return angle;
		}
		T SignedAngleRadian(const Vector3<T>& to, const Vector3<T>& axis) const
		{
			//得到两个向量在axis上投影的正交向量
			Vector3<T> v1 = *this - Projection(axis);
			Vector3<T> v2 = to - to.Projection(axis);
			return v1.SignedAngleRadian(v2);
		}
		T SignedAngleDegree(const Vector3<T>& to, const Vector3<T>& axis) const
		{
			//得到两个向量在axis上投影的正交向量
			Vector3<T> v1 = *this - Projection(axis);
			Vector3<T> v2 = to - to.Projection(axis);
			return v1.SignedAngleDegree(v2);
		}

		//Lerp、SLerp、NLerp
		//https://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/
		Vector3 SLerp(const Vector3<T>& to, float t) const
		{
			// Dot product - the cosine of the angle between 2 vectors.
			float dot = Dot(to);

			// Clamp it to be in the range of Acos()
			// This may be unnecessary, but floating point
			// precision can be a fickle mistress.
			//Mathf.Clamp(dot, -1.0f, 1.0f);
			dot = dot < -1.0f ? -1.0f : dot>1.0f ? 1.0f : dot;
			// Acos(dot) returns the angle between start and end,
			// And multiplying that by percent returns the angle between
			// start and the final result.
			float theta = acos(dot) * t;
			Vector3 RelativeVec = to - (*this) * dot;
			RelativeVec.Normalize();

			// Orthonormal basis
			// The final result.
			return (((*this) * cos(theta)) + (RelativeVec * sin(theta)));
		}
		//插值
		Vector3 Lerp(const Vector3<T>& to, float t) const
		{
			//from + (to - from)*t
			//from*(1-t) + to*t;
			float onemt = 1 - t;
			return Vector3<T>(x * onemt + to.x * t, y * onemt + to.y * t, z * onemt + to.z * t);
		}

		Vector3 NLerp(const Vector3<T>& to, float t) const
		{
			return Lerp(to, t).normalized();
		}

		//向量的投影
		Vector3 Projection(const Vector3<T> n) const
		{
			//向量投影
			//https://www.zhihu.com/question/321053242?ivk_sa=1024320u

			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y + z * n.z;
			T smg = n.x * n.x + n.y * n.y + n.z * n.z;
			T s = dot / smg;
			return Vector3<T>(n.x * s, n.y * s, n.z * s);
		}

		//返回向量投影的单位向量方向
		Vector3 ProjectionDir(const Vector3<T> n) const
		{
			if (n.isZero()) { return zero(); }
			T dot = x * n.x + y * n.y + z * n.z;
			T smg = n.x * n.x + n.y * n.y + n.z * n.z;
			T s = dot / smg;
			return Vector3<T>(n.x * s, n.y * s, n.z * s).normalized();
		}

		//投影到平面上，这里都认为是过原点的平面，参数是平面的法线
		Vector3 ProjectOnPlane(Vector3 planeNormal) const
		{
			if (isZero() || planeNormal.isZero()) { return Vector3<T>(0, 0, 0); }
			Vector3 nz = Cross(planeNormal);
			Vector3 n = planeNormal.Cross(nz);
			return Projection(n);
		}

		//投影到XZ平面
		Vector3 ProjectOnXZ() const
		{
			return Vector3<T>(x, 0, z);
		}
		//投影到YZ平面
		Vector3 ProjectOnYZ() const
		{
			return Vector3<T>(0, y, z);
		}
		//投影到XY平面
		Vector3 ProjectOnXY() const
		{
			return Vector3<T>(x, y, 0);
		}

		//计算反射向量
		Vector3 Reflect(const Vector3<T>& normal) const
		{
			if (isZero() || normal.isZero()) { return Vector3<T>(0, 0, 0); }
			Vector3 nz = Cross(normal);
			Vector3 n = normal.Cross(nz);
			Vector3 pt = Projection(n) * 2;
			return pt - *this;
		}

		//计算反射向量方向
		Vector3 ReflectDir(const Vector3<T>& normal) const
		{
			if (isZero() || normal.isZero()) { return Vector3<T>(0, 0, 0); }
			Vector3 nz = Cross(normal);
			Vector3 n = normal.Cross(nz);
			Vector3 pt = Projection(n) * 2;
			return (pt - *this).normalized();
		}

		//取负
		Vector3 operator -() const
		{
			return Vector3(-x, -y, -z);
		}

		//向量加法
		Vector3 operator + (const Vector3<T>& b) const
		{
			return Vector3<T>(x + b.x, y + b.y, z + b.z);
		}

		//向量减法
		Vector3 operator - (const Vector3<T>& b) const
		{
			return Vector3<T>(x - b.x, y - b.y, z - b.z);
		}

		//向量数乘
		Vector3 operator * (float d) const
		{
			return Vector3<T>(x * d, y * d, z * d);
		}

		//向量与向量相乘，规定x*x,y*y
		Vector3 operator *(const Vector3<T>& b) const
		{
			return Vector3<T>(x * b.x, y * b.y, z * b.z);
		}

		//向量数除
		Vector3 operator /(float d) const
		{
			if (abs(d) <= kEpsilon<T>())
			{
				return zero();
			}
			return Vector3<T>(x / d, y / d, z / d);
		}

		//向量加法
		Vector3<T>& operator += (const Vector3<T>& b)
		{
			x += b.x;
			y += b.y;
			z += b.z;
			return *this;
		}

		//向量减法
		Vector3<T>& operator -= (const Vector3<T>& b)
		{
			x -= b.x;
			y -= b.y;
			z -= b.z;
			return *this;
		}

		//向量数乘
		Vector3<T>& operator *= (float d)
		{
			x *= d;
			y *= d;
			z *= d;
			return *this;
		}

		//向量与向量相乘，规定x*x,y*y
		Vector3<T>& operator *= (const Vector3<T>& b)
		{
			x *= b.x;
			y *= b.y;
			z *= b.z;
			return *this;
		}

		//向量数除
		Vector3<T>& operator /= (float d)
		{
			if (abs(d) <= kEpsilon<T>())
			{
				x = 0; y = 0; z = 0;
			}
			else
			{
				x /= d;
				y /= d;
				z /= d;
			}
			return *this;
		}

		//相等判断
		bool operator ==(const Vector3<T>& rhs) const
		{
			return abs(x - rhs.x) <= kEpsilon<T>() &&
				abs(y - rhs.y) <= kEpsilon<T>() &&
				abs(z - rhs.z) <= kEpsilon<T>();
		}

		//不等判断
		bool operator !=(const Vector3<T>& rhs) const
		{
			return !(abs(x - rhs.x) <= kEpsilon<T>() &&
				abs(y - rhs.y) <= kEpsilon<T>() &&
				abs(z - rhs.z) <= kEpsilon<T>());
		}

		bool operator < (const Vector3<T>& r) const
		{
			return ToString() < r.ToString();
		}

		std::string ToString() const
		{
			char buff[100] = {};
			sprintf_s(buff, 100, "(%f,%f,%f)", x, y, z);
			return buff;
		}
	protected:
	private:
	};

	//数左乘向量
	template<typename T>
	inline Vector3<T> operator * (float d, const Vector3<T>& v)
	{
		return Vector3<T>(v.x * d, v.y * d, v.z * d);
	}

	inline std::ostream& operator<<(std::ostream& o, const Vector3<float>& v)
	{
		o << "(" << v.x << "," << v.y << "," << v.z << ")";
		return o;
	}

	inline std::ostream& operator<<(std::ostream& o, const Vector3<double>& v)
	{
		o << "(" << v.x << "," << v.y << "," << v.z << ")";
		return o;
	}


	inline std::istream& operator>>(std::istream& i, Vector3<float>& v)
	{
		i >> v.x >> v.y >> v.z;
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Vector3<double>& v)
	{
		i >> v.x >> v.y >> v.z;
		return i;
	}

	typedef Vector3<float> vec3f;
	typedef Vector3<double> vec3d;
}
#endif