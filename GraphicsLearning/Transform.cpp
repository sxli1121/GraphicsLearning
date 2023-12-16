#include "GameObject.h"
#include "UtilityHelper.h"

void Transform::rotate(const vec3f& eulerangle)
{
	vec3f euler_r;
	euler_r.x = D2R(eulerangle.x);
	euler_r.y = D2R(eulerangle.y);
	euler_r.z = D2R(eulerangle.z);
	//此处按照zxy顺规外旋计算欧拉角的旋转结果
	//计算三个标准轴的旋转矩阵
	Mat4x4f _mx; _mx.SetRotateXRadian(euler_r.x);
	Mat4x4f _my; _my.SetRotateYRadian(euler_r.y);
	Mat4x4f _mz; _mz.SetRotateZRadian(euler_r.z);

	//本次旋转的矩阵
	Mat4x4f _mr = _mz * _mx * _my;
	//在本身的基础上旋转
	matrixRotate *= _mr;

	Mat4x4f& r = matrixRotate;

	//根据旋转矩阵和轴向量组成的矩阵互逆的关系，可以直接得到三个轴向的结果
	mRight.x = r._11; mUp.x = r._12; mForward.x = r._13;
	mRight.y = r._21; mUp.y = r._22; mForward.y = r._23;
	mRight.z = r._31; mUp.z = r._32; mForward.z = r._33;



	//根据最终的旋转矩阵求最后的欧拉角
	/*
				| cz  sz  0 |
		R(z) = 	|-sz  cz  0 |
				| 0   0   1 |

				| 1  0   0  |
		R(x) =  | 0  cx  sx |
				| 0 -sx  cx |

				| cy  0 -sy |
		R(y) =  | 0   1  0  |
				| sy  0  cy |

					 | r11  r12  r13 |
		R(zxy) = R = | r21  r22  r23 |
					 | r31  r32  r33 |

					 |cycz-sxsysz   -cxsz   czsy+cysxsz  |
				   = |czsxsy+cysz   cxcz    -cyczsx+sysz |
					 |-cxsy         sx      cxcy         |


		//atan和atan2的区别
		//https://zhuanlan.zhihu.com/p/503524181?utm_id=0
		anglez = atan2(-r12,r22);
		angley = atan2(-r31,r33);
		anglex = atan2(r32,sqrt(r31^2+r33^2))
			   = asin(r32)
	*/
	/*
	//我们现在认为当前的位置关系是从标准轴经过一次欧拉角的旋转得到的
	//求解的思路是倒着转回去，求每次的旋转量
	//因为我们的顺序是zxy，所以先求y的角度
	//将旋转后的z轴向量投影到xz平面,注意当z轴和y重合的时候，不能投影的时候，需要怎么计算
	vec3f xz = vec3f(mForward.x, 0, mForward.z);
	float angley = vec3f::forward().AngleDegree (xz);
	if (mForward.x < 0)
	{
		angley = -angley;
	}
	//按照y的角度反转回去
	Mat4x4f mat_mr_y;
	mat_mr_y.SetRotateYDegree(-angley);
	vec3f cx = mRight * mat_mr_y;
	vec3f cy = mUp * mat_mr_y;
	vec3f cz = mForward * mat_mr_y;

	//反转回去以后求x的旋转量
	float anglex = vec3f::forward().AngleDegree(cz);
	if (cz.y > 0)
	{
		anglex = -anglex;
	}
	//反转回去
	Mat4x4f mat_mr_x;
	mat_mr_x.SetRotateXDegree(-anglex);
	cx = cx * mat_mr_x;
	cy = cy * mat_mr_x;
	cz = cz * mat_mr_x;

	//求z轴的旋转量
	float anglez = vec3f::right().AngleDegree(cx);
	if (cx.y < 0)
	{
		anglez = -anglez;
	}
	mRotation.x = anglex;
	mRotation.y = angley;
	mRotation.z = anglez;
	*/
	//将上述的代码封装到下面的函数中
	_update_EulerAngle_ByAxis();
}

