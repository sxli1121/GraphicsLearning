#pragma once
#ifndef __MESH_DATA_3D_H__
#define __MESH_DATA_3D_H__
#include "SingletonTemplate.h"
#include <map>
#include "math3d/Vector3.h"
#include "math3d/Vector2.h"
using namespace Math;
//ģ��������
typedef class MeshData
{
public:
	MeshData();
	~MeshData();
	MeshData(const MeshData& that);
	MeshData& operator = (const MeshData& that);

	//���ļ�����3dģ��
	bool LoadFromFile(const char* meshfile);

	//���ػҶ�ͼ --���ػҶ�ģ��
	bool LoadGrayMap(const char* hm,
		float unitsize,//��λ��С��һ�����ӵ����ؿ�ߣ�������֮��ļ������
		float _floor,//��С�߶�
		float _top);
	
protected:
	void _Clear();
	void _CalcBoundingBox();
	void _Copy(const MeshData& that);
public:
	int mVertextCount;//���������
	int mIndexCount;//����������
	//����ģ����������������������λ��Ƕ���Σ��������ն�ת��Ϊ���������ݽ��д���
	int mTriangleCount;//�����εĸ���
	vec3f* mPVertexts;//ָ��Ķ�������
	int* mPIndex;//ָ����������

	vec2f* mPUVs; // 
	class MyTexture2D* mPTexture;  // ���ģ��ʹ�õ�����


	// ��������ϵ����İ�Χ��
	vec3f mBoundingBox[8];
}MeshData, * LPMESHDATA;





class MeshManager :public Singleton<MeshManager>
{
	PRIVATE_CONSTRUCTOR_AND_COPY(MeshManager)
public:
	//������ھͷ��أ����ܴ��ھͳ���ͨ���ļ�����
	LPMESHDATA GetMesh(const char* meshfile)
	{
		auto itr = mMeshData.find(meshfile);
		if (itr != mMeshData.end())
		{
			return itr->second;
		}
		LPMESHDATA pdata = new MeshData;
		if (pdata == nullptr)
		{
			return nullptr;
		}
		if (pdata->LoadFromFile(meshfile))
		{
			mMeshData[meshfile] = pdata;
			return pdata;
		}
		delete pdata;
		return nullptr;
	}

	~MeshManager()
	{
		if (!mMeshData.empty())
		{
			for (auto& i : mMeshData)
			{
				delete i.second;
			}
			mMeshData.clear();
		}
	}

private:
	//���е�ģ�����ݶ��洢�����map��
	std::map<std::string, LPMESHDATA> mMeshData;
};
#endif