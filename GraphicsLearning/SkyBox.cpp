#include "SkyBox.h"
#include "GameObject.h"
#include <windows.h>
#include <direct.h> // _getcwd
#include <io.h>
#include <string>
#include "MyTexture2D.h"
#include "RenderList.h"
#include "./math3d/Matrix4x4.h"

bool SkyBox::LoadSkyBox(const char* path)
{
	char curpath[MAX_PATH] = {};
	if (!_getcwd(curpath, MAX_PATH))
	{
		printf("获取当前路径失败\n");
		return false;
	}
	printf("当前工作路径：%s\n", curpath);

	std::string skyboxpath = curpath;
	skyboxpath = skyboxpath + "\\res\\skybox\\";
	skyboxpath = skyboxpath + path;
	skyboxpath = skyboxpath + "\\";
	std::string infofile = skyboxpath + path;
	infofile = infofile + ".txt";

	if ((_access(skyboxpath.c_str(), 0)) == -1)
	{
		printf("天空盒配置文件路径不存在：%s\n", skyboxpath.c_str());
		return false;
	}
	printf("当前使用天空盒配置文件路径：%s\n", skyboxpath.c_str());

	char box_bmps[6][MAX_PATH] = {};
	GetPrivateProfileStringA("skyinfo", "front", "", box_bmps[0], MAX_PATH, infofile.c_str());
	GetPrivateProfileStringA("skyinfo", "back", "", box_bmps[1], MAX_PATH, infofile.c_str());
	GetPrivateProfileStringA("skyinfo", "left", "", box_bmps[2], MAX_PATH, infofile.c_str());
	GetPrivateProfileStringA("skyinfo", "right", "", box_bmps[3], MAX_PATH, infofile.c_str());
	GetPrivateProfileStringA("skyinfo", "top", "", box_bmps[4], MAX_PATH, infofile.c_str());
	GetPrivateProfileStringA("skyinfo", "bottom", "", box_bmps[5], MAX_PATH, infofile.c_str());

	std::string curbmp;
	for (int i = 0; i < 6; ++i)
	{
		curbmp = skyboxpath + box_bmps[i];
		if (_access(curbmp.c_str(), 0) == -1)
		{
			printf("天空盒文件缺失:%s\n", curbmp.c_str());
			return false;
		}
		mSkyTexture[i] = TextureManager::Instance().LoadTexture(curbmp.c_str());
		if (mSkyTexture[i] == nullptr)
		{
			printf("天空盒文件加载失败:%s\n", curbmp.c_str());
			return false;
		}
	}
	printf("天空盒数据加载成功\n");
	return true;
}

void SkyBox::CalculateWorldVertexts()
{
	//主要同步天空盒的位置到摄像机的位置
	Transform& CamTrans = Camera::mainCamera->gameobject()->transform();
	Transform& transform = gameobject()->transform();
	transform.SetPosition(CamTrans.position());
	Mat4x4f matworld = transform.MatS() * transform.MatR() * transform.MatT();
	for (int i = 0; i < 36; ++i)
	{
		mPVertexts[i] = mPModelVertexts[i] * matworld;
	}
}

void SkyBox::DrawSkyBox()
{
	const vec3f& campos = Camera::mainCamera->gameobject()->transform().position();
	const Frustum& frustum = Camera::mainCamera->GetWorldFrustum();//视景体
	vec3f dir;
	for (int i = 0; i < 12; ++i)
	{
		Triangle& t = SRL.GetNextTriangle();
		t.p1 = mPVertexts[mIndexs[i * 3]];
		t.p2 = mPVertexts[mIndexs[i * 3 + 1]];
		t.p3 = mPVertexts[mIndexs[i * 3 + 2]];
		t.uv1 = mPUVs[mIndexs[i * 3]];
		t.uv2 = mPUVs[mIndexs[i * 3 + 1]];
		t.uv3 = mPUVs[mIndexs[i * 3 + 2]];
		t.ptexture = mSkyTexture[i / 2];

		//当三角形在视景体内或者和视景体有相交(至少有一个点在内)
		//if (frustum.TriangleInFrustum(t))
		{
			//摄像机看向三角形的方向向量
			dir = t.p1 - campos;
			if (t.CheckFront(dir))
			{
				SRL.AddTriangle();
			}
		}
	}

}
