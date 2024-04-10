#include "Scene.h"
#include <iostream>
#include <windows.h>
#include <functional>
#include "MemRenderBuffer.h"
#include "InputMgr.h"
#include "math3d/Matrix4x4.h"

#include "GameObject.h"
#include "Application.h"

GameObject g_Cam;
GameObject g_SkyBox;
GameObject g_Box;
GameObject g_RabbitVoxel;

bool Scene::Init()
{

	//MeshData test;
	//test.LoadFromObj(".\\res\\obj\\Rabbit\\Rabbit.obj");
	//test.Save2File(".\\res\\mesh\\Rabbit.txt","Rabbit_D.bmp");


	if (!g_SkyBox.LoadSkyBox("s1"))
	{
		printf("加载天空盒失败\n");
		system("pause");
	}

	//if (g_Box.LoadMesh(".\\res\\mesh\\newbox.txt"))
	//{
	//	g_Box.transform().SetPosition(vec3f::zero());
	//	g_Box.transform().SetRotation(vec3f::zero());
	//	g_Box.transform().SetScale(vec3f::one());
	//}
	//else
	//{
	//	printf("加载模型失败\n");
	//	system("pause");
	//	return false;
	//}

	//if (g_RabbitVoxel.LoadMesh(".\\res\\mesh\\Rabbit.txt")/*LoadObj(".\\res\\obj\\monu1\\monu1.obj")*/)
	//{
	//	g_RabbitVoxel.transform().SetPosition(vec3f::zero());
	//}
	//else
	//{
	//	printf("加载模型失败\n");
	//	system("pause");
	//	return false;
	//}

	//if (g_RabbitVoxel.LoadObj(".\\res\\obj\\Rabbit\\Rabbit.obj"))
	//{
	//	g_RabbitVoxel.transform().SetPosition(vec3f::zero());
	//}
	//else
	//{
	//	printf("加载模型失败\n");
	//	system("pause");
	//	return false;
	//}

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
			w, h, 0.3, 1000.0f,
			float(w) / h,
			60.0f);
	}

	return true;
}

float anglex = 0, angley = 0;
void Scene::Update(float dt)
{

	float speed = 10;
	if (Input.KeyDown('W'))
	{
		vec3f f = g_Cam.transform().forward();
		g_Cam.transform().translate(vec3f(f.x, 0, f.z).normalized() * dt * speed);
	}
	else if (Input.KeyDown('S'))
	{
		vec3f b = g_Cam.transform().back();
		g_Cam.transform().translate(vec3f(b.x, 0, b.z).normalized() * dt * speed);
	}

	if (Input.KeyDown('A'))
	{
		vec3f f = g_Cam.transform().left();
		g_Cam.transform().translate(vec3f(f.x, 0, f.z).normalized() * dt * speed);
	}
	else if (Input.KeyDown('D'))
	{
		vec3f b = g_Cam.transform().right();
		g_Cam.transform().translate(vec3f(b.x, 0, b.z).normalized() * dt * speed);
	}

	if (Input.KeyDown(VK_LEFT))
	{
		angley -= dt * speed * 3;
		g_Cam.transform().SetRotation(vec3f(0, 0, 0));
		g_Cam.transform().rotate(vec3f(anglex, 0, 0));
		g_Cam.transform().rotate(vec3f(0, angley, 0));
	}
	else if (Input.KeyDown(VK_RIGHT))
	{
		angley += dt * speed * 3;
		g_Cam.transform().SetRotation(vec3f(0, 0, 0));
		g_Cam.transform().rotate(vec3f(anglex, 0, 0));
		g_Cam.transform().rotate(vec3f(0, angley, 0));
	}

	if (Input.KeyDown(VK_UP))
	{
		anglex -= dt * 10;
		g_Cam.transform().SetRotation(vec3f(0, 0, 0));
		g_Cam.transform().rotate(vec3f(anglex, 0, 0));
		g_Cam.transform().rotate(vec3f(0, angley, 0));
	}
	else if (Input.KeyDown(VK_DOWN))
	{
		anglex += dt * 10;
		g_Cam.transform().SetRotation(vec3f(0, 0, 0));
		g_Cam.transform().rotate(vec3f(anglex, 0, 0));
		g_Cam.transform().rotate(vec3f(0, angley, 0));
	}

	g_SkyBox.OnUpdate();
	g_Cam.OnUpdate();
	g_Box.OnUpdate();
	//g_RabbitVoxel.OnUpdate();
}

void Scene::Render(float dt)
{
	g_Box.OnRender();
	g_SkyBox.OnRender();
	//g_RabbitVoxel.OnRender();
}

Scene::~Scene()
{

}
