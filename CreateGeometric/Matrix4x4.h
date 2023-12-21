#pragma once
#include "vector3d.h"
#include <iostream>


class Matrix4x4
{
public:
	union
	{
		struct 
		{
			float _11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34,
				_41, _42, _43, _44;
		};
		float _m44[4][4];
		float _m16[16];
	};


	Matrix4x4()
	{
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _22 = _33 = _44=1;
	}

	//单位化
	void Identity()
	{
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _22 = _33 = _44 = 1;
	}

	Matrix4x4(float __11, float __12, float __13, float __14,
		float __21, float __22, float __23, float __24,
		float __31, float __32, float __33, float __34,
		float __41, float __42, float __43, float __44)
	{
		_11 = __11, _12 = __12, _13 = __13, _14 = __14;
		_21 = __21, _22 = __22, _23 = __23, _24 = __24;
		_31 = __31, _32 = __32, _33 = __33, _34 = __34;
		_41 = __41, _42 = __42, _43 = __43, _44 = __44;
	}

	Matrix4x4(float arr[16])
	{
		memcpy(_m16, arr, sizeof(Matrix4x4));
	}

	Matrix4x4(float arr[4][4])
	{
		memcpy(_m44, arr, sizeof(Matrix4x4));
	}

	Matrix4x4& Set(float __11, float __12, float __13, float __14,
		float __21, float __22, float __23, float __24,
		float __31, float __32, float __33, float __34,
		float __41, float __42, float __43, float __44)
	{
		_11 = __11, _12 = __12, _13 = __13, _14 = __14;
		_21 = __21, _22 = __22, _23 = __23, _24 = __24;
		_31 = __31, _32 = __32, _33 = __33, _34 = __34;
		_41 = __41, _42 = __42, _43 = __43, _44 = __44;

		return *this;
	}
	Matrix4x4& Set(float arr[16])
	{
		memcpy(_m16, arr, sizeof(Matrix4x4));
		return *this;
	}

	Matrix4x4& Set(float arr[4][4])
	{
		memcpy(_m44, arr, sizeof(Matrix4x4));
		return *this;
	}

	//矩阵的乘法
	Matrix4x4 operator * (const Matrix4x4& that)
	{
		Matrix4x4 temp;
		temp._11 = _11 * that._11 + _12 * that._21 + _13 * that._31 + _14 * that._41;
		temp._12 = _11 * that._12 + _12 * that._22 + _13 * that._32 + _14 * that._42;
		temp._13 = _11 * that._13 + _12 * that._23 + _13 * that._33 + _14 * that._43;
		temp._14 = _11 * that._14 + _12 * that._24 + _13 * that._34 + _14 * that._44;

		temp._21 = _21 * that._11 + _22 * that._21 + _23 * that._31 + _24 * that._41;
		temp._22 = _21 * that._12 + _22 * that._22 + _23 * that._32 + _24 * that._42;
		temp._23 = _21 * that._13 + _22 * that._23 + _23 * that._33 + _24 * that._43;
		temp._24 = _21 * that._14 + _22 * that._24 + _23 * that._34 + _24 * that._44;

		temp._31 = _31 * that._11 + _32 * that._21 + _33 * that._31 + _34 * that._41;
		temp._32 = _31 * that._12 + _32 * that._22 + _33 * that._32 + _34 * that._42;
		temp._33 = _31 * that._13 + _32 * that._23 + _33 * that._33 + _34 * that._43;
		temp._34 = _31 * that._14 + _32 * that._24 + _33 * that._34 + _34 * that._44;

		temp._41 = _41 * that._11 + _42 * that._21 + _43 * that._31 + _44 * that._41;
		temp._42 = _41 * that._12 + _42 * that._22 + _43 * that._32 + _44 * that._42;
		temp._43 = _41 * that._13 + _42 * that._23 + _43 * that._33 + _44 * that._43;
		temp._44 = _41 * that._14 + _42 * that._24 + _43 * that._34 + _44 * that._44;

		return temp;
	}
	Matrix4x4 operator * (const Matrix4x4& that) const
	{
		Matrix4x4 temp;
		temp._11 = _11 * that._11 + _12 * that._21 + _13 * that._31 + _14 * that._41;
		temp._12 = _11 * that._12 + _12 * that._22 + _13 * that._32 + _14 * that._42;
		temp._13 = _11 * that._13 + _12 * that._23 + _13 * that._33 + _14 * that._43;
		temp._14 = _11 * that._14 + _12 * that._24 + _13 * that._34 + _14 * that._44;

		temp._21 = _21 * that._11 + _22 * that._21 + _23 * that._31 + _24 * that._41;
		temp._22 = _21 * that._12 + _22 * that._22 + _23 * that._32 + _24 * that._42;
		temp._23 = _21 * that._13 + _22 * that._23 + _23 * that._33 + _24 * that._43;
		temp._24 = _21 * that._14 + _22 * that._24 + _23 * that._34 + _24 * that._44;

		temp._31 = _31 * that._11 + _32 * that._21 + _33 * that._31 + _34 * that._41;
		temp._32 = _31 * that._12 + _32 * that._22 + _33 * that._32 + _34 * that._42;
		temp._33 = _31 * that._13 + _32 * that._23 + _33 * that._33 + _34 * that._43;
		temp._34 = _31 * that._14 + _32 * that._24 + _33 * that._34 + _34 * that._44;

		temp._41 = _41 * that._11 + _42 * that._21 + _43 * that._31 + _44 * that._41;
		temp._42 = _41 * that._12 + _42 * that._22 + _43 * that._32 + _44 * that._42;
		temp._43 = _41 * that._13 + _42 * that._23 + _43 * that._33 + _44 * that._43;
		temp._44 = _41 * that._14 + _42 * that._24 + _43 * that._34 + _44 * that._44;

		return temp;
	}

