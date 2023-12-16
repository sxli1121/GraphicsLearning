#pragma once
#ifndef __MATH_3D_QUATERNION_H_
#define __MATH_3D_QUATERNION_H_
#include "Universal.h"
#include "Vector3.h"
//四元数
/*
四元数的表示方法
标量分量 w
矢量分量 V 或者表示为 (x,y,z)
[w	V]    [w	(x y z)]

//四元数的几何意义
		[w  V]	=	[cos(θ/2) sin(θ/2)n]
[w	(x y z)]	=	[cos(θ/2) sin(θ/2)x sin(θ/2)y sin(θ/2)z]
*/

namespace Math
{
	template<typename T>
	class Quaternion
	{
	public:
		T w, x, y, z;
		Quaternion() :x(0), y(0), z(0), w(1) {}
		//根据角度和轴生成四元数
		Quaternion(T angle, const Vector3<T>& v)
		{
			T theta = angle / 2;
			T sin_theta = sin(theta);
			Vector3<T> n = v.normalized();
			w = cos(theta);
			x = sin_theta * n.x;
			y = sin_theta * n.y;
			z = sin_theta * n.z;
		}

		//欧拉角转四元数？
		Quaternion(T eulerx, T eulery, T eulerz)
		{
			T hx = eulerx * 0.5;
			T hy = eulery * 0.5;
			T hz = eulerz * 0.5;

			T cx = cos(hx);
			T cy = cos(hy);
			T cz = cos(hz);

			T sx = sin(hx);
			T sy = sin(hy);
			T sz = sin(hz);

			w = cx * cy * cz + sx * sy * sz;
			x = sx * cy * cz - cx * sy * sz;
			y = cx * sy * cz + sx * cy * sz;
			z = cx * cy * sz - sx * sy * cz;
		}

		Quaternion(const Vector3<T>& euler)
		{
			T hx = euler.x * 0.5;
			T hy = euler.y * 0.5;
			T hz = euler.z * 0.5;

			T cx = cos(hx);
			T cy = cos(hy);
			T cz = cos(hz);

			T sx = sin(hx);
			T sy = sin(hy);
			T sz = sin(hz);

			w = cx * cy * cz + sx * sy * sz;
			x = sx * cy * cz - cx * sy * sz;
			y = cx * sy * cz + sx * cy * sz;
			z = cx * cy * sz - sx * sy * cz;
		}

		Quaternion(T _x, T _y, T _z, T _w) :x(_x), y(_y), z(_y), w(_w) {}

		//单位四元数
		void Identity()
		{
			w = 1;
			x = y = z = 0;
		}
		
		//四元数变负
		Quaternion operator - () const
		{
			Quaternion<T> temp;
			temp.w = -w;
			temp.x = -x;
			temp.y = -y;
			temp.z = -z;
			return temp;
		}
		//四元数的加法
		Quaternion operator + (const Quaternion& q) const
		{
			Quaternion<T> temp;
			temp.w = w + q.w;
			temp.x = x + q.x;
			temp.y = y + q.y;
			temp.z = z + q.z;
			return temp;
		}
		//四元数的减法
		Quaternion operator - (const Quaternion& q) const
		{
			Quaternion<T> temp;
			temp.w = w - q.w;
			temp.x = x - q.x;
			temp.y = y - q.y;
			temp.z = z - q.z;
			return temp;
		}
		//数乘
		Quaternion operator * (const T r) const
		{
			return Quaternion(w * r, x * r, y * r, z * r);
		}

		//四元数的乘法
		/*
		q1q2	= [w1 (x1 y1 z1)][w2 (x2 y2 z2)]		
				= [w1 v1][w2 v2]
				= [w1w2 - v1·v2	w1v2+w2v1+v1×v2]


		一个四元数与它共轭的积等于该四元数与自身的点乘，也等于该四元数长度的平方
		qq* = q*q = q·q = |q|^2 = q^2
		*/
		Quaternion operator * (const Quaternion& q) const
		{
			Quaternion<T> r;
			r.w = w * q.w - x * q.x - y * q.y - z * q.z;
			r.x = w * q.x + x * q.w + y * q.z - z * q.y;
			r.y = w * q.y + y * q.w + z * q.x - x * q.z;
			r.z = w * q.z + z * q.w + x * q.y - y * q.x;
			return r;
		}

		//模长,四元数的大小
		/*
		|q| = |[w v]| = sqrt(w^2 + |v|^2)
			= sqrt(cos^2(θ/2) + sin^2(θ/2)|n|^2)
			//如果n为单位向量
			= sqrt(cos^2(θ/2) + sin^2(θ/2))
			//cos^2(θ/2) + sin^2(θ/2) = 1
			= 1
		*/
		T magnitude() const
		{
			return sqrt(x * x + y * y + z * z + w * w);
		}

