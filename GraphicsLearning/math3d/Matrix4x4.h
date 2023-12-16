#pragma once
#ifndef __MATH_3D_MATRIX_4x4_H__
#define __MATH_3D_MATRIX_4x4_H__
#include "Universal.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Math
{
	template<typename T>
	class Matrix4x4
	{
	public:
		union
		{
			struct
			{
				T _11, _12, _13, _14;
				T _21, _22, _23, _24;
				T _31, _32, _33, _34;
				T _41, _42, _43, _44;
			};

			T m44[4][4];

			T m16[16];
		};

		Matrix4x4()
		{
			_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0;
			_11 = _22 = _33 = _44 = 1;
		}

		Matrix4x4(T __11, T __12, T __13, T __14,
			T __21, T __22, T __23, T __24,
			T __31, T __32, T __33, T __34,
			T __41, T __42, T __43, T __44)
		{
			_11 = __11, _12 = __12, _13 = __13, _14 = __14;
			_21 = __21, _22 = __22, _23 = __23, _24 = __24;
			_31 = __31, _32 = __32, _33 = __33, _34 = __34;
			_41 = __41, _42 = __42, _43 = __43, _44 = __44;
		}
		Matrix4x4(T arr[16])
		{
			memcpy(m16, arr, sizeof(T) * 16);
		}

		Matrix4x4(T arr[4][4])
		{
			memcpy(m44, arr, sizeof(T) * 16);
		}

		void Set(T __11, T __12, T __13, T __14,
			T __21, T __22, T __23, T __24,
			T __31, T __32, T __33, T __34,
			T __41, T __42, T __43, T __44)
		{
			_11 = __11; _12 = __12; _13 = __13; _14 = __14;
			_21 = __21; _22 = __22; _23 = __23; _24 = __24;
			_31 = __31; _32 = __32; _33 = __33; _34 = __34;
			_41 = __41; _42 = __42; _43 = __43; _44 = __44;
		}

		void Set(T arr[16])
		{
			memcpy(m16, arr, sizeof(T) * 16);
		}

		void Set(T arr[4][4])
		{
			memcpy(m44, arr, sizeof(T) * 16);
		}

		void Identity()
		{
			_12 = _21 = _13 = _31 = _14 = _41 = _23 = _32 = _24 = _42 = _34 = _43 = 0;
			_11 = _22 = _33 = _44 = 1;
		}
		//行列式的值
		T Det()
		{
			return _11 * (_22 * _33 * _44 - _24 * _33 * _42) +
				_12 * (_23 * _34 * _41 - _21 * _34 * _43) +
				_13 * (_24 * _31 * _42 - _22 * _31 * _44) +
				_14 * (_21 * _32 * _43 - _23 * _32 * _41);
		}
		//余子式的值,i,j范围1-4
		T Cofactor(int i, int j)
		{
			/*
			对于矩阵Mmxn的余子式Cij
			其实就是去掉第i行第j列的元素组成的M(i-1)x(j-1)的新矩阵
			这里是3x3的矩阵，其余子式的矩阵对应为2x2的新矩阵
			该矩阵的行列式值为：m11*m22-m12*m21
			*/

			int id = i * 10 + j;//11 12 13 14 21 22 23 24 31 32 33 34 41 42 43 44
			T m[9];
			switch (id)
			{
			case 11: {	m[0] = _22; m[1] = _23; m[2] = _24;
				m[3] = _32; m[4] = _33; m[5] = _34;
				m[6] = _42; m[7] = _43; m[8] = _44; }break;//C11

			case 12: {	m[0] = _21; m[1] = _23; m[2] = _24;
				m[3] = _31; m[4] = _33; m[5] = _34;
				m[6] = _41; m[7] = _43; m[8] = _44; }break;//C12

			case 13: {	m[0] = _21; m[1] = _22; m[2] = _24;
				m[3] = _31; m[4] = _32; m[5] = _34;
				m[6] = _41; m[7] = _42; m[8] = _44; }break;//C13

			case 14: {	m[0] = _21; m[1] = _22; m[2] = _23;
				m[3] = _31; m[4] = _32; m[5] = _33;
				m[6] = _41; m[7] = _42; m[8] = _43; }break;//C14

			case 21: {	m[0] = _12; m[1] = _13; m[2] = _14;
				m[3] = _32; m[4] = _33; m[5] = _34;
				m[6] = _42; m[7] = _43; m[8] = _44; }break;//C21

			case 22: {	m[0] = _11; m[1] = _13; m[2] = _14;
				m[3] = _31; m[4] = _33; m[5] = _34;
				m[6] = _41; m[7] = _43; m[8] = _44; }break;//C22

			case 23: {	m[0] = _11; m[1] = _12; m[2] = _14;
				m[3] = _31; m[4] = _32; m[5] = _34;
				m[6] = _41; m[7] = _42; m[8] = _44; }break;//C23

			case 24: {	m[0] = _11; m[1] = _12; m[2] = _13;
				m[3] = _31; m[4] = _32; m[5] = _33;
				m[6] = _41; m[7] = _42; m[8] = _43; }break;//C24

			case 31: {	m[0] = _12; m[1] = _13; m[2] = _14;
				m[3] = _22; m[4] = _23; m[5] = _24;
				m[6] = _42; m[7] = _43; m[8] = _44; }break;//C31

			case 32: {	m[0] = _11; m[1] = _13; m[2] = _14;
				m[3] = _21; m[4] = _23; m[5] = _24;
				m[6] = _41; m[7] = _43; m[8] = _44; }break;//C32

			case 33: {	m[0] = _11; m[1] = _12; m[2] = _14;
				m[3] = _21; m[4] = _22; m[5] = _24;
				m[6] = _41; m[7] = _42; m[8] = _44; }break;//C33

			case 34: {	m[0] = _11; m[1] = _12; m[2] = _13;
				m[3] = _21; m[4] = _22; m[5] = _23;
				m[6] = _41; m[7] = _42; m[8] = _43; }break;//C34

			case 41: {	m[0] = _12; m[1] = _13; m[2] = _14;
				m[3] = _22; m[4] = _23; m[5] = _24;
				m[6] = _32; m[7] = _33; m[8] = _34; }break;//C41

			case 42: {	m[0] = _11; m[1] = _13; m[2] = _14;
				m[3] = _21; m[4] = _23; m[5] = _24;
				m[6] = _31; m[7] = _33; m[8] = _34; }break;//C42

			case 43: {	m[0] = _11; m[1] = _12; m[2] = _14;
				m[3] = _21; m[4] = _22; m[5] = _24;
				m[6] = _31; m[7] = _32; m[8] = _34; }break;//C43

			case 44: {	m[0] = _11; m[1] = _12; m[2] = _13;
				m[3] = _21; m[4] = _22; m[5] = _23;
				m[6] = _31; m[7] = _32; m[8] = _33; }break;//C44

			default: {
				m[0] = _22; m[1] = _23; m[2] = _24;
				m[3] = _32; m[4] = _33; m[5] = _34;
				m[6] = _42; m[7] = _43; m[8] = _44; }//C11
			}
			return m[0] * (m[4] * m[8] - m[5] * m[7]) +
				m[1] * (m[5] * m[6] - m[3] * m[8]) +
				m[2] * (m[3] * m[7] - m[4] * m[6]);
		}

		//伴随矩阵
		Matrix4x4<T> Adj()
		{
			/*
			*		|c11   c12   c13	c14|T
			adj M = |c21   c22   c23	c24|
					|c31   c32   c33	c34|
					|c41   c42   c43	c44|

					|c11   c21   c31	c41|
				  = |c12   c22   c32	c42|
					|c13   c23   c33	c43|
					|c14   c24   c34	c44|
			*/

			Matrix4x4<T> adj;
			adj.Set(
				Cofactor(1, 1), Cofactor(2, 1), Cofactor(3, 1), Cofactor(4, 1),
				Cofactor(1, 2), Cofactor(2, 2), Cofactor(3, 2), Cofactor(4, 2),
				Cofactor(1, 3), Cofactor(2, 3), Cofactor(3, 3), Cofactor(4, 3),
				Cofactor(1, 4), Cofactor(2, 4), Cofactor(3, 4), Cofactor(4, 4));

			return adj;
		}

		//求逆矩阵
		bool Inverse(Matrix4x4<T>& inverseMatrix)
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
				Cofactor(1, 1), Cofactor(2, 1), Cofactor(3, 1), Cofactor(4, 1),
				Cofactor(1, 2), Cofactor(2, 2), Cofactor(3, 2), Cofactor(4, 2),
				Cofactor(1, 3), Cofactor(2, 3), Cofactor(3, 3), Cofactor(4, 3),
				Cofactor(1, 4), Cofactor(2, 4), Cofactor(3, 4), Cofactor(4, 4));

			inverseMatrix /= det;

			return true;
		}

		//转置矩阵
		Matrix4x4<T>& Transpos()
		{
			T temp = _12; _12 = _21; _21 = temp;
			temp = _13; _13 = _31; _31 = temp;
			temp = _14; _14 = _41; _41 = temp;
			temp = _23; _23 = _32; _32 = temp;
			temp = _24; _24 = _42; _42 = temp;
			temp = _34; _34 = _43; _43 = temp;
			return *this;
		}

		//以弧度置旋转矩阵
		Matrix4x4<T>& SetRotateXRadian(T angle)
		{
			T c = cos(angle);
			T s = sin(angle);
			_12 = _13 = _14 = _21 = _24 = _31 = _34 = _41 = _42 = _43 = 0;
			_22 = c;
			_23 = s;
			_32 = -s;
			_33 = c;
			_11 = _44 = 1;
			return *this;
		}
		//以角度置旋转矩阵
		Matrix4x4<T>& SetRotateXDegree(T angle)
		{
			angle = static_cast<T>(D2R(angle));
			T c = cos(angle);
			T s = sin(angle);
			_12 = _13 = _14 = _21 = _24 = _31 = _34 = _41 = _42 = _43 = 0;
			_22 = c;
			_23 = s;
			_32 = -s;
			_33 = c;
			_11 = _44 = 1;
			return *this;
		}
		//在当前矩阵的基础上旋转angle弧度
		Matrix4x4<T>& RotateXRadian(T angle)
		{
			Matrix4x4<T> mat;
			mat.SetRotateXRadian(angle);
			*this *= mat;
			return *this;
		}
		//在当前矩阵的基础上旋转angle角度
		Matrix4x4<T>& RotateXDegree(T angle)
		{
			Matrix4x4<T> mat;
			mat.SetRotateXDegree(angle);
			*this *= mat;
			return *this;
		}

		//以弧度置旋转矩阵
		Matrix4x4<T>& SetRotateYRadian(T angle)
		{
			T c = cos(angle);
			T s = sin(angle);
			_12 = _14 = _21 = _23 = _24 = _32 = _34 = _41 = _42 = _43 = 0;
			_11 = c;
			_13 = -s;
			_31 = s;
			_33 = c;
			_22 = _44 = 1;
			return *this;
		}
		//以角度置旋转矩阵
		Matrix4x4<T>& SetRotateYDegree(T angle)
		{
			angle = static_cast<T>(D2R(angle));
			T c = cos(angle);
			T s = sin(angle);
			_12 = _14 = _21 = _23 = _24 = _32 = _34 = _41 = _42 = _43 = 0;
			_11 = c;
			_13 = -s;
			_31 = s;
			_33 = c;
			_22 = _44 = 1;
			return *this;
		}
		//在当前矩阵的基础上旋转angle弧度
		Matrix4x4<T>& RotateYRadian(T angle)
		{
			Matrix4x4<T> mat;
			mat.SetRotateYRadian(angle);
			*this *= mat;
			return *this;
		}
		//在当前矩阵的基础上旋转angle角度
		Matrix4x4<T>& RotateYDegree(T angle)
		{
			Matrix4x4<T> mat;
			mat.SetRotateYDegree(angle);
			*this *= mat;
			return *this;
		}
		//以弧度置旋转矩阵
		Matrix4x4<T>& SetRotateZRadian(T angle)
		{
			T c = cos(angle);
			T s = sin(angle);
			_13 = _23 = _31 = _32 = _41 = _14 = _42 = _24 = _43 = _34 = 0;
			_11 = c;
			_12 = s;
			_21 = -s;
			_22 = c;
			_33 = _44 = 1;
			return *this;
		}
		//以角度置旋转矩阵
		Matrix4x4<T>& SetRotateZDegree(T angle)
		{
			angle = static_cast<T>(D2R(angle));
			T c = cos(angle);
			T s = sin(angle);
			_13 = _23 = _31 = _32 = _41 = _14 = _42 = _24 = _43 = _34 = 0;
			_11 = c;
			_12 = s;
			_21 = -s;
			_22 = c;
			_33 = _44 = 1;
			return *this;
		}
		//在当前矩阵的基础上旋转angle弧度
		Matrix4x4<T>& RotateZRadian(T angle)
		{
			Matrix4x4<T> mat;
			mat.SetRotateZRadian(angle);
			*this *= mat;
			return *this;
		}
		//在当前矩阵的基础上旋转angle角度
		Matrix4x4<T>& RotateZDegree(T angle)
		{
			Matrix4x4<T> mat;
			mat.SetRotateZDegree(angle);
			*this *= mat;
			return *this;
		}


		Matrix4x4<T>& SetRotateAxisRadian(const Vector3<T>& n, T angle)
		{
			T c = cos(angle);
			T s = sin(angle);
			T onemc = 1 - c;

			T nx2oc = n.x * n.x * onemc;
			T ny2oc = n.y * n.y * onemc;
			T nz2oc = n.z * n.z * onemc;

			T nxyoc = n.x * n.y * onemc;
			T nyzoc = n.y * n.z * onemc;
			T nxzoc = n.x * n.z * onemc;

			T nxs = n.y * s;
			T nys = n.y * s;
			T nzs = n.y * s;

			/*
			绕任意轴旋转的矩阵
			R(n,θ) =
			|nx^2·(1-cosθ) + cosθ		nx·ny·(1-cosθ) + nz·sinθ		nx·nz·(1-cosθ) - ny·sinθ|
			|nx·ny·(1-cosθ) - nz·sinθ	ny^2·(1-cosθ) + cosθ				ny·nz·(1-cosθ) + nx·sinθ|
			|nx·nz·(1-cosθ) + ny·sinθ	ny·nz·(1-cosθ) - nx·sinθ		nz^2·(1-cosθ) + cosθ		 |
			*/

			_11 = nx2oc + c;		_12 = nxyoc * nzs;			_13 = nxzoc - nys;
			_21 = nxyoc - nzs;		_22 = ny2oc + c;			_23 = nyzoc + nxs;
			_31 = nxzoc + nys;		_32 = nyzoc - nxs;			_33 = nz2oc + c;

			_14 = _24 = _34 = _41 = _42 = _42 = 0;
			_44 = 1;
			return *this;
		}
		//以角度置旋转矩阵
		Matrix4x4<T>& SetRotateAxisDegree(const Vector3<T>& n, T angle)
		{
			angle = static_cast<T>(D2R(angle));
			T c = cos(angle);
			T s = sin(angle);
			T onemc = 1 - c;

			T nx2oc = n.x * n.x * onemc;
			T ny2oc = n.y * n.y * onemc;
			T nz2oc = n.z * n.z * onemc;

			T nxyoc = n.x * n.y * onemc;
			T nyzoc = n.y * n.z * onemc;
			T nxzoc = n.x * n.z * onemc;

			T nxs = n.y * s;
			T nys = n.y * s;
			T nzs = n.y * s;

			/*
			绕任意轴旋转的矩阵
			R(n,θ) =
			|nx^2·(1-cosθ) + cosθ		nx·ny·(1-cosθ) + nz·sinθ		nx·nz·(1-cosθ) - ny·sinθ|
			|nx·ny·(1-cosθ) - nz·sinθ	ny^2·(1-cosθ) + cosθ				ny·nz·(1-cosθ) + nx·sinθ|
			|nx·nz·(1-cosθ) + ny·sinθ	ny·nz·(1-cosθ) - nx·sinθ		nz^2·(1-cosθ) + cosθ		 |
			*/

			_11 = nx2oc + c;		_12 = nxyoc * nzs;			_13 = nxzoc - nys;
			_21 = nxyoc - nzs;		_22 = ny2oc + c;			_23 = nyzoc + nxs;
			_31 = nxzoc + nys;		_32 = nyzoc - nxs;			_33 = nz2oc + c;

			_14 = _24 = _34 = _41 = _42 = _42 = 0;
			_44 = 1;
			return *this;
		}
		//在当前矩阵的基础上旋转angle弧度
		Matrix4x4<T>& RotateAxisRadian(const Vector3<T>& n, T angle)
		{
			Matrix4x4<T> mat;
			mat.SetRotateAxisRadian(n, angle);
			*this *= mat;
			return *this;
		}
		//在当前矩阵的基础上旋转angle角度
		Matrix4x4<T>& RotateAxisDegree(const Vector3<T>& n, T angle)
		{
			Matrix4x4<T> mat;
			mat.SetRotateAxisDegree(n, angle);
			*this *= mat;
			return *this;
		}


		//置x，y, z方向缩放矩阵
		Matrix4x4<T>& SetScale(T sx, T sy, T sz)
		{
			_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0;
			_44 = 1;
			_11 = sx;
			_22 = sy;
			_33 = sz;
			return *this;
		}
		//在x,y,z方向上进行缩放
		Matrix4x4<T>& Scale(T sx, T sy, T sz)
		{
			Matrix4x4<T> mat;
			mat.SetScale(sx, sy);
			return *this *= mat;
		}

		//置沿任意轴向缩放k矩阵
		Matrix4x4<T>& SetAxisScale(const Vector3<T>& n, T k)
		{
			T km1 = k - 1;
			T nx2km1 = n.x * n.x * km1;
			T ny2km1 = n.y * n.y * km1;
			T nz2km1 = n.z * n.z * km1;
			T nxykm1 = n.x * n.y * km1;
			T nyzkm1 = n.y * n.z * km1;
			T nxzkm1 = n.x * n.z * km1;

			_11 = 1 + nx2km1;	_12 = nxykm1;		_13 = nxzkm1;
			_21 = nxykm1;		_22 = 1 + ny2km1;	_23 = nyzkm1;
			_31 = nxzkm1;		_32 = nyzkm1;		_33 = 1 + nz2km1;

			_14 = _24 = _34 = _41 = _42 = _43 = 0;
			_44 = 1;

			return *this;
		}
		//沿任意轴向缩放k
		Matrix4x4<T>& AxisScale(const Vector3<T>& n, T k)
		{
			Matrix4x4<T> mat;
			mat.SetAxisScale(n, k);
			return *this *= mat;
		}
		//置平移矩阵
		Matrix4x4<T>& SetTranslate(T dx, T dy, T dz)
		{
			_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = 0;
			_11 = _22 = _33 = _44 = 1;
			_41 = dx;
			_42 = dy;
			_43 = dz;
			return *this;
		}
		//在当前矩阵基础上平移
		Matrix4x4<T>& Translate(T dx, T dy, T dz)
		{
			Matrix4x4<T> mat;
			mat.SetTranslate(dx, dy, dz);
			return *this *= mat;
		}

		Matrix4x4<T> operator * (T k) const
		{
			Matrix4x4<T> mat;
			mat._11 = _11 * k; mat._12 = _12 * k; mat._13 = _13 * k; mat._14 = _14 * k;
			mat._21 = _21 * k; mat._22 = _22 * k; mat._23 = _23 * k; mat._24 = _24 * k;
			mat._31 = _31 * k; mat._32 = _32 * k; mat._33 = _33 * k; mat._34 = _34 * k;
			mat._41 = _41 * k; mat._42 = _42 * k; mat._43 = _43 * k; mat._44 = _44 * k;
			return mat;
		}

		Matrix4x4<T> operator / (T k) const
		{
			Matrix4x4<T> mat;
			if (abs(k) < kEpsilon<T>())
			{
				mat.Identity();
			}
			else
			{
				mat._11 = _11 / k; mat._12 = _12 / k; mat._13 = _13 / k; mat._14 = _14 / k;
				mat._21 = _21 / k; mat._22 = _22 / k; mat._23 = _23 / k; mat._24 = _24 / k;
				mat._31 = _31 / k; mat._32 = _32 / k; mat._33 = _33 / k; mat._34 = _34 / k;
				mat._41 = _41 / k; mat._42 = _42 / k; mat._43 = _43 / k; mat._44 = _44 / k;
			}
			return mat;
		}

		Matrix4x4<T>& operator *= (T k)
		{
			_11 *= k; _12 *= k; _13 *= k; _14 *= k;
			_21 *= k; _22 *= k; _23 *= k; _24 *= k;
			_31 *= k; _32 *= k; _33 *= k; _34 *= k;
			_41 *= k; _42 *= k; _43 *= k; _44 *= k;
			return *this;
		}

		Matrix4x4<T>& operator /= (T k)
		{
			if (abs(k) < kEpsilon<T>())
			{
				Identity();
			}
			else
			{
				_11 /= k; _12 /= k; _13 /= k; _14 /= k;
				_21 /= k; _22 /= k; _23 /= k; _24 /= k;
				_31 /= k; _32 /= k; _33 /= k; _34 /= k;
				_41 /= k; _42 /= k; _43 /= k; _44 /= k;
			}
			return *this;
		}

		Matrix4x4<T> operator * (const Matrix4x4<T>& m) const
		{
			Matrix4x4<T> mat;
			mat._11 = _11 * m._11 + _12 * m._21 + _13 * m._31 + _14 * m._41;
			mat._12 = _11 * m._12 + _12 * m._22 + _13 * m._32 + _14 * m._42;
			mat._13 = _11 * m._13 + _12 * m._23 + _13 * m._33 + _14 * m._43;
			mat._14 = _11 * m._14 + _12 * m._24 + _13 * m._34 + _14 * m._44;

			mat._21 = _21 * m._11 + _22 * m._21 + _23 * m._31 + _24 * m._41;
			mat._22 = _21 * m._12 + _22 * m._22 + _23 * m._32 + _24 * m._42;
			mat._23 = _21 * m._13 + _22 * m._23 + _23 * m._33 + _24 * m._43;
			mat._24 = _21 * m._14 + _22 * m._24 + _23 * m._34 + _24 * m._44;

			mat._31 = _31 * m._11 + _32 * m._21 + _33 * m._31 + _34 * m._41;
			mat._32 = _31 * m._12 + _32 * m._22 + _33 * m._32 + _34 * m._42;
			mat._33 = _31 * m._13 + _32 * m._23 + _33 * m._33 + _34 * m._43;
			mat._34 = _31 * m._14 + _32 * m._24 + _33 * m._34 + _34 * m._44;

			mat._41 = _41 * m._11 + _42 * m._21 + _43 * m._31 + _44 * m._41;
			mat._42 = _41 * m._12 + _42 * m._22 + _43 * m._32 + _44 * m._42;
			mat._43 = _41 * m._13 + _42 * m._23 + _43 * m._33 + _44 * m._43;
			mat._44 = _41 * m._14 + _42 * m._24 + _43 * m._34 + _44 * m._44;

			return mat;
		}
		Matrix4x4<T>& operator *= (const Matrix4x4<T>& m)
		{
			Matrix4x4<T> mat;
			mat._11 = _11 * m._11 + _12 * m._21 + _13 * m._31 + _14 * m._41;
			mat._12 = _11 * m._12 + _12 * m._22 + _13 * m._32 + _14 * m._42;
			mat._13 = _11 * m._13 + _12 * m._23 + _13 * m._33 + _14 * m._43;
			mat._14 = _11 * m._14 + _12 * m._24 + _13 * m._34 + _14 * m._44;

			mat._21 = _21 * m._11 + _22 * m._21 + _23 * m._31 + _24 * m._41;
			mat._22 = _21 * m._12 + _22 * m._22 + _23 * m._32 + _24 * m._42;
			mat._23 = _21 * m._13 + _22 * m._23 + _23 * m._33 + _24 * m._43;
			mat._24 = _21 * m._14 + _22 * m._24 + _23 * m._34 + _24 * m._44;

			mat._31 = _31 * m._11 + _32 * m._21 + _33 * m._31 + _34 * m._41;
			mat._32 = _31 * m._12 + _32 * m._22 + _33 * m._32 + _34 * m._42;
			mat._33 = _31 * m._13 + _32 * m._23 + _33 * m._33 + _34 * m._43;
			mat._34 = _31 * m._14 + _32 * m._24 + _33 * m._34 + _34 * m._44;

			mat._41 = _41 * m._11 + _42 * m._21 + _43 * m._31 + _44 * m._41;
			mat._42 = _41 * m._12 + _42 * m._22 + _43 * m._32 + _44 * m._42;
			mat._43 = _41 * m._13 + _42 * m._23 + _43 * m._33 + _44 * m._43;
			mat._44 = _41 * m._14 + _42 * m._24 + _43 * m._34 + _44 * m._44;

			return *this = mat;
		}
	};

	template<typename T>
	inline Vector3<T> operator * (const Vector3<T>& v3, const Matrix4x4<T>& m)
	{
		Vector4<T> n4;
		n4.x = v3.x * m._11 + v3.y * m._21 + v3.z * m._31 + m._41;
		n4.y = v3.x * m._12 + v3.y * m._22 + v3.z * m._32 + m._42;
		n4.z = v3.x * m._13 + v3.y * m._23 + v3.z * m._33 + m._43;
		n4.w = v3.x * m._14 + v3.y * m._24 + v3.z * m._34 + m._44;
		if (abs(n4.w) < kEpsilon<T>())
		{
			return Vector3<T>(0, 0, 0);
		}
		return Vector3<T>(n4.x / n4.w, n4.y / n4.w, n4.z / n4.w);
	}

	template<typename T>
	inline Vector3<T>& operator *= (Vector3<T>& v3, const Matrix4x4<T>& m)
	{
		Vector4<T> n4;
		n4.x = v3.x * m._11 + v3.y * m._21 + v3.z * m._31 + m._41;
		n4.y = v3.x * m._12 + v3.y * m._22 + v3.z * m._32 + m._42;
		n4.z = v3.x * m._13 + v3.y * m._23 + v3.z * m._33 + m._43;
		n4.w = v3.x * m._14 + v3.y * m._24 + v3.z * m._34 + m._44;
		if (abs(n4.w) < kEpsilon<T>())
		{
			return Vector3<T>(0, 0, 0);
		}
		v3.x = n4.x / n4.w;
		v3.y = n4.y / n4.w;
		v3.z = n4.z / n4.w;
		return v3;
	}


	template<typename T>
	inline Vector4<T> operator * (const Vector4<T>& v4, const Matrix4x4<T>& m)
	{
		Vector4<T> n4;
		n4.x = v4.x * m._11 + v4.y * m._21 + v4.z * m._31 + v4.w * m._41;
		n4.y = v4.x * m._12 + v4.y * m._22 + v4.z * m._32 + v4.w * m._42;
		n4.z = v4.x * m._13 + v4.y * m._23 + v4.z * m._33 + v4.w * m._43;
		n4.w = v4.x * m._14 + v4.y * m._24 + v4.z * m._34 + v4.w * m._44;
		return n4;
	}

	template<typename T>
	inline Vector4<T>& operator *= (Vector4<T>& v4, const Matrix4x4<T>& m)
	{
		Vector4<T> n4;
		n4.x = v4.x * m._11 + v4.y * m._21 + v4.z * m._31 + v4.w * m._41;
		n4.y = v4.x * m._12 + v4.y * m._22 + v4.z * m._32 + v4.w * m._42;
		n4.z = v4.x * m._13 + v4.y * m._23 + v4.z * m._33 + v4.w * m._43;
		n4.w = v4.x * m._14 + v4.y * m._24 + v4.z * m._34 + v4.w * m._44;
		v4 = n4;
		return v4;
	}

	//inline std::ostream& operator<<(std::ostream& o, const Matrix4x4<float>& v)
	//{
	//	o << "(" << v.x << "," << v.y << "," << v.z << ")";
	//	return o;
	//}

	//inline std::ostream& operator<<(std::ostream& o, const Matrix4x4<double>& v)
	//{
	//	o << "(" << v.x << "," << v.y << "," << v.z << ")";
	//	return o;
	//}
	typedef Matrix4x4<float> Mat4x4f;
	typedef Matrix4x4<double> Mat4x4d;
}
#endif