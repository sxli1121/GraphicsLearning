#include "Camera.h"
#include "GameObject.h"
Camera* Camera::mainCamera = nullptr;

Camera::Camera(GameObject* pobj) :Component(pobj)
{
	mType = emComponentType::ComType_Camera;  // ���ø����е��������
	mCamType = emCameraType::Camera_Perspective;
	sizex = 10;
	sizey = 10;


	mViewWidth = 800;
	mViewHeight = 600;
	mZnear = 0.3f;
	mZfar = 1000;   
	mAspect = 4 / 3.0f;
	mFov = 60.0f;

	// ʹ��transform ���� Camera
	SetCamera(gameobject()->transform().position(),
		gameobject()->transform().position() + gameobject()->transform().forward(),
		gameobject()->transform().up(), mViewWidth, mViewHeight, mZnear, mZfar, mAspect, mFov);


}


void Camera::UpdateCamera()
{
	//�Ȼ�ȡ��������
	vec3f mZ = mOwner->transform().forward();
	vec3f mX = mOwner->transform().right();
	vec3f mY = mOwner->transform().up();

	const vec3f& pos = mOwner->transform().position();

	//���õ����������
	MatrixView.Set(
		mX.x, mY.x, mZ.x, 0,
		mX.y, mY.y, mZ.y, 0,
		mX.z, mY.z, mZ.z, 0,
		-pos.Dot(mX), -pos.Dot(mY), -pos.Dot(mZ), 1);

	if (mCamType == CAMERA_TYPE::Camera_Perspective)
	{
		_SetPerspective();
	}
	else
	{
		_SetOrthogonal();
	}

	//�ӿھ���
	float wd2 = mViewWidth * 0.5f;
	float hd2 = mViewHeight * 0.5f;
	MatrixViewPort.Set(
		wd2, 0, 0, 0,
		0, -hd2, 0, 0,
		0, 0, 1, 0,
		wd2, hd2, 0, 1);//���￼�ǵ��������������ڣ�����û��ƫ�����Ͻ�

	_CaclWorldFrustum();
}



void Camera::SetType(CAMERA_TYPE t)
{
	if (mCamType != t)
	{
		mCamType = t;
		if (mCamType == CAMERA_TYPE::Camera_Perspective)
		{
			_SetPerspective();
		}
		else
		{
			_SetOrthogonal();
		}
	}
}

void Camera::SetCamera(vec3f eye, vec3f at, vec3f up, int width, int height, float znear, float zfar, float aspect, float fov)
{
	mAspect = aspect;
	mViewWidth = width;
	mViewHeight = height;
	mZnear = znear;
	mZfar = zfar;
	mFov = fov;
	//mPosition = eye;
	//��ȡ������Ϸ�����transform��������������������λ��
	mOwner->transform().SetPosition(eye);
	mOwner->transform().lookat(at, &up);
	UpdateCamera();
	mainCamera = this;
}

void Camera::SetCameraSizeh(float h)
{
	if (mCamType == CAMERA_TYPE::Camera_Orthogonal)
	{
		sizey = abs(h);
		if (sizey < 1.0f)
		{
			sizey = 1.0f;
		}
		sizex = sizey * mAspect;
		MatrixProjection.Set(
			2 / sizex, 0, 0, 0,
			0, 2 / sizey, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
}

Mat4x4f& Camera::GetVPV(Mat4x4f& vpv)
{
	vpv = MatrixView * MatrixProjection * MatrixViewPort;
	return vpv;
}

void Camera::_SetPerspective()
{
	//���fov�ǵ�����ֵ
	//��Ϊ��ѧ����û��cot����
	//����cot������tan������Ϊ����������cot(fov/2) = 1/tan(fov/2);
	float cot_fov_d2 = 1 / tan(D2R(mFov) * 0.5f);
	float fdfsubn = mZfar / (mZfar - mZnear);
	MatrixProjection.Set(
		cot_fov_d2 / mAspect, 0, 0, 0,
		0, cot_fov_d2, 0, 0,
		0, 0, fdfsubn, 1,
		0, 0, -fdfsubn * mZnear, 0);
}

void Camera::_SetOrthogonal()
{
	//�ڵ�ǰfov���½�����Ŀ�� -- �����͸������� ��fov ת����������������
	//sizey = tan(D2R(mFov) * 0.5f) * mZnear * 2;
	sizex = sizey * mAspect;

	MatrixProjection.Set(
		2 / sizex, 0, 0, 0,
		0, 2 / sizey, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

void Camera::_CaclWorldFrustum()
{
	/*
		4 ������5
		���v   ���v
		��  0������1
		��  �� �� ��
		7 --��- 6 ��
		 �v ��  �v��
			3������2
		 �����棺0,1,2,3
		 Զ���棺4,5,6,7
		 ����6��������������ϵ�£�ÿ���涼���⣬ÿ������ѡ������˳ʱ�빹��ƽ��
		 plane_near		1,2,3
		 plane_far		4,7,6
		 plane_left		0,3,7
		 plane_right	1,5,6
		 plane_up		5,1,0
		 plane_down		2,6,7
	 */

	const Transform& t = mOwner->transform(); // ��ȡ��������Ŀռ���Ϣ
	//  �õ���������ֻ� λ����Ϣ
	const vec3f& campos = t.position();
	const vec3f& axis_z = t.forward();
	const vec3f& axis_y = t.up();
	const vec3f& axis_x = t.right();

	vec3f nearz = campos + axis_z * mZnear;//���������������λ��
	float nearh = tan(D2R(mFov * 0.5f)) * mZnear;//һ�������߶�
	float nearw = mAspect * nearh;//һ�������Ŀ��

	vec3f near_deltax = axis_x * nearw;
	vec3f near_deltay = axis_y * nearh;

	vec3f farz = campos + axis_z * mZfar;
	float farh = tan(D2R(mFov * 0.5f)) * mZfar;//һ��Զ����߶�
	float farw = mAspect * farh;//һ��Զ����Ŀ��

	vec3f far_deltax = axis_x * farw;
	vec3f far_deltay = axis_y * farh;

	vec3f point[8] = {};
	point[0] = nearz - near_deltax + near_deltay;
	point[1] = nearz + near_deltax + near_deltay;
	point[2] = nearz + near_deltax - near_deltay;
	point[3] = nearz - near_deltax - near_deltay;
	point[4] = farz - far_deltax + far_deltay;
	point[5] = farz + far_deltax + far_deltay;
	point[6] = farz + far_deltax - far_deltay;
	point[7] = farz - far_deltax - far_deltay;

	/*
		 plane_near		1,2,3
		 plane_far		4,7,6
		 plane_left		0,3,7
		 plane_right	1,5,6
		 plane_up		5,1,0
		 plane_down		2,6,7
	*/
	mWorldFrustum.plane_near.Create3P(point[1], point[2], point[3]);
	mWorldFrustum.plane_far.Create3P(point[4], point[7], point[6]);
	mWorldFrustum.plane_left.Create3P(point[0], point[3], point[7]);
	mWorldFrustum.plane_right.Create3P(point[1], point[5], point[6]);
	mWorldFrustum.plane_up.Create3P(point[5], point[1], point[0]);
	mWorldFrustum.plane_down.Create3P(point[2], point[6], point[7]);

	// ������Ǽ���������е��Ӿ���
}