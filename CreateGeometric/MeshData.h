#pragma once
#ifndef __MESH_DATA_3D_H__
#define __MESH_DATA_3D_H__
#include "vector3d.h"
#include "SingletonTemplate.h"
#include <map>
#include <string>
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
	//将模型数据保存为文件
	bool Save2File(const char* meshfile);


	//创建几何体
	//平面
	void CreatePlane(size_t size_x = 1,//平面的x方向大小
		size_t size_y = 1,//平面的y方向大小
		size_t precision = 1//精度,表示的是x和y方向的分割数量
	);
	//立方体
	void CreateCube(size_t size = 1);
	//圆柱体
	void CreateCylinder(float radius ,float height,size_t side = 12);

	//复杂的图形三选一
	//胶囊体
	void CreateCapsule(float radius, 
		float height,//胶囊体不算两端半球的高度
		size_t precision = 4/*使用挤压法创建胶囊体的两端，最少精度4，且必须为2的倍数*/);
	//球体
	//通过正四面体裂变
	void CreateSphere_s4(float r, int level = 4);//半径和裂变次数
	//通过正六面体裂变
	void CreateSphere_s6(float r, int samplerate = 4);
	//通过正八面体裂变
	void CreateSphere_s8(float r, int level = 4);//半径和裂变次数
	//正十二面体
	void CreateSphere_s12(float r, int level = 4);
	//正20面体
	void CreateSphere_s20(float r, int level = 4);

	//通过UV方式创建球形
	void CreateSphere_uv(float r,
		int s1,//s1表示纬度精度，至少为2
		int s2);//s2表示经度精度，至少为3

	//环形
	void CreateRing(
		float radius1,//小圆半径
		float radius2,//大圆半径
		size_t precision1 = 8,//小圆精度
		size_t precision2 = 16);//大圆精度
	
	//简单的图形三选二
	//五角星
	void CreateStar(float radius1,float radius2,float height,size_t side = 5);	
	//钻石
	void CreateDiamond(float radius1, float radius2, float height1, float height2, size_t side = 12);
	//正椎体
	void CreatePyramid(float radius,float height,size_t side = 12);
	//......
protected:
	void _Clear();
	void _Copy(const MeshData& that);
	void _CalcBoundingBox();
	void _InvNormal();//反转法线，实际上是靠反转三角形索引顺序实现
public:
	int mVertextCount;//顶点的数量
	int mIndexCount;//索引的数量
	//不管模型中描述面的数据是三角形还是多边形，我们最终都转化为三角形数据进行处理
	int mTriangleCount;//三角形的个数
	vector3d* mPVertexts;//指向的顶点数据
	int* mPIndex;//指向索引数据

	//本地坐标系下的包围盒
	vector3d mBoundingBox[8];
}MeshData,*LPMESHDATA;

class MeshManager :public Singleton<MeshManager>
{
	PRIVATE_COPY_AND_CONSTRUCTOR(MeshManager)
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
	std::map<std::string,LPMESHDATA> mMeshData;
};
#endif