	Matrix4x4& operator *= (const Matrix4x4& that)
	{
		Matrix4x4 temp;
		temp._11 = _11 * that._11 + _12 * that._21 + _13 * that._31 + _14 * that._41;
		temp._12 = _11 * that._12 + _12 * that._22 + _13 * that._32 + _14 * that._42;
		temp._13 = _11 * that._13 + _12 * that._23 + _13 * that._33 + _14 * that._43;
		temp._14 = _11 * that._14 + _12 * that._24 + _13 * that._34 + _14 * that._44;

		temp._21 = _21 * that._11 + _22 * that._21 + _23 * that._31 + _24 * that._41;
		temp._22 = _21 * that._12 + _22 * that._22 + _23 * that._32 + _24 * that._42;
		temp._23 = _21 * that._13 + _22 * that._23 + _23 * that._33 + _24 * that._43;
		temp._24 = _21 * that._14 + _22 * that._24 + _23 * that._34 + _24 * that._44;

		temp._31 = _31 * that._11 + _32 * that._21 + _33 * that._31 + _34 * that._41;
		temp._32 = _31 * that._12 + _32 * that._22 + _33 * that._32 + _34 * that._42;
		temp._33 = _31 * that._13 + _32 * that._23 + _33 * that._33 + _34 * that._43;
		temp._34 = _31 * that._14 + _32 * that._24 + _33 * that._34 + _34 * that._44;

		temp._41 = _41 * that._11 + _42 * that._21 + _43 * that._31 + _44 * that._41;
		temp._42 = _41 * that._12 + _42 * that._22 + _43 * that._32 + _44 * that._42;
		temp._43 = _41 * that._13 + _42 * that._23 + _43 * that._33 + _44 * that._43;
		temp._44 = _41 * that._14 + _42 * that._24 + _43 * that._34 + _44 * that._44;

		return *this = temp;
	}

	//设置矩阵为平移矩阵
	Matrix4x4& SetTranslate(float x, float y,float z)
	{
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _22 = _33 = _44 = 1;
		_41 = x;
		_42 = y;
		_43 = z;
		return *this;
	}
	//在原来矩阵的基础上进行平移操作，也就是在原来的基础上乘以平移矩阵
	Matrix4x4& Translate(float x, float y,float z)
	{
		//构造了一个平移矩阵
		Matrix4x4 temp;
		temp._41 = x;
		temp._42 = y;
		temp._43 = z;

		//在自己的基础上进行了平移变化
		(*this) *= temp;
		return *this;
	}

