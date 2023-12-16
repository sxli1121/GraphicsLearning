#include "Camera.h"
#include "GameObject.h"
Camera* Camera::mainCamera = nullptr;

Camera::Camera(GameObject* pobj) :Component(pobj)
{
	mType = emComponentType::ComType_Camera;  // 设置父类中的组件类型
	mCamType = emCameraType::Camera_Perspective;
	sizex = 10;
	sizey = 10;


	mViewWidth = 800;
	mViewHeight = 600;
	mZnear = 0.3f;
	mZfar = 1000;   
	mAspect = 4 / 3.0f;
	mFov = 60.0f;

	// 使用transform 设置 Camera
	SetCamera(gameobject()->transform().position(),
		gameobject()->transform().position() + gameobject()->transform().forward(),
		gameobject()->transform().up(), mViewWidth, mViewHeight, mZnear, mZfar, mAspect, mFov);


}


void Camera::UpdateCamera()
{
	//先获取三个轴向
	vec3f mZ = mOwner->transform().forward();
	vec3f mX = mOwner->transform().right();
	vec3f mY = mOwner->transform().up();

	const vec3f& pos = mOwner->transform().position();

	//设置的摄像机矩阵
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

	//视口矩阵
	float wd2 = mViewWidth * 0.5f;
	float hd2 = mViewHeight * 0.5f;
	MatrixViewPort.Set(
		wd2, 0, 0, 0,
		0, -hd2, 0, 0,
		0, 0, 1, 0,
		wd2, hd2, 0, 1);//这里考虑的是铺满整个窗口，所以没有偏移左上角

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
	//获取所在游戏对象的transform组件，并且设置摄像机的位置
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
	//求半fov角的余切值
	//因为数学库中没有cot函数
	//但是cot函数和tan函数互为倒数，所以cot(fov/2) = 1/tan(fov/2);
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
	//在当前fov角下近截面的宽高 -- 计算从透视相机下 用fov 转过来的正交近截面
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
		4 ───5
		│╲   │╲
		│  0───1
		│  │ │ │
		7 --│- 6 │
		 ╲ │  ╲│
			3───2
		 近截面：0,1,2,3
		 远截面：4,5,6,7
		 假设6个面在左手坐标系下，每个面都朝外，每个面任选三个点顺时针构成平面
		 plane_near		1,2,3
		 plane_far		4,7,6
		 plane_left		0,3,7
		 plane_right	1,5,6
		 plane_up		5,1,0
		 plane_down		2,6,7
	 */

	const Transform& t = mOwner->transform(); // 获取的摄像机的空间信息
	//  拿到摄像机的手机 位置信息
	const vec3f& campos = t.position();
	const vec3f& axis_z = t.forward();
	const vec3f& axis_y = t.up();
	const vec3f& axis_x = t.right();

	vec3f nearz = campos + axis_z * mZnear;//近截面的中心世界位置
	float nearh = tan(D2R(mFov * 0.5f)) * mZnear;//一半近截面高度
	float nearw = mAspect * nearh;//一半近截面的宽度

	vec3f near_deltax = axis_x * nearw;
	vec3f near_deltay = axis_y * nearh;

	vec3f farz = campos + axis_z * mZfar;
	float farh = tan(D2R(mFov * 0.5f)) * mZfar;//一半远截面高度
	float farw = mAspect * farh;//一半远截面的宽度

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

	// 这里就是计算的世界中的视景体
}