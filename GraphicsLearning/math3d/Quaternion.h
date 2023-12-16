#pragma once
#ifndef __MATH_3D_QUATERNION_H_
#define __MATH_3D_QUATERNION_H_
#include "Universal.h"
#include "Vector3.h"
//��Ԫ��
/*
��Ԫ���ı�ʾ����
�������� w
ʸ������ V ���߱�ʾΪ (x,y,z)
[w	V]    [w	(x y z)]

//��Ԫ���ļ�������
		[w  V]	=	[cos(��/2) sin(��/2)n]
[w	(x y z)]	=	[cos(��/2) sin(��/2)x sin(��/2)y sin(��/2)z]
*/

namespace Math
{
	template<typename T>
	class Quaternion
	{
	public:
		T w, x, y, z;
		Quaternion() :x(0), y(0), z(0), w(1) {}
		//���ݽǶȺ���������Ԫ��
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

		//ŷ����ת��Ԫ����
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

		//��λ��Ԫ��
		void Identity()
		{
			w = 1;
			x = y = z = 0;
		}
		
		//��Ԫ���为
		Quaternion operator - () const
		{
			Quaternion<T> temp;
			temp.w = -w;
			temp.x = -x;
			temp.y = -y;
			temp.z = -z;
			return temp;
		}
		//��Ԫ���ļӷ�
		Quaternion operator + (const Quaternion& q) const
		{
			Quaternion<T> temp;
			temp.w = w + q.w;
			temp.x = x + q.x;
			temp.y = y + q.y;
			temp.z = z + q.z;
			return temp;
		}
		//��Ԫ���ļ���
		Quaternion operator - (const Quaternion& q) const
		{
			Quaternion<T> temp;
			temp.w = w - q.w;
			temp.x = x - q.x;
			temp.y = y - q.y;
			temp.z = z - q.z;
			return temp;
		}
		//����
		Quaternion operator * (const T r) const
		{
			return Quaternion(w * r, x * r, y * r, z * r);
		}

		//��Ԫ���ĳ˷�
		/*
		q1q2	= [w1 (x1 y1 z1)][w2 (x2 y2 z2)]		
				= [w1 v1][w2 v2]
				= [w1w2 - v1��v2	w1v2+w2v1+v1��v2]


		һ����Ԫ����������Ļ����ڸ���Ԫ��������ĵ�ˣ�Ҳ���ڸ���Ԫ�����ȵ�ƽ��
		qq* = q*q = q��q = |q|^2 = q^2
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

		//ģ��,��Ԫ���Ĵ�С
		/*
		|q| = |[w v]| = sqrt(w^2 + |v|^2)
			= sqrt(cos^2(��/2) + sin^2(��/2)|n|^2)
			//���nΪ��λ����
			= sqrt(cos^2(��/2) + sin^2(��/2))
			//cos^2(��/2) + sin^2(��/2) = 1
			= 1
		*/
		T magnitude() const
		{
			return sqrt(x * x + y * y + z * z + w * w);
		}

		//��һ����Ԫ��
		Quaternion normalized() const
		{
			T mag = sqrt(x * x + y * y + z * z + w * w);
			//���ģ��Ϊ0
			if (mag <= kEpsilon<T>())
			{
				return Quaternion<T>();
			}
			return Quaternion<T>(x / mag, y / mag, z / mag, w / mag);
		}
		Quaternion& Normalize()
		{
			T mag = sqrt(x * x + y * y + z * z + w * w);
			//���ģ��Ϊ0
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
		
		//��Ԫ���Ĺ�������ڸ����ĸ����ͨ����ʾΪ q* ����ͨ������Ԫ����ʸ�����ֱ为�õ���
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

		//��Ԫ������
		/*                -1
		��Ԫ�������ʾΪ q
		�䶨��Ϊ��Ԫ���������С�Ĺ���
		 -1    q*
		q   = ---
			  |q|
		*/
		Quaternion inverse() const
		{
			T mag = sqrt(x * x + y * y + z * z + w * w);
			//���ģ��Ϊ0
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
		//��Ԫ���ĵ��
		/*
		q1��q2 = [w1 v1]��[w2 v2]
		=w1*w1 + v1��v2
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
			//ʹ�õ�˱�ʾ�Ƕȵ� ����
			T cosOmega = Dot(_q);
			//������Ϊ����������һ���������Ԫ���为
			//�԰�����̻�����
			if (cosOmega<0)
			{
				_q = -q;
				cosOmega = -cosOmega;
			}
			float k0, k1;
			//���������Ԫ���Ƿ�ܽӽ�
			//�Ա������0
			if (abs(cosOmega - 1)<kEpsilon<T>())
			{
				k0 = 1 - t;
				k1 = t;
			}
			else
			{
				//1 = sin^2 + cos^2
				T sinOmega = sqrt(1 - cosOmega * cosOmega);
				//ͨ��sin��cos����Ƕ�
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
		//��������֮�����Ԫ��
		static Quaternion GetRotationBetweenVectors(const Vector3<T>& _start, const Vector3<T>& _dest)
		{
			Vector3<T> start = _start.normalized();
			Vector3<T> dest = _dest.normalized();
			//�����н�
			float cosAngle = start.x * dest.x + start.y * dest.y + start.z * dest.z;
			//�������ת��
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