		//归一化四元数
		Quaternion normalized() const
		{
			T mag = sqrt(x * x + y * y + z * z + w * w);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				return Quaternion<T>();
			}
			return Quaternion<T>(x / mag, y / mag, z / mag, w / mag);
		}
		Quaternion& Normalize()
		{
			T mag = sqrt(x * x + y * y + z * z + w * w);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				w = 1;
				x = y = z = 0;
			}
			else
			{
				x /= mag, y /= mag, z /= mag, w /= mag;
			}
			return *this;
		}
		
		//四元数的共轭（来自于复数的概念）：通常表示为 q* ，是通过将四元数的矢量部分变负得到的
		/*
		q*	= [w v]* = [w  -v]
			= [w (x y z)]* = [w (-x -y -z)]
		*/
		Quaternion conjugate() const
		{
			Quaternion<T> temp;
			temp.w = w;
			temp.x = -x;
			temp.y = -y;
			temp.z = -z;
			return temp;
		}

		//四元数的逆
		/*                -1
		四元数的逆表示为 q
		其定义为四元数除以其大小的共轭
		 -1    q*
		q   = ---
			  |q|
		*/
		Quaternion inverse() const
		{
			T mag = sqrt(x * x + y * y + z * z + w * w);
			//如果模长为0
			if (mag <= kEpsilon<T>())
			{
				return Quaternion<T>();
			}

			Quaternion<T> temp;
			temp.w = w / mag;
			temp.x = -x / mag;
			temp.y = -y / mag;
			temp.z = -z / mag;
			
			return temp;
		}
		//四元数的点乘
		/*
		q1·q2 = [w1 v1]·[w2 v2]
		=w1*w1 + v1·v2
		=[w1  (x1 y1 z1)][w2  (x2 y2 z2)]
		=w1w2 + x1x2 + y1y2 + z1z2
		*/
		T Dot(const Quaternion<T>& q)
		{
			return w * q.w + x * q.x + y * q.y + z * q.z;
		}

		Quaternion Slerp(const Quaternion& q, T t)
		{
			if (t < 0) { return *this; }
			if (t > 1.0f) { return q; }

			Quaternion<T> _q = q;
			//使用点乘表示角度的 余弦
			T cosOmega = Dot(_q);
			//如果点乘为负，则将其中一个输入的四元数变负
			//以按照最短弧计算
			if (cosOmega<0)
			{
				_q = -q;
				cosOmega = -cosOmega;
			}
			float k0, k1;
			//检查两个四元数是否很接近
			//以避免除到0
			if (abs(cosOmega - 1)<kEpsilon<T>())
			{
				k0 = 1 - t;
				k1 = t;
			}
			else
			{
				//1 = sin^2 + cos^2
				T sinOmega = sqrt(1 - cosOmega * cosOmega);
				//通过sin和cos计算角度
				T omega = atan2(sinOmega, cosOmega);

				T oneOverSinOmega = 1 / sinOmega;

				k0 = sin(1 - t) * omega * oneOverSinOmega;
				k1 = sin(t) * omega * oneOverSinOmega;
			}

			Quaternion<T> r;
			r.w = w * k0 + _q.w * k1;
			r.x = x * k0 + _q.x * k1;
			r.y = y * k0 + _q.y * k1;
			r.z = z * k0 + _q.z * k1;
			return r;
		}
		//两个向量之间的四元数
		static Quaternion GetRotationBetweenVectors(const Vector3<T>& _start, const Vector3<T>& _dest)
		{
			Vector3<T> start = _start.normalized();
			Vector3<T> dest = _dest.normalized();
			//点乘求夹角
			float cosAngle = start.x * dest.x + start.y * dest.y + start.z * dest.z;
			//叉乘求旋转轴
			Vector3<T> rotateAxis(start.y * dest.z - start.z * dest.y,
				start.z * dest.x - start.x * dest.z,
				start.x * dest.y - start.y * dest.x);
			rotateAxis.Normalize();

			float s = sqrtf((1 + cosAngle) * 2) * 0.5f;//s = cos(angle/2);
			float invs = sqrtf(1 - s * s);

			rotateAxis *= invs;

			Quaternion q(s, rotateAxis.x, rotateAxis.y, rotateAxis.z);
			return q.normalized();
		}
	};

	template<typename T>
	inline Vector3<T> operator *(const Vector3<T>& v, const Quaternion<T>& q)
	{
		//Quaternion<T> p;
		//p.w = 0;
		//p.x = v.x;
		//p.y = v.y;
		//p.z = v.z;

		//Quaternion<T> _q = q.conjugate();

		//Quaternion<T> r = q * p * _q;
		//return Vector3<T>(r.x, r.y, r.z);

		T dq = q.w * q.w - q.x * q.x - q.y * q.y - q.z * q.z;
		T x1 = dq * v.x;
		T y1 = dq * v.y;
		T z1 = dq * v.z;

		T dwz = q.w * q.z * 2, dwy = q.w * q.y * 2, dwx = q.w * q.x * 2;
		T x2 = -dwz * v.y + dwy * v.z;
		T y2 = dwz * v.x - dwx * v.z;
		T z2 = -dwy * v.x + dwx * v.y;

		T dxx = q.x * q.x * 2, dyy = q.y * q.y * 2, dzz = q.z * q.z * 2,
			dxy = q.x * q.y * 2, dxz = q.x * q.z * 2, dyz = q.y * q.z * 2;
		T x3 = dxx * v.x + dxy * v.y + dxz * v.z;
		T y3 = dxy * v.x + dyy * v.y + dyz * v.z;
		T z3 = dxz * v.x + dyz * v.y + dzz * v.z;
		return Vector3<T>(x1 + x2 + x3, y1 + y2 + y3, z1 + z2 + z3);
	}

	typedef Quaternion<float> Quaternionf;
	typedef Quaternion<double> Quaterniond;
}
#endif