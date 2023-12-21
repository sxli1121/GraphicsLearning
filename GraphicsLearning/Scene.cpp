#include "Scene.h"
#include <iostream>
#include <windows.h>
#include <functional>
#include "MemRenderBuffer.h"
#include "InputMgr.h"
#include "math3d/Matrix4x4.h"

#include "GameObject.h"
#include "Application.h"

GameObject g_TriPyramid;
GameObject g_Cam;
GameObject g_Plane;
GameObject g_Plane2;

bool Scene::Init()
{
	g_Plane2.LoadMesh(".\\res\\mesh\\plane_uv.txt");
	g_Plane2.transform().SetPosition(vec3f::zero());
	g_Plane2.transform().SetRotation(vec3f::zero());
	g_Plane2.transform().SetScale(vec3f::one());
	g_Plane2.transform().rotate(vec3f::forward() * 45.f);

	bool isok = g_Plane.LoadMesh(".\\res\\mesh\\plane_uv.txt");
	if (!isok)
	{
		printf("读取失败\n");
		return false;
	}
	else
	{
		printf("读取成功\n");
		g_Plane.transform().SetPosition(vec3f::zero());
		g_Plane.transform().SetRotation(vec3f::zero());
		g_Plane.transform().SetScale(vec3f::one());
	}

	POINT s = App.WndSize();
	int w = s.x;
	int h = s.y;
	Camera* pcam = (Camera*)g_Cam.AddComponent(emComponentType::ComType_Camera);
	if (pcam != nullptr)
	{
		pcam->SetCamera(
			vec3f(0, 5, -10),
			vec3f::zero(),
			vec3f::up(),
			w, h, 0.3f, 1000.0f,
			float(w) / h,
			60.0f);
	}
	return true;
}

float t = 0;
void Scene::Update(float dt)
{
	g_Plane.transform().rotate(vec3f::up() * dt * 20);
	if (Input.KeyDown(VK_UP))
	{
		g_Cam.transform().translate(g_Cam.transform().forward() * dt * 10);
		g_Cam.transform().lookat(vec3f::zero());
	}
	else if (Input.KeyDown(VK_DOWN))
	{
		g_Cam.transform().translate(g_Cam.transform().back() * dt * 10);
		g_Cam.transform().lookat(vec3f::zero());
	}
	else if (Input.KeyDown(VK_PRIOR))
	{
		g_Cam.transform().translate(vec3f::up() * dt * 10);
		g_Cam.transform().lookat(vec3f::zero());
	}
	else if (Input.KeyDown(VK_NEXT))
	{
		g_Cam.transform().translate(vec3f::down() * dt * 10);
		g_Cam.transform().lookat(vec3f::zero());
	}

	g_Plane.OnUpdate();
	g_Plane2.OnUpdate();
	g_Cam.OnUpdate();
}

void Scene::Render(float dt)
{
	g_Plane2.OnRender();
	g_Plane.OnRender();
}

Scene::~Scene()
{

}