	//设置矩阵为平移矩阵
	Matrix4x4& SetTranslate(const vector3d& v)
	{
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _22 = _33 = _44 = 1;
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
		return *this;
	}
	//在原来矩阵的基础上进行平移操作，也就是在原来的基础上乘以平移矩阵
	Matrix4x4& Translate(const vector3d& v)
	{
		//构造了一个平移矩阵
		Matrix4x4 temp;
		temp._41 = v.x;
		temp._42 = v.y;
		temp._43 = v.z;

		//在自己的基础上进行了平移变化
		(*this) *= temp;
		return *this;
	}

	//设置矩阵为缩放矩阵
	Matrix4x4& SetScale(float x, float y,float z)
	{
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _22 = _33 = _44 = 1;
		_11 = x;
		_22 = y;
		_33 = z;
		return *this;
	}
	//在原来矩阵的基础上进行缩放操作，也就是在原来的基础上乘以缩放矩阵
	Matrix4x4& Scale(float x, float y,float z)
	{
		Matrix4x4 temp;
		temp._11 = x;
		temp._22 = y;
		temp._33 = z;

		//在自己的基础上进行了缩放变化
		(*this) *= temp;
		return *this;
	}
	//设置矩阵为缩放矩阵
	Matrix4x4& SetScale(const vector3d& v)
	{
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _22 = _33 = _44 = 1;
		_11 = v.x;
		_22 = v.y;
		_33 = v.z;
		return *this;
	}
	//在原来矩阵的基础上进行缩放操作，也就是在原来的基础上乘以缩放矩阵
	Matrix4x4& Scale(const vector3d& v)
	{
		Matrix4x4 temp;
		temp._11 = v.x;
		temp._22 = v.y;
		temp._33 = v.z;

		//在自己的基础上进行了缩放变化
		(*this) *= temp;
		return *this;
	}
	/*
	三阶旋转矩阵,相当于绕z轴旋转
	c  s  0
	-s c  0
	0  0  1
	*/

	//作业：补全上述的旋转
	//分为角度和弧度
	//分为设置旋转和在自己基础上旋转
	
	/*
	四阶旋转矩阵，绕z轴旋转
	c  s  0  0
	-s c  0  0
	0  0  1  0
	0  0  0  1
	*/
	//设置绕Z轴旋转矩阵，弧度
	Matrix4x4& SetRotateZ_r(float a)
	{
		memset(this, 0, sizeof(Matrix4x4));
		_33 = _44 = 1;
		float c = cos(a);
		float s = sin(a);
		_11 = _22 = c;
		_12 = s;
		_21 = -s;
		return *this;
	}
	//设置绕Z轴旋转矩阵，角度
	Matrix4x4& SetRotateZ_d(float a)
	{
		a = D2R(a);
		memset(this, 0, sizeof(Matrix4x4));
		_33 = _44 = 1;
		float c = cos(a);
		float s = sin(a);
		_11 = _22 = c;
		_12 = s;
		_21 = -s;
		return *this;
	}
	//绕自己本身Z轴旋转，弧度
	Matrix4x4& RotateZ_r(float a)
	{
		Matrix4x4 temp;
		float c = cos(a);
		float s = sin(a);
		temp._11 = temp._22 = c;
		temp._12 = s;
		temp._21 = -s;
		return *this *= temp;
	}
	//绕自己本身Z轴旋转，角度
	Matrix4x4& RotateZ_d(float a)
	{
		a = D2R(a);
		Matrix4x4 temp;
		float c = cos(a);
		float s = sin(a);
		temp._11 = temp._22 = c;
		temp._12 = s;
		temp._21 = -s;
		return *this *= temp;
	}


