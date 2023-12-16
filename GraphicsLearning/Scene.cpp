#include "Scene.h"
#include <iostream>
#include <windows.h>
#include "MemRenderBuffer.h"
#include "InputMgr.h"
#include "math3d/Matrix4x4.h"

#include "GameObject.h"
#include "Application.h"

GameObject g_Box;
GameObject g_Cam;

bool Scene::Init()
{
	bool isok = g_Box.LoadMesh(".\\res\\mesh\\box.txt");
	if (!isok)
	{
		printf("读取失败\n");
		return false;
	}
	else
	{
		printf("读取成功\n");
		g_Box.transform().SetPosition(vec3f::zero());
		g_Box.transform().SetRotation(vec3f::zero());
		g_Box.transform().SetScale(vec3f::one());
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
	g_Box.transform().rotate(vec3f::up() * dt * 50);
	//g_Cam.transform().rotate(vec3f::up() * dt * 50);
	if (Input.KeyDown(VK_UP))
	{
		g_Cam.transform().translate(vec3f::up() * dt * 30);
		g_Cam.transform().lookat(vec3f::zero());
	}
	else if (Input.KeyDown(VK_DOWN))
	{
		g_Cam.transform().translate(vec3f::down() * dt * 30);
		g_Cam.transform().lookat(vec3f::zero());
	}

	vec3f pos = g_Cam.transform().position();
	pos.y = sin(t += dt)*3;
	g_Cam.transform().SetPosition(pos);
	g_Cam.transform().lookat(vec3f::zero());


	g_Box.OnUpdate();
	g_Cam.OnUpdate();

}

void Scene::Render(float dt)
{
	g_Box.OnRender();
}

Scene::~Scene()
{

}