void Transform::lookat(const vec3f& at, const vec3f* pUp)
{
	mForward = (at - mPosition).normalized();
	if (pUp)//如果传入了Up就更新Up否则保持自己的Up
	{
		mUp = *pUp;
	}
	mRight = mUp.Cross(mForward).normalized();
	//从新计算Up
	mUp = mForward.Cross(mRight).normalized();
	//计算旋转矩阵
	matrixRotate.Set(
		mRight.x, mUp.x, mForward.x, 0,
		mRight.y, mUp.y, mForward.y, 0,
		mRight.z, mUp.z, mForward.z, 0,
		0, 0, 0, 1);

	_update_EulerAngle_ByAxis();
}


void Transform::_update_EulerAngle_ByAxis()
{
	//根据最终的轴状态求欧拉角
	if (IS_FLOAT_ZERO(mForward.x) && IS_FLOAT_ZERO(mForward.z))
	{//此时mZ和标准Y轴重合，x可能是90度也可能-90度，我们抛弃y的旋转角度，因为无法确认
		float anglex = 90, angley = 0, anglez;
		if (mForward.y > 0)
		{
			anglex = -90;
		}
		Mat4x4f mat_mr_x;
		mat_mr_x.SetRotateXDegree(-anglex);
		vec3f cx = mRight * mat_mr_x;
		//vector3d cy = mUp * mat_mr_x;
		//vector3d cz = mForward * mat_mr_x;

		anglez = vec3f::right().AngleDegree(cx);
		if (cx.y < 0)
		{
			anglez = -anglez;
		}
		mRotation.x = anglex;
		mRotation.y = angley;
		mRotation.z = anglez;
	}
	else//可以投影的一般情况
	{
		//先求y的角度
		//将旋转后的z轴向量投影到xz平面,注意当z轴和y重合的时候，不能投影的时候，需要怎么计算
		vec3f xz = vec3f(mForward.x, 0, mForward.z);
		float angley = vec3f::forward().AngleDegree(xz);
		if (mForward.x < 0)
		{
			angley = -angley;
		}
		Mat4x4f mat_mr_y;
		mat_mr_y.SetRotateYDegree(-angley);
		vec3f cx = mRight * mat_mr_y;
		//vector3d cy = mUp * mat_mr_y;
		vec3f cz = mForward * mat_mr_y;

		float anglex = vec3f::forward().AngleDegree(cz);
		if (cz.y > 0)
		{
			anglex = -anglex;
		}
		Mat4x4f mat_mr_x;
		mat_mr_x.SetRotateXDegree(-anglex);
		cx = cx * mat_mr_x;
		//cy = cy * mat_mr_x;
		//cz = cz * mat_mr_x;

		float anglez = vec3f::right().AngleDegree(cx);
		if (cx.y < 0)
		{
			anglez = -anglez;
		}
		mRotation.x = anglex;
		mRotation.y = angley;
		mRotation.z = anglez;
	}
	mNeedUpdate = true;
}

void Transform::_update_Matrix_Translate()
{
	matrixTranslate.SetTranslate(mPosition.x, mPosition.y, mPosition.z);  // 平移
}

void Transform::_update_Matrix_Scale()
{
	matrixScale.SetScale(mScale.x, mScale.y, mScale.z);
}

void Transform::_update_Matrix_Rotate()  // 更新旋转矩阵
{
	// 使用已经储存在类内部的欧拉角 计算旋转矩阵
	vec3f euler_r;
	euler_r.x = D2R(mRotation.x);
	euler_r.y = D2R(mRotation.y);
	euler_r.z = D2R(mRotation.z);
	//此处按照zxy顺规外旋计算欧拉角的旋转结果
	//计算三个标准轴的旋转矩阵
	Mat4x4f _mx; _mx.SetRotateXRadian(euler_r.x);
	Mat4x4f _my; _my.SetRotateYRadian(euler_r.y);
	Mat4x4f _mz; _mz.SetRotateZRadian(euler_r.z);

	//本次旋转的矩阵
	matrixRotate = _mz * _mx * _my;

	Mat4x4f& r = matrixRotate;

	//根据旋转矩阵和轴向量组成的矩阵互逆的关系，可以直接得到三个轴向的结果
	mRight.x = r._11; mUp.x = r._12; mForward.x = r._13;
	mRight.y = r._21; mUp.y = r._22; mForward.y = r._23;
	mRight.z = r._31; mUp.z = r._32; mForward.z = r._33;
}
