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
	Camera(GameObject* pobj);//��Ա��ʼ���б��츸��

	Mat4x4f MatrixView;//������任����
	Mat4x4f MatrixProjection;//ͶӰ����
	Mat4x4f MatrixViewPort;//�ӿھ���


	float mViewWidth, mViewHeight;//�ӿڵĿ��
	float mAspect;
	float mFov;
	float mZnear, mZfar;


	//�������Ϊ��������߱�λ�úͳ�����Ϣ
	//��Щ��ϢӦ����Դ�ڸ�������ڵ���Ϸ�����ϵ�transform���
	//vec3f mPosition;//�������λ��
	//vec3f mZ, mX, mY;

		//��Ϊλ����Ϣ�����ⲿ��������һ���������ڸ�������ľ�����Ϣ
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

	//ֻ������ͶӰ��ʽ��ʹ��
	void SetCameraSizeh(float h);

	Mat4x4f& GetVPV(Mat4x4f& vpv);

	const Frustum& GetWorldFrustum()const
	{
		return mWorldFrustum;
	}

private:
	void _SetPerspective();//����͸��ͶӰ
	void _SetOrthogonal();//��������ͶӰ

	void _CaclWorldFrustum();//������������ϵ�µ��Ӿ���
private:
	//���������ͶӰ������
	float sizex;   
	float sizey;
	CAMERA_TYPE mCamType;
	Frustum mWorldFrustum;//������Ӿ���
};


#endif