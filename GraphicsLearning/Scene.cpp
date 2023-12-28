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

GameObject g_touzi10p;
GameObject g_touzi14p;

GameObject g_Terrain;

bool Scene::Init()
{
	bool tok = g_Terrain.CreateTerrainByHeightMap(".\\res\\heightmap\\257x257.bmp", 0.5, 0, 20);
	if (tok)
	{
		printf("地形加载成功\n");
	}
	else
	{
		printf("地形加载失败\n");
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
			w, h, 0.3f, 1000.0f,
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
		g_Cam.transform().translate(g_Cam.transform().forward() * dt * 10);
		//g_Cam.transform().lookat(vec3f::zero());
	}
	else if (Input.KeyDown(VK_DOWN))
	{
		g_Cam.transform().translate(g_Cam.transform().back() * dt * 10);
		//g_Cam.transform().lookat(vec3f::zero());
	}
	else if (Input.KeyDown(VK_LEFT))
	{
		g_Cam.transform().translate(g_Cam.transform().left() * dt * 10);
		//g_Cam.transform().lookat(vector3d::Zero());
	}
	else if (Input.KeyDown(VK_RIGHT))
	{
		g_Cam.transform().translate(g_Cam.transform().right() * dt * 10);
		//g_Cam.transform().lookat(vector3d::Zero());
	}
	else if (Input.KeyDown(VK_PRIOR))
	{
		g_Cam.transform().translate(vec3f::up() * dt * 10);
		//g_Cam.transform().lookat(vec3f::zero());
	}
	else if (Input.KeyDown(VK_NEXT))
	{
		g_Cam.transform().translate(vec3f::down() * dt * 10);
		//g_Cam.transform().lookat(vec3f::zero());
	}

	g_Terrain.OnUpdate();
	g_Cam.OnUpdate();
}

void Scene::Render(float dt)
{
	g_Terrain.OnRender();

}

Scene::~Scene()
{

}
