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
		printf("��ȡ��ǰ·��ʧ��\n");
		return false;
	}
	printf("��ǰ����·����%s\n", curpath);

	std::string skyboxpath = curpath;
	skyboxpath = skyboxpath + "\\res\\skybox\\";
	skyboxpath = skyboxpath + path;
	skyboxpath = skyboxpath + "\\";
	std::string infofile = skyboxpath + path;
	infofile = infofile + ".txt";

	if ((_access(skyboxpath.c_str(), 0)) == -1)
	{
		printf("��պ������ļ�·�������ڣ�%s\n", skyboxpath.c_str());
		return false;
	}
	printf("��ǰʹ����պ������ļ�·����%s\n", skyboxpath.c_str());

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
			printf("��պ��ļ�ȱʧ:%s\n", curbmp.c_str());
			return false;
		}
		mSkyTexture[i] = TextureManager::Instance().LoadTexture(curbmp.c_str());
		if (mSkyTexture[i] == nullptr)
		{
			printf("��պ��ļ�����ʧ��:%s\n", curbmp.c_str());
			return false;
		}
	}
	printf("��պ����ݼ��سɹ�\n");
	return true;
}

void SkyBox::CalculateWorldVertexts()
{
	//��Ҫͬ����պе�λ�õ��������λ��
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
	const Frustum& frustum = Camera::mainCamera->GetWorldFrustum();//�Ӿ���
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

		//�����������Ӿ����ڻ��ߺ��Ӿ������ཻ(������һ��������)
		//if (frustum.TriangleInFrustum(t))
		{
			//��������������εķ�������
			dir = t.p1 - campos;
			if (t.CheckFront(dir))
			{
				SRL.AddTriangle();
			}
		}
	}

}
