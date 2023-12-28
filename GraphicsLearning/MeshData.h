#pragma once
#ifndef __MESH_DATA_3D_H__
#define __MESH_DATA_3D_H__
#include "SingletonTemplate.h"
#include <map>
#include "math3d/Vector3.h"
#include "math3d/Vector2.h"
using namespace Math;
//模型数据类
typedef class MeshData
{
public:
	MeshData();
	~MeshData();
	MeshData(const MeshData& that);
	MeshData& operator = (const MeshData& that);

	//从文件加载3d模型
	bool LoadFromFile(const char* meshfile);

	//加载灰度图 --加载灰度模型
	bool LoadGrayMap(const char* hm,
		float unitsize,//单位大小，一个格子的像素宽高，两个点之间的间隔距离
		float _floor,//最小高度
		float _top);
	
protected:
	void _Clear();
	void _CalcBoundingBox();
	void _Copy(const MeshData& that);
public:
	int mVertextCount;//顶点的数量
	int mIndexCount;//索引的数量
	//不管模型中描述面的数据是三角形还是多边形，我们最终都转化为三角形数据进行处理
	int mTriangleCount;//三角形的个数
	vec3f* mPVertexts;//指向的顶点数据
	int* mPIndex;//指向索引数据

	vec2f* mPUVs; // 
	class MyTexture2D* mPTexture;  // 这个模型使用的纹理


	// 本地坐标系下面的包围盒
	vec3f mBoundingBox[8];
}MeshData, * LPMESHDATA;





class MeshManager :public Singleton<MeshManager>
{
	PRIVATE_CONSTRUCTOR_AND_COPY(MeshManager)
public:
	//如果存在就返回，不能存在就尝试通过文件加载
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
	//所有的模型数据都存储到这个map中
	std::map<std::string, LPMESHDATA> mMeshData;
};
#endif