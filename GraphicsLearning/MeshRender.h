#pragma once
#ifndef __COMPONENT_MESHRENDER_H__
#define __COMPONENT_MESHRENDER_H__
#include "Component.h"
#include "MeshData.h"
#include "math3d/Vector3.h"

class MeshRender :public Component
{
public:
	MeshRender(GameObject* pobj) :Component(pobj)
	{
		mType = emComponentType::ComType_MeshRender;
		mPMeshData = nullptr;
		mPVertexts = nullptr;
	}

	//设置、切换模型
	bool SetMesh(LPMESHDATA pMesh);

	//计算世界中的顶点信息
	void CalculateWorldVertexts();   // model 矩阵的数据

	//模型绘制
	void DrawMesh();
protected:
private:
	LPMESHDATA mPMeshData;//指向所拥有的原始模型数据的指针
	vec3f* mPVertexts;//存储世界变换以后的顶点数组 = 原始模型数据 * 世界变换矩阵

	vec3f mBoundingBox[8]; // 储存世界中的包围盒
};
#endif