	/*
		四阶旋转矩阵，绕y轴旋转
		c  0 -s  0
		0  1  0  0
		s  0  c  0
		0  0  0  1
	*/
	//设置绕Y轴旋转矩阵，弧度
	Matrix4x4& SetRotateY_r(float a)
	{
		memset(this, 0, sizeof(Matrix4x4));
		_22 = _44 = 1;
		float c = cos(a);
		float s = sin(a);
		_11 = _33 = c;
		_13 = -s;
		_31 = s;
		return *this;
	}
	//设置绕Y轴旋转矩阵，角度
	Matrix4x4& SetRotateY_d(float a)
	{
		a = D2R(a);
		memset(this, 0, sizeof(Matrix4x4));
		_22 = _44 = 1;
		float c = cos(a);
		float s = sin(a);
		_11 = _33 = c;
		_13 = -s;
		_31 = s;
		return *this;
	}
	//绕自己本身Y轴旋转，弧度
	Matrix4x4& RotateY_r(float a)
	{
		Matrix4x4 temp;
		float c = cos(a);
		float s = sin(a);
		temp._11 = temp._33 = c;
		temp._13 = -s;
		temp._31 = s;
		return *this *= temp;
	}
	//绕自己本身Y轴旋转，角度
	Matrix4x4& RotateY_d(float a)
	{
		a = D2R(a);
		Matrix4x4 temp;
		float c = cos(a);
		float s = sin(a);
		temp._11 = temp._33 = c;
		temp._13 = -s;
		temp._31 = s;
		return *this *= temp;
	}

	/*
		四阶旋转矩阵，绕x轴旋转
		1  0  0  0
		0  c  s  0
		0 -s  c  0
		0  0  0  1
	*/
	//设置绕X轴旋转矩阵，弧度
	Matrix4x4& SetRotateX_r(float a)
	{
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _44 = 1;
		float c = cos(a);
		float s = sin(a);
		_22 = _33 = c;
		_23 = s;
		_32 = -s;
		return *this;
	}
	//设置绕X轴旋转矩阵，角度
	Matrix4x4& SetRotateX_d(float a)
	{
		a = D2R(a);
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _44 = 1;
		float c = cos(a);
		float s = sin(a);
		_22 = _33 = c;
		_23 = s;
		_32 = -s;
		return *this;
	}
	//绕自己本身X轴旋转，弧度
	Matrix4x4& RotateX_r(float a)
	{
		Matrix4x4 temp;
		float c = cos(a);
		float s = sin(a);
		temp._22 = temp._33 = c;
		temp._23 = s;
		temp._32 = -s;
		return *this *= temp;
	}
	//绕自己本身X轴旋转，角度
	Matrix4x4& RotateX_d(float a)
	{
		a = D2R(a);
		Matrix4x4 temp;
		float c = cos(a);
		float s = sin(a);
		temp._22 = temp._33 = c;
		temp._23 = s;
		temp._32 = -s;
		return *this *= temp;
	}

