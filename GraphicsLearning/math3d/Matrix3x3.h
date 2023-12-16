#pragma once
#ifndef __MATH_3D_MATRIX_3x3_H__
#define __MATH_3D_MATRIX_3x3_H__
#include "Universal.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Math
{
	template<typename T>
	class Matrix3x3
	{
	public:
		union
		{
			struct
			{
				T _11, _12, _13;
				T _21, _22, _23;
				T _31, _32, _33;
			};

			T m33[3][3];

			T m9[9];
		};
		void Set(T __11, T __12, T __13,
			T __21, T __22, T __23,
			T __31, T __32, T __33)
		{
			_11 = __11; _12 = __12; _13 = __13;
			_21 = __21; _22 = __22; _23 = __23;
			_31 = __31; _32 = __32; _33 = __33;
		}

		void Set(T arr[9])
		{
			memcpy(m9, arr, sizeof(T) * 9);
		}

		void Set(T arr[3][3])
		{
			memcpy(m33, arr, sizeof(T) * 9);
		}

		void Identity()
		{
			_12 = _13 = _21 = _23 = _31 = _32 = 0;
			_11 = _22 = _33 = 1;
		}
		//行列式的值
		T Det()
		{
			return _11 * (_22 * _33 - _23 * _32) +
				_12 * (_23 * _31 - _21 * _33) +
				_13 * (_21 * _32 - _22 * _31);
		}
		//余子式的值,i,j范围1-3
		T Cofactor(int i, int j)
		{
			/*
			对于矩阵Mmxn的余子式Cij
			其实就是去掉第i行第j列的元素组成的M(i-1)x(j-1)的新矩阵
			这里是3x3的矩阵，其余子式的矩阵对应为2x2的新矩阵
			该矩阵的行列式值为：m11*m22-m12*m21
			*/

			int id = i * 10 + j;//11 12 13 21 22 23 31 32 33
			T m[4];
			switch (id)
			{
			case 11: {m[0] = _22; m[1] = _23; m[2] = _32; m[3] = _33; }break;//C11
			case 12: {m[0] = _21; m[1] = _23; m[2] = _31; m[3] = _33; }break;//C12
			case 13: {m[0] = _21; m[1] = _22; m[2] = _31; m[3] = _32; }break;//C13
			case 21: {m[0] = _12; m[1] = _13; m[2] = _32; m[3] = _33; }break;//C21
			case 22: {m[0] = _11; m[1] = _13; m[2] = _31; m[3] = _33; }break;//C22
			case 23: {m[0] = _11; m[1] = _12; m[2] = _31; m[3] = _32; }break;//C23
			case 31: {m[0] = _12; m[1] = _13; m[2] = _22; m[3] = _23; }break;//C31
			case 32: {m[0] = _11; m[1] = _13; m[2] = _21; m[3] = _23; }break;//C32
			case 33: {m[0] = _11; m[1] = _12; m[2] = _21; m[3] = _22; }break;//C33
			default: {m[0] = _22; m[1] = _23; m[2] = _32; m[3] = _33; }//C11
			}
			return m[0] * m[3] - m[1] * m[2];
		}

		//伴随矩阵
		Matrix3x3<T> Adj()
		{
			/*
			*		|c11   c12   c13|T
			adj M = |c21   c22   c23|
					|c31   c32   c33|

					|c11   c21   c31|
				  = |c12   c22   c32|
					|c13   c23   c33|
			*/

			Matrix3x3<T> adj;
			adj.Set(
				Cofactor(1, 1), Cofactor(2, 1), Cofactor(3, 1),
				Cofactor(1, 2), Cofactor(2, 2), Cofactor(3, 2),
				Cofactor(1, 3), Cofactor(2, 3), Cofactor(3, 3));

			return adj;
		}

		//求逆矩阵
		bool Inverse(Matrix3x3<T>& inverseMatrix)
		{

			/*

			  -1    adj M		//伴随矩阵 Adj
			M    = ————
					 |M|		//行列式值 Det

			*/

			T det = Det();
			if (abs(det) < kEpsilon<T>())
			{
				return false;
			}

			inverseMatrix.Set(
				Cofactor(1, 1), Cofactor(2, 1), Cofactor(3, 1),
				Cofactor(1, 2), Cofactor(2, 2), Cofactor(3, 2),
				Cofactor(1, 3), Cofactor(2, 3), Cofactor(3, 3));

			inverseMatrix /= det;

			return true;
		}

		//转置矩阵
		Matrix3x3<T>& Transpos()
		{
			T temp = _12; _12 = _21; _21 = temp;
			temp = _13; _13 = _31; _31 = temp;
			temp = _23; _23 = _32; _32 = temp;
			return *this;
		}

		//以弧度置旋转矩阵
		Matrix3x3<T>& SetRotateRadian(T angle)
		{
			T c = cos(angle);
			T s = sin(angle);
			_13 = _23 = _31 = _32 = 0;
			_11 = c;
			_12 = s;
			_21 = -s;
			_22 = c;
			_33 = 1;
			return *this;
		}
		//以角度置旋转矩阵
		Matrix3x3<T>& SetRotateDegree(T angle)
		{
			angle = static_cast<T>(D2R(angle));
			T c = cos(angle);
			T s = sin(angle);
			_13 = _23 = _31 = _32 = 0;
			_11 = c;
			_12 = s;
			_21 = -s;
			_22 = c;
			_33 = 1;
			return *this;
		}
		//在当前矩阵的基础上旋转angle弧度
		Matrix3x3<T>& RotateRadian(T angle)
		{
			Matrix3x3<T> mat;
			mat.SetRotateRadian(angle);
			*this *= mat;
			return *this;
		}
		//在当前矩阵的基础上旋转angle角度
		Matrix3x3<T>& RotateDegree(T angle)
		{
			Matrix3x3<T> mat;
			mat.SetRotateDegree(angle);
			*this *= mat;
			return *this;
		}
		//置x，y方向缩放矩阵
		Matrix3x3<T>& SetScale(T sx, T sy)
		{
			_12 = _13 = _21 = _23 = _31 = _32 = 0;
			_33 = 1;
			_11 = sx;
			_22 = sy;
			return *this;
		}
		//在x,y方向上进行缩放
		Matrix3x3<T>& Scale(T sx, T sy)
		{
			Matrix3x3<T> mat;
			mat.SetScale(sx, sy);
			return *this *= mat;
		}

		//置沿任意轴向缩放k矩阵
		Matrix3x3<T>& SetAxisScale(const Vector2<T>& n, T k)
		{
			T km1 = k - 1;
			T nx2km1 = n.x * n.x * km1;
			T ny2km1 = n.y * n.y * km1;
			T nxykm1 = n.x * n.y * km1;

			_11 = 1 + nx2km1;	_12 = nxykm1;
			_21 = nxykm1;		_22 = 1 + ny2km1;

			_13 = _23 = _31 = _32 = 0;
			_33 = 1;

			return *this;
		}
		//沿任意轴向缩放k
		Matrix3x3<T>& AxisScale(const Vector2<T>& n, T k)
		{
			Matrix3x3<T> mat;
			mat.SetAxisScale(n, k);
			return *this *= mat;
		}
		//置平移矩阵
		Matrix3x3<T>& SetTranslate(T dx, T dy)
		{
			_12 = _13 = _21 = _23 = 0;
			_11 = _22 = _33 = 1;
			_31 = dx;
			_32 = dy;
			return *this;
		}
		//在当前矩阵基础上平移
		Matrix3x3<T>& Translate(T dx, T dy)
		{
			Matrix3x3<T> mat;
			mat.SetTranslate(dx, dy);
			return *this *= mat;
		}

		//错切变换
		Matrix3x3<T>& SetShearing(T sx, T sy)
		{
			_13 = _23 = _31 = _32 = 0;
			_11 = _22 = _33 = 1;
			_21 = sx;
			_12 = sy;
			return *this;
		}

		Matrix3x3<T>& Shearing(T sx, T sy)
		{
			Matrix3x3<T> mat;
			mat.SetShearing(sx, sy);
			return *this *= mat;
		}

		//轴镜像变换
		Matrix3x3<T>& SetMirroring(const Vector2<T>& mirror_axis)
		{
			Vector2<T> n = mirror_axis.Perpendicular().normalized();

			_11 = 1 - 2 * n.x * n.x;
			_12 = -2 * n.x * n.y;
			_21 = _12;
			_22 = 1 - 2 * n.y * n.y;

			_13 = _23 = _31 = _32 = 0;
			_33 = 1;

			return *this;
		}

		Matrix3x3<T>& Mirroring(const Vector2<T>& mirror_axis)
		{
			Matrix3x3<T> mat;
			mat.SetMirroring(mirror_axis);
			return *this *= mat;
		}

		Matrix3x3<T> operator * (T k) const
		{
			Matrix3x3<T> mat;
			mat._11 = _11 * k; mat._12 = _12 * k; mat._13 = _13 * k;
			mat._21 = _21 * k; mat._22 = _22 * k; mat._23 = _23 * k;
			mat._31 = _31 * k; mat._32 = _32 * k; mat._33 = _33 * k;
			return mat;
		}

		Matrix3x3<T> operator / (T k) const
		{
			Matrix3x3<T> mat;
			if (abs(k) < kEpsilon<T>())
			{
				mat.Identity();
			}
			else
			{
				mat._11 = _11 / k; mat._12 = _12 / k; mat._13 = _13 / k;
				mat._21 = _21 / k; mat._22 = _22 / k; mat._23 = _23 / k;
				mat._31 = _31 / k; mat._32 = _32 / k; mat._33 = _33 / k;
			}
			return mat;
		}

		Matrix3x3<T>& operator *= (T k)
		{
			_11 *= k; _12 *= k; _13 *= k;
			_21 *= k; _22 *= k; _23 *= k;
			_31 *= k; _32 *= k; _33 *= k;
			return *this;
		}

		Matrix3x3<T>& operator /= (T k)
		{
			if (abs(k) < kEpsilon<T>())
			{
				Identity();
			}
			else
			{
				_11 /= k; _12 /= k; _13 /= k;
				_21 /= k; _22 /= k; _23 /= k;
				_31 /= k; _32 /= k; _33 /= k;
			}
			return *this;
		}

		Matrix3x3<T> operator * (const Matrix3x3<T>& m) const
		{
			Matrix3x3<T> mat;
			mat._11 = _11 * m._11 + _12 * m._21 + _13 * m._31;
			mat._12 = _11 * m._12 + _12 * m._22 + _13 * m._32;
			mat._13 = _11 * m._13 + _12 * m._23 + _13 * m._33;

			mat._21 = _21 * m._11 + _22 * m._21 + _23 * m._31;
			mat._22 = _21 * m._12 + _22 * m._22 + _23 * m._32;
			mat._23 = _21 * m._13 + _22 * m._23 + _23 * m._33;

			mat._31 = _31 * m._11 + _32 * m._21 + _33 * m._31;
			mat._32 = _31 * m._12 + _32 * m._22 + _33 * m._32;
			mat._33 = _31 * m._13 + _32 * m._23 + _33 * m._33;

			return mat;
		}
		Matrix3x3<T>& operator *= (const Matrix3x3<T>& m)
		{
			Matrix3x3<T> mat;
			mat._11 = _11 * m._11 + _12 * m._21 + _13 * m._31;
			mat._12 = _11 * m._12 + _12 * m._22 + _13 * m._32;
			mat._13 = _11 * m._13 + _12 * m._23 + _13 * m._33;

			mat._21 = _21 * m._11 + _22 * m._21 + _23 * m._31;
			mat._22 = _21 * m._12 + _22 * m._22 + _23 * m._32;
			mat._23 = _21 * m._13 + _22 * m._23 + _23 * m._33;

			mat._31 = _31 * m._11 + _32 * m._21 + _33 * m._31;
			mat._32 = _31 * m._12 + _32 * m._22 + _33 * m._32;
			mat._33 = _31 * m._13 + _32 * m._23 + _33 * m._33;

			return *this = mat;
		}

		//为了兼容d2d或者gdi的变换矩阵
		T* GetMat3x2()
		{
			static T m32[6] = { 0 };
			m32[0] = _11;
			m32[1] = _12;
			m32[2] = _21;
			m32[3] = _22;
			m32[4] = _31;
			m32[5] = _32;
			return m32;
		}
	};
	template<typename T>
	inline Vector2<T> operator * (const Vector2<T>& v2, const Matrix3x3<T>& m)
	{
		//Vector3<T> v3 = { v2.x,v2.y,1 };
		Vector3<T> n3;
		n3.x = v2.x * m._11 + v2.y * m._21 + m._31;
		n3.y = v2.x * m._12 + v2.y * m._22 + m._32;
		n3.z = v2.x * m._13 + v2.y * m._23 + m._33;
		if (abs(n3.z) < kEpsilon<T>())
		{
			return Vector2<T>(0, 0);
		}
		return Vector2<T>(n3.x / n3.z, n3.y / n3.z);
	}

	template<typename T>
	inline Vector2<T>& operator *= (Vector2<T>& v2, const Matrix3x3<T>& m)
	{
		Vector3<T> n3;
		n3.x = v2.x * m._11 + v2.y * m._21 + m._31;
		n3.y = v2.x * m._12 + v2.y * m._22 + m._32;
		n3.z = v2.x * m._13 + v2.y * m._23 + m._33;
		if (abs(n3.z) < kEpsilon<T>())
		{
			v2.x = 0;
			v2.y = 0;
			return v2;
		}
		v2.x = n3.x / n3.z;
		v2.y = n3.y / n3.z;
		return v2;
	}

	template<typename T>
	inline Vector3<T> operator * (const Vector3<T>& v3, const Matrix3x3<T>& m)
	{
		Vector3<T> n3;
		n3.x = v3.x * m._11 + v3.y * m._21 + v3.z * m._31;
		n3.y = v3.x * m._12 + v3.y * m._22 + v3.z * m._32;
		n3.z = v3.x * m._13 + v3.y * m._23 + v3.z * m._33;
		return n3;
	}

	template<typename T>
	inline Vector3<T>& operator *= (Vector3<T>& v3, const Matrix3x3<T>& m)
	{
		Vector3<T> n3;
		n3.x = v3.x * m._11 + v3.y * m._21 + v3.z * m._31;
		n3.y = v3.x * m._12 + v3.y * m._22 + v3.z * m._32;
		n3.z = v3.x * m._13 + v3.y * m._23 + v3.z * m._33;
		return v3 = n3;
	}

	typedef Matrix3x3<float> Mat3x3f;
	typedef Matrix3x3<double> Mat3x3d;
}
#endif