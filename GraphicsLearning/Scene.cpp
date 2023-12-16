#include "Scene.h"
#include <iostream>
#include <windows.h>
#include "MemRenderBuffer.h"
#include "InputMgr.h"
#include "math3d/Matrix4x4.h"

#include "GameObject.h"
#include "Application.h"

GameObject g_TriPyramid;
GameObject g_Cam;

bool Scene::Init()
{
	bool isok = g_TriPyramid.LoadMesh(".\\res\\mesh\\tripyramid.txt");
	if (!isok)
	{
		printf("读取失败\n");
		return false;
	}
	else
	{
		printf("读取成功\n");
		g_TriPyramid.transform().SetPosition(vec3f::zero());
		g_TriPyramid.transform().SetRotation(vec3f::zero());
		g_TriPyramid.transform().SetScale(vec3f::one());
	}

	POINT s = App.WndSize();
	int w = s.x;
	int h = s.y;
	Camera* pcam = (Camera*)g_Cam.AddComponent(emComponentType::ComType_Camera);
	if (pcam != nullptr)
	{
		pcam->SetCamera(
			vec3f(0, 0, -10),
			vec3f::zero(),
			vec3f::up(),
			w, h, 4, 50.0f,
			float(w) / h,
			60.0f);
	}
	return true;
}

float t = 0;
void Scene::Update(float dt)
{

	if (Input.KeyDown(VK_UP))
	{
		g_Cam.transform().translate(vec3f::forward() * dt * 10);
	}
	else if (Input.KeyDown(VK_DOWN))
	{
		g_Cam.transform().translate(vec3f::back() * dt * 10);
	}
	else if (Input.KeyDown(VK_PRIOR))//pageup
	{
		g_Cam.transform().translate(vec3f::up() * dt * 10);
	}
	else if (Input.KeyDown(VK_NEXT))//pagedown
	{
		g_Cam.transform().translate(vec3f::down() * dt * 10);
	}

	g_TriPyramid.OnUpdate();
	g_Cam.OnUpdate();

}

void Scene::Render(float dt)
{
	g_TriPyramid.OnRender();
}

Scene::~Scene()
{

}