	//设置绕任意轴 n 旋转 弧度 a 的矩阵
	Matrix4x4& SetRotateAxis_r(const vector3d& n, float a)
	{
		float c = cos(a);
		float s = sin(a);
		float t = 1 - c;

		float nxxt = n.x * n.x * t;
		float nyyt = n.y * n.y * t;
		float nzzt = n.z * n.z * t;
		float nxyt = n.x * n.y * t;
		float nyzt = n.y * n.z * t;
		float nzxt = n.z * n.x * t;
		float nxs = n.x * s;
		float nzs = n.z * s;
		float nys = n.y * s;

		_11 = nxxt + c;		_12 = nxyt + nzs;	_13 = nzxt - nys;	_14 = 0;
		_21 = nxyt - nzs;	_22 = nyyt + c;		_23 = nyzt + nxs;	_24 = 0;
		_31 = nzxt + nys;	_32 = nyzt - nxs;	_33 = nxxt + c;		_34 = 0;
		_41 = 0;			_42 = 0;			_43 = 0;			_44 = 1;

		return *this;
	}
	//设置绕任意轴 n 旋转 角度 a 的矩阵
	Matrix4x4& SetRotateAxis_d(const vector3d& n, float a)
	{
		a = D2R(a);
		float c = cos(a);
		float s = sin(a);
		float t = 1 - c;

		float nxxt = n.x * n.x * t;
		float nyyt = n.y * n.y * t;
		float nzzt = n.z * n.z * t;
		float nxyt = n.x * n.y * t;
		float nyzt = n.y * n.z * t;
		float nzxt = n.z * n.x * t;
		float nxs = n.x * s;
		float nzs = n.z * s;
		float nys = n.y * s;

		_11 = nxxt + c;		_12 = nxyt + nzs;	_13 = nzxt - nys;	_14 = 0;
		_21 = nxyt - nzs;	_22 = nyyt + c;		_23 = nyzt + nxs;	_24 = 0;
		_31 = nzxt + nys;	_32 = nyzt - nxs;	_33 = nxxt + c;		_34 = 0;
		_41 = 0;			_42 = 0;			_43 = 0;			_44 = 1;

		return *this;
	}
	//在本身基础上绕任意轴 n 旋转 弧度 a
	Matrix4x4& RotateAxis_r(const vector3d& n, float a)
	{
		float c = cos(a);
		float s = sin(a);
		float t = 1 - c;

		float nxxt = n.x * n.x * t;
		float nyyt = n.y * n.y * t;
		float nzzt = n.z * n.z * t;
		float nxyt = n.x * n.y * t;
		float nyzt = n.y * n.z * t;
		float nzxt = n.z * n.x * t;
		float nxs = n.x * s;
		float nzs = n.z * s;
		float nys = n.y * s;

		Matrix4x4 temp;
		temp._11 = nxxt + c;	temp._12 = nxyt + nzs;	temp._13 = nzxt - nys;
		temp._21 = nxyt - nzs;	temp._22 = nyyt + c;	temp._23 = nyzt + nxs;
		temp._31 = nzxt + nys;	temp._32 = nyzt - nxs;	temp._33 = nxxt + c;

		return *this*=temp;
	}
	//在本身基础上绕任意轴 n 旋转 角度 a
	Matrix4x4& RotateAxis_d(const vector3d& n, float a)
	{
		a = D2R(a);
		float c = cos(a);
		float s = sin(a);
		float t = 1 - c;

		float nxxt = n.x * n.x * t;
		float nyyt = n.y * n.y * t;
		float nzzt = n.z * n.z * t;
		float nxyt = n.x * n.y * t;
		float nyzt = n.y * n.z * t;
		float nzxt = n.z * n.x * t;
		float nxs = n.x * s;
		float nzs = n.z * s;
		float nys = n.y * s;

		Matrix4x4 temp;
		temp._11 = nxxt + c;	temp._12 = nxyt + nzs;	temp._13 = nzxt - nys;
		temp._21 = nxyt - nzs;	temp._22 = nyyt + c;	temp._23 = nyzt + nxs;
		temp._31 = nzxt + nys;	temp._32 = nyzt - nxs;	temp._33 = nxxt + c;

		return *this *= temp;
	}

	//矩阵的一些数学运算
	//将矩阵转置
	Matrix4x4& Transpos()
	{
		float temp = _21;
		_21 = _12, _12 = temp;
		temp = _13, _13 = _31, _31 = temp;
		temp = _14, _14 = _41, _41 = temp;
		temp = _23, _23 = _32, _32 = temp;
		temp = _24, _24 = _42, _42 = temp;
		temp = _34, _34 = _43, _43 = temp;
	}

	//求3阶行列式值
	float GetDetRank3(float src[3][3])
	{
		float det = src[0][0] * src[1][1] * src[2][2]
			+ src[0][1] * src[1][2] * src[2][0]
			+ src[0][2] * src[1][0] * src[2][1]
			- src[0][2] * src[1][1] * src[2][0]
			- src[0][1] * src[1][0] * src[2][2]
			- src[0][0] * src[1][2] * src[2][1];
		return det;
	}
	float GetDetRank3(float src[3][3]) const
	{
		float det = src[0][0] * src[1][1] * src[2][2]
			+ src[0][1] * src[1][2] * src[2][0]
			+ src[0][2] * src[1][0] * src[2][1]
			- src[0][2] * src[1][1] * src[2][0]
			- src[0][1] * src[1][0] * src[2][2]
			- src[0][0] * src[1][2] * src[2][1];
		return det;
	}

