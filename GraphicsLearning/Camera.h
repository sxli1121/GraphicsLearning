#pragma once
#ifndef __CAMERA_3D_H__
#define __CAMERA_3D_H__
#include "Component.h"
#include "math3d/Matrix4x4.h"
using namespace Math;
#include "Geometric.h"

typedef enum class emCameraType
{
	Camera_Perspective,
	Camera_Orthogonal
}CAMERA_TYPE, * LP_CAMERA_TYPE;


class Camera :public Component
{
public:
	static Camera* mainCamera;
	Camera(GameObject* pobj);//成员初始化列表构造父类

	Mat4x4f MatrixView;//摄像机变换矩阵
	Mat4x4f MatrixProjection;//投影矩阵
	Mat4x4f MatrixViewPort;//视口矩阵


	float mViewWidth, mViewHeight;//视口的宽高
	float mAspect;
	float mFov;
	float mZnear, mZfar;


	//摄像机作为组件，不具备位置和朝向信息
	//这些信息应该来源于该组件所在的游戏对象上的transform组件
	//vec3f mPosition;//摄像机的位置
	//vec3f mZ, mX, mY;

		//因为位置信息到了外部，所以留一个函数用于更摄像机的矩阵信息
	void UpdateCamera();

	void SetType(CAMERA_TYPE t);
	CAMERA_TYPE Type() { return mCamType; }


	void SetCamera(
		vec3f eye,
		vec3f at,
		vec3f up,
		int width,
		int height,
		float znear,
		float zfar,
		float aspect,
		float fov = 60.0f);

	//只给正交投影方式下使用
	void SetCameraSizeh(float h);

	Mat4x4f& GetVPV(Mat4x4f& vpv);

	const Frustum& GetWorldFrustum()const
	{
		return mWorldFrustum;
	}

private:
	void _SetPerspective();//设置透视投影
	void _SetOrthogonal();//设置正交投影

	void _CaclWorldFrustum();//计算世界坐标系下的视景体
private:
	//正交摄像机投影截面宽高
	float sizex;   
	float sizey;
	CAMERA_TYPE mCamType;
	Frustum mWorldFrustum;//世界的视景体
};


#endif