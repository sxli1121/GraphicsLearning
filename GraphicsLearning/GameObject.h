#pragma once
#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__
#include "Camera.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Terrain.h"

#include <vector>

class GameObject
{
public:
	GameObject()
	{
		mpTransform = (Transform*)AddComponent(COMTYPE::ComType_Transform);
	}
	~GameObject()
	{
		//将创建的组件释放
		if (!mComponents.empty())
		{
			for (auto& c:mComponents)
			{
				delete c;
			}
			mComponents.clear();
		}
		if (mpTransform != nullptr)
		{
			delete mpTransform;
			mpTransform = nullptr;
		}
	}

	//增加组件的功能
	LPCOMPONENT AddComponent(COMTYPE t)
	{
		switch (t)
		{
		case COMTYPE::ComType_Transform:
		{
			if (mpTransform != nullptr)
			{
				return mpTransform;
			}
			mpTransform = new Transform(this);
			return mpTransform;
		}break;
		case COMTYPE::ComType_Camera:
		{
			Camera* pCam = (Camera*)GetComponent(COMTYPE::ComType_Camera);
			if (pCam!= nullptr)
			{
				return pCam;
			}
			pCam = new Camera(this);
			mComponents.push_back(pCam);
			return pCam;
		}break;
		case COMTYPE::ComType_MeshRender:
		{
			MeshRender* pMR = (MeshRender*)GetComponent(COMTYPE::ComType_MeshRender);
			if (pMR != nullptr)
			{
				return pMR;
			}
			pMR = new MeshRender(this);
			mComponents.push_back(pMR);
			return pMR;
		}break;
		case COMTYPE::ComType_Terrain:
		{
			Terrain* pt = (Terrain*)GetComponent(COMTYPE::ComType_Terrain);
			if (pt != nullptr)
			{
				return pt;
			}
			pt = new Terrain(this);
			mComponents.push_back(pt);
			return pt;
		}break;
		case COMTYPE::ComType_Light:
		{

		}break;
		default:
			break;
		}


		return nullptr;
	}
	//获取组件的功能
	LPCOMPONENT GetComponent(COMTYPE t)
	{
		//变换组件是单独存储的，所以优先特殊判断
		if (t == COMTYPE::ComType_Transform)
		{
			return mpTransform;
		}
		//其余的非变换组件通过遍历查找
		for (auto& itr : mComponents)
		{
			if (itr->Type() == t)
			{
				return itr;
			}
		}
		return nullptr;
	}
	//移除组件的功能
	void RemoveComponent(COMTYPE t)
	{
		if (t == COMTYPE::ComType_Transform) { return; }
		for (auto it = mComponents.begin(); it != mComponents.end(); it++)
		{
			if ((*it)->Type() == t)
			{
				delete* it;
				mComponents.erase(it);
				return;
			}
		}
	}
	//绘制物体
	void OnRender()
	{
		MeshRender* pMR = (MeshRender*)GetComponent(COMTYPE::ComType_MeshRender);
		if (pMR != nullptr)
		{
			return pMR->DrawMesh();
		}

		Terrain* pt = (Terrain*)GetComponent(COMTYPE::ComType_Terrain);
		if (pt != nullptr)
		{
			return pt->DrawTerrain();
		}
	}
	//逻辑更新
	void OnUpdate()
	{
		bool& isNeedUpdate = mpTransform->NeedUpdate();
		if (isNeedUpdate)
		{
			//如果有需要绘制的物体
			MeshRender* pMR = (MeshRender*)GetComponent(emComponentType::ComType_MeshRender);
			if (pMR != nullptr)
			{
				//重新计算世界中的顶点信息，并且加入绘制列表
				return pMR->CalculateWorldVertexts();
			}

			Terrain* pt = (Terrain*)GetComponent(emComponentType::ComType_Terrain);
			if (pt != nullptr)
			{
				return pt->UpdateTerrain();
			}

			//如果该物体是一个摄像机的话，因为transform改变了所以摄像机也要更新
			Camera* pCam = (Camera*)GetComponent(emComponentType::ComType_Camera);
			if (pCam != nullptr)
			{
				return pCam->UpdateCamera();
			}
			isNeedUpdate = false;
		}
	}

	bool LoadMesh(const char* name)
	{
		MeshRender* pMR = (MeshRender*)AddComponent(emComponentType::ComType_MeshRender);
		if (pMR != nullptr)
		{
			LPMESHDATA pmesh = MeshManager::Instance().GetMesh(name); 
			return pMR->SetMesh(pmesh);
		}
		return false;
	}


	bool CreateTerrainByHeightMap(const char* hm,
		float unitsize, float _floor, float _top)
	{
		//如果当前gameobject要作为一个地形，那么不需要transform以外的组件
		if (!mComponents.empty())
		{
			for (auto& c : mComponents)
			{
				delete c;
			}
			mComponents.clear();
		}
		Terrain* pt = (Terrain*)AddComponent(COMTYPE::ComType_Terrain);
		if (pt != nullptr)
		{
			return pt->LoadHeightMap(hm, unitsize, _floor, _top);
		}
		return false;
	}

	Transform& transform() { return *mpTransform; }
protected:
	Transform* mpTransform;//唯一的tansform组件，不允许移除
	std::vector<LPCOMPONENT> mComponents;//除了transform以外的其他组件存储在这个列表中
};

#endif