	//取任意行列3阶代数余子式
	float GetCofactor(int Row, int Col)
	{
		float dest[3][3] = {};
		float* temp = (float*)dest;//用一维的指针访问二维的内存
		float det = 0;
		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i != Row && j != Col)
				{
					temp[index++] = _m44[i][j];
				}
			}
		}
		det = GetDetRank3(dest);	
		return ((Row + Col) % 2)?-det:det;
	}

	float GetCofactor(int Row, int Col) const
	{
		float dest[3][3] = {};
		float* temp = (float*)dest;//用一维的指针访问二维的内存
		float det = 0;
		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i != Row && j != Col)
				{
					temp[index++] = _m44[i][j];
				}
			}
		}
		//det = pow(double(-1), Row + Col) * GetDetRank3(dest);
		//return det;

		det = GetDetRank3(dest);
		return ((Row + Col) % 2) ? -det : det;
	}

	//求伴随矩阵
	const Matrix4x4& Adjoint()
	{
		Matrix4x4 temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp._m44[j][i] = GetCofactor(i, j);
			}
		}
		return temp;
	}
	const Matrix4x4& Adjoint() const
	{
		Matrix4x4 temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp._m44[j][i] = GetCofactor(i, j);
			}
		}
		return temp;
	}

	//求逆矩阵//传入一个逻辑引用参数，来判断是否可逆
	const Matrix4x4& Inverse(bool& IsInverse)
	{
		Matrix4x4 temp;
		double det = GetDet();
		if (det != 0)
		{
			IsInverse = true;
			temp = Adjoint();
			temp *= (1 / det);
			return temp;
		}
		IsInverse = false;
		return temp;
	}
	const Matrix4x4& Inverse(bool& IsInverse) const
	{
		Matrix4x4 temp;
		double det = GetDet();
		if (det != 0)
		{
			IsInverse = true;
			temp = Adjoint();
			temp *= (1 / det);
			return temp;
		}
		IsInverse = false;
		return temp;
	}
	//求行列式的值
	float GetDet()
	{
		float det = 0;
		for (int i = 0; i < 4; i++)
		{
			det += _m44[i][0] * GetCofactor(i, 0);
		}
		return det;
	}
	float GetDet()const
	{
		float det = 0;
		for (int i = 0; i < 4; i++)
		{
			det += _m44[i][0] * GetCofactor(i, 0);
		}
		return det;
	}
	//矩阵加法
	const Matrix4x4& operator + (const Matrix4x4& other)
	{
		Matrix4x4 temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp._m44[i][j] = _m44[i][j] + other._m44[i][j];
			}
		}
		return temp;
	}
	//矩阵减法
	const Matrix4x4& operator - (const Matrix4x4& other)
	{
		Matrix4x4 temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp._m44[i][j] = _m44[i][j] - other._m44[i][j];
			}
		}
		return temp;
	}
	//矩阵数乘
	const Matrix4x4& operator * (const float k)
	{
		Matrix4x4 temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp._m44[i][j] = _m44[i][j] * k;
			}
		}
		return temp;
	}
	const Matrix4x4& operator + (const Matrix4x4& other) const
	{
		Matrix4x4 temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp._m44[i][j] = _m44[i][j] + other._m44[i][j];
			}
		}
		return temp;
	}
	const Matrix4x4& operator - (const Matrix4x4& other) const
	{
		Matrix4x4 temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp._m44[i][j] = _m44[i][j] - other._m44[i][j];
			}
		}
		return temp;
	}

	const Matrix4x4& operator * (const float k) const
	{
		Matrix4x4 temp;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				temp._m44[i][j] = _m44[i][j] * k;
			}
		}
		return temp;
	}
	Matrix4x4& operator += (const Matrix4x4& other)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				_m44[i][j] += other._m44[i][j];
			}
		}
		return *this;
	}
	Matrix4x4& operator -= (const Matrix4x4& other)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				_m44[i][j] -= other._m44[i][j];
			}
		}
		return *this;
	}
	Matrix4x4& operator *= (const float k)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				_m44[i][j] *= k;
			}
		}
		return *this;
	}
};

//向量和矩阵乘法
inline vector3d operator * (const vector3d& _v, const Matrix4x4& m)
{
	float v[4] = {_v.x,_v.y,_v.z,1};
	float vx = v[0] * m._11 + v[1] * m._21 + v[2] * m._31 + v[3] * m._41;
	float vy = v[0] * m._12 + v[1] * m._22 + v[2] * m._32 + v[3] * m._42;
	float vz = v[0] * m._13 + v[1] * m._23 + v[2] * m._33 + v[3] * m._43;
	float _w = v[0] * m._14 + v[1] * m._24 + v[2] * m._34 + v[3] * m._44;
	vx /= _w;
	vy /= _w;
	vz /= _w;
	return vector3d(vx, vy, vz);
}