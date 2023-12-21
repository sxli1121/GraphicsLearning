#pragma once
#ifndef __MATRIX_3x3_H__
#define __MATRIX_3x3_H__
#include "vector2d.h"

//单位化
//矩阵的乘法运算（矩阵和矩阵的乘法）
//矩阵的乘法运算（矩阵和向量的乘法）
//平移矩阵、旋转矩阵、缩放矩阵

class Matrix3x3
{
public:
	//使用联合来定义矩阵元素的内容
	//使我们的矩阵元素可以以
	//单独元素或者一维数组或者二维数组的方式来访问
	union
	{
		struct {float _11, _12, _13, _21, _22, _23, _31, _32, _33;};
		float _m33[3][3];
		float _m9[9];
	};
	//单位化单位化矩阵
	void Identity()
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (i == j)
				{
					_m33[i][j] = 1;
				}
				else
				{
					_m33[i][j] = 0;
				}
			}
		}
	}
	//矩阵和矩阵的乘法
	Matrix3x3 operator * (const Matrix3x3& that)
	{
		Matrix3x3 temp;
		temp._11 = _11 * that._11 + _12 * that._21 + _13 * that._31;
		temp._12 = _11 * that._12 + _12 * that._22 + _13 * that._32;
		temp._13 = _11 * that._13 + _12 * that._23 + _13 * that._33;

		temp._21 = _21 * that._11 + _22 * that._21 + _23 * that._31;
		temp._22 = _21 * that._12 + _22 * that._22 + _23 * that._32;
		temp._23 = _21 * that._13 + _22 * that._23 + _23 * that._33;

		temp._31 = _31 * that._11 + _32 * that._21 + _33 * that._31;
		temp._32 = _31 * that._12 + _32 * that._22 + _33 * that._32;
		temp._33 = _31 * that._13 + _32 * that._23 + _33 * that._33;
		return temp;
	}
	Matrix3x3 operator * (const Matrix3x3& that) const
	{
		Matrix3x3 temp;
		temp._11 = _11 * that._11 + _12 * that._21 + _13 * that._31;
		temp._12 = _11 * that._12 + _12 * that._22 + _13 * that._32;
		temp._13 = _11 * that._13 + _12 * that._23 + _13 * that._33;

		temp._21 = _21 * that._11 + _22 * that._21 + _23 * that._31;
		temp._22 = _21 * that._12 + _22 * that._22 + _23 * that._32;
		temp._23 = _21 * that._13 + _22 * that._23 + _23 * that._33;

		temp._31 = _31 * that._11 + _32 * that._21 + _33 * that._31;
		temp._32 = _31 * that._12 + _32 * that._22 + _33 * that._32;
		temp._33 = _31 * that._13 + _32 * that._23 + _33 * that._33;
		return temp;
	}
	Matrix3x3& operator *=(const Matrix3x3& that)
	{
		Matrix3x3 temp;
		temp._11 = _11 * that._11 + _12 * that._21 + _13 * that._31;
		temp._12 = _11 * that._12 + _12 * that._22 + _13 * that._32;
		temp._13 = _11 * that._13 + _12 * that._23 + _13 * that._33;

		temp._21 = _21 * that._11 + _22 * that._21 + _23 * that._31;
		temp._22 = _21 * that._12 + _22 * that._22 + _23 * that._32;
		temp._23 = _21 * that._13 + _22 * that._23 + _23 * that._33;

		temp._31 = _31 * that._11 + _32 * that._21 + _33 * that._31;
		temp._32 = _31 * that._12 + _32 * that._22 + _33 * that._32;
		temp._33 = _31 * that._13 + _32 * that._23 + _33 * that._33;
		(*this) = temp;
		return *this;
	}
	//设置矩阵为平移矩阵
	Matrix3x3& SetTranslate(float x, float y)
	{
		Identity();
		_31 = x;
		_32 = y;
		return *this;
	}
	//在原来矩阵的基础上进行平移操作，也就是在原来的基础上乘以平移矩阵
	Matrix3x3& Translate(float x, float y)
	{
		Matrix3x3 temp;
		temp.SetTranslate(x, y);
		(*this) *= temp;//在自己的基础上进行了平移变化
		return *this;
	}

	//设置矩阵为缩放矩阵
	Matrix3x3& SetScale(float x, float y)
	{
		Identity();
		_11 = x;
		_22 = y;
		return *this;
	}
	//在原来矩阵的基础上进行缩放操作，也就是在原来的基础上乘以缩放矩阵
	Matrix3x3& Scale(float x, float y)
	{
		Matrix3x3 temp;
		temp.SetScale(x, y);
		(*this) *= temp;//在自己的基础上进行了缩放变化
		return *this;
	}

	//设置矩阵为旋转矩阵(弧度)
	Matrix3x3& SetRotate_r(float a)
	{
		Identity();
		float sina = sinf(a);
		float cosa = cosf(a);
		_11 = cosa;
		_12 = sina;
		_21 = -sina;
		_22 = cosa;
		return *this;
	}
	//设置矩阵为旋转矩阵（角度）
	Matrix3x3& SetRotate_d(float a)
	{
		return SetRotate_r(float(a / 180.0f*M_PI));
	}
	//在原来矩阵的基础上进行旋转操作，也就是在原来的基础上乘以旋转矩阵(弧度)
	Matrix3x3& Rotate_r(float a)
	{
		Matrix3x3 temp;
		temp.SetRotate_r(a);
		(*this) *= temp;//在自己的基础上进行了旋转变化
		return *this;
	}
	//在原来矩阵的基础上进行旋转操作，也就是在原来的基础上乘以旋转矩阵(角度)
	Matrix3x3& Rotate_d(float a)
	{
		Matrix3x3 temp;
		temp.SetRotate_r(float(a / 180.0f*M_PI));
		(*this) *= temp;//在自己的基础上进行了旋转变化
		return *this;
	}
};

inline vector2d operator * (const vector2d& _v, const Matrix3x3& m)
{
	float v[3] = { _v.x,_v.y,1 };
	float vx = v[0] * m._11 + v[1] * m._21 + v[2] * m._31;
	float vy = v[0] * m._12 + v[1] * m._22 + v[2] * m._32;
	float vz = v[0] * m._13 + v[1] * m._23 + v[2] * m._33;
	vx /= vz;
	vy /= vz;
	return vector2d(vx, vy);
}
#endif // !__MATRIX_3x3_H__






