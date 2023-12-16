#pragma once
#include "Component.h"
#include "math3d/Matrix4x4.h"
using namespace Math;

class Transform :public Component
{
public:
	//必须使用初始化列表的方式调用父类唯一的有参构造
	Transform(GameObject* pobj) :Component(pobj)
	{
		mType = COMTYPE::ComType_Transform;//确定真正的类型

		mPosition = vec3f::zero();
		mScale = vec3f::one();
		mRotation = vec3f::zero();

		//凡是修改了位置、缩放和旋转信息，都应该重新计算对应的矩阵
		_update_Matrix_Translate();
		_update_Matrix_Scale();
		_update_Matrix_Rotate();

	}


	//提供前后左右上下的方位
	vec3f forward() const { return mForward; }
	vec3f back() const { return -mForward; }
	vec3f right() const { return mRight; }
	vec3f left()  const { return -mRight; }
	vec3f up() const { return mUp; }
	vec3f down()  const { return -mUp; }

	const vec3f& localScale() const { return mScale; }
	const vec3f& position() const { return mPosition; }
	const vec3f& rotation() const { return mRotation; }

	const Mat4x4f& MatT() const { return matrixTranslate; }
	const Mat4x4f& MatS()  const { return matrixScale; }
	const Mat4x4f& MatR()  const { return matrixRotate; }

	void SetPosition(const vec3f& v)
	{
		mPosition = v;
		_update_Matrix_Translate();
		mNeedUpdate = true;
	}
	void SetScale(const vec3f& v)
	{
		mScale = v;
		_update_Matrix_Scale();
		mNeedUpdate = true;
	}
	void SetRotation(const vec3f& v)
	{
		mRotation = v;
		_update_Matrix_Rotate();
		mNeedUpdate = true;
	}


	void translate(const vec3f& tv)
	{
		mPosition += tv;
		_update_Matrix_Translate();
	}

	void scale(const vec3f& sv)
	{
		mScale *= sv;
		_update_Matrix_Scale();
	}

	void rotate(const vec3f& eulerangle);

	void lookat(const vec3f& v, const vec3f* pUp = nullptr);

	bool& NeedUpdate()
	{
		return mNeedUpdate;
	}
protected:
	void _update_EulerAngle_ByAxis();
	void _update_Matrix_Translate();
	void _update_Matrix_Scale();
	void _update_Matrix_Rotate();


protected:
	vec3f mPosition;
	vec3f mScale;
	vec3f mRotation;//存储的是 EulerAngle 欧拉角

	vec3f mForward;//z axis
	vec3f mUp;//y axis
	vec3f mRight;//x axis

	//存储RTS(rotate,translate,scale)矩阵
	Mat4x4f matrixTranslate;
	Mat4x4f matrixScale;
	Mat4x4f matrixRotate;

	bool mNeedUpdate;//是否更新
};

