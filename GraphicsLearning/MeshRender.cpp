﻿#include "GameObject.h"
#include "RenderList.h"
#include "Geometric.h"
#include "math3d/Matrix4x4.h"
using namespace Math;
#include "UtilityHelper.h"

bool MeshRender::SetMesh(LPMESHDATA pMesh)
{
	if (pMesh == nullptr)
	{
		return false;
	}
	if (mPMeshData == pMesh)
	{
		return true;
	}
	mPMeshData = pMesh;
	SAFE_DELETE_ARRAY_PTR(mPVertexts);
	mPVertexts = new vec3f[mPMeshData->mVertextCount];
	//重新计算世界中的顶点数据
	CalculateWorldVertexts();
	return true;
}

void MeshRender::CalculateWorldVertexts()
{
	if (mPMeshData == nullptr)
	{
		return;
	}
	Transform& transform = gameobject()->transform();   // 和这个MeshRender 一起在的Transform
	Mat4x4f matworld = transform.MatS()*transform.MatR()*transform.MatT();   // 拿到当前模型的model 矩阵

	//获取顶点数量
	int& vertextCount = mPMeshData->mVertextCount;   // 顶点数量
	vec3f*& pVertexts = mPMeshData->mPVertexts;     // 顶点数据
	for (int i=0;i<vertextCount;++i)
	{
		mPVertexts[i] = pVertexts[i] * matworld;    // 所有的顶点乘 model 矩阵
	}

	// 重新计算世界中的包围盒的数据
	for (int i = 0; i < 8; i++)
	{
		mBoundingBox[i] = mPMeshData->mBoundingBox[i] * matworld;
	}
}

void MeshRender::DrawMesh()
{
	//在这里不是直接的绘制所有的模型，而是将要绘制的信息
	//传递到一个总的绘制管理的角色中，这里我们叫它渲染列表
	//渲染列表中收集了所有当前帧需要绘制的三角形信息
	//我们可以在一帧中绘制的时候一次性的将所有需要绘制的信息集中绘制
	Mat4x4f vpv;
	Camera::mainCamera->GetVPV(vpv);   // 拿到当前的摄像机 投影视口矩阵

	// 在绘制的时候 拿到摄像机的位置
	const vec3f& campos = Camera::mainCamera->gameobject()->transform().position();


	int& tc = mPMeshData->mTriangleCount;  // 当前模型的三角形数量
	int*& pindex = mPMeshData->mPIndex;    // 当前模型的索引


	// 得到视景体
	const Frustum& frustum = Camera::mainCamera->GetWorldFrustum();
	vec3f dir;

	for (int j = 0; j < 8; j++)
	{
		if (frustum.PtInFrustum(mBoundingBox[j]))  // 判断包围盒的顶点是否在视景体里面(只要有一个在里面就绘制)
		{
			for (int i = 0; i < tc; i++)
			{
				//获取当前可用三角形的引用
				Triangle& t = RL.GetNextTriangle();
				//修改对应的三角形数据
				t.p1 = mPVertexts[pindex[i * 3]];
				t.p2 = mPVertexts[pindex[i * 3 + 1]];
				t.p3 = mPVertexts[pindex[i * 3 + 2]];

				// 背面拣选
				// 从摄像机看向三角形的方向向量
				dir = t.p1 - campos;  // 摄像机看向三角形的任意一点的向量 与 
				if (t.CheckFront(dir))  
				{
					// 通过背面拣选之后在计算矩阵--进行绘制
					t.p1 = t.p1 * vpv;
					t.p2 = t.p2 * vpv;
					t.p3 = t.p3 * vpv;

					//移动下标到下一个三角形
					RL.AddTriangle();
				}
			}
			break;
		}
	}
}

// 视景体裁剪(世界中)
// 背面拣选(世界中)
