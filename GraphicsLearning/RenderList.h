#pragma once
#ifndef __RENDER_LIST_H__
#define __RENDER_LIST_H__
#include "SingletonTemplate.h"
#include "GameObject.h"
#include "UtilityHelper.h"
#include "Geometric.h"


class RenderList:public Singleton<RenderList>
{
	PRIVATE_COPY(RenderList)
	RenderList();
	friend class TerrainRenderList;
public:
	//增加需要绘制的三角形
	void AddTriangle();
	//获取下一个可用三角形
	Triangle& GetNextTriangle();
	//获取三角形数量
	int GetTriangleCount();
	//绘制所有的三角形
	void RenderAllTriangle();
	//清除现有的三角形
	void ClearTriangle();

protected:
private:
	int mCurIndex;//表示三角形的数量也表示下一个可用的三角形的索引
	static Triangle mRenderTriangle[MAX_TRANGLE_NUM];
};
#define RL RenderList::Instance()



class TerrainRenderList :public Singleton<TerrainRenderList>
{
	PRIVATE_COPY(TerrainRenderList)
	TerrainRenderList();
	friend class RenderList;
public:
	//增加需要绘制的三角形
	void AddTriangle();
	//获取下一个可用三角形
	Triangle& GetNextTriangle();
	//获取三角形数量
	int GetTriangleCount();
	//绘制所有的三角形
	void RenderAllTriangle();
	//清除现有的三角形
	void ClearTriangle();

protected:
private:
	int mCurIndex;//表示三角形的数量也表示下一个可用的三角形的索引
	static Triangle mTerrainRenderTriangle[MAX_TRANGLE_NUM];
};
#define TRL TerrainRenderList::Instance()



class SkyBoxRenderList :public Singleton<SkyBoxRenderList>
{
	PRIVATE_COPY(SkyBoxRenderList)
		SkyBoxRenderList();
public:
	//增加需要绘制的三角形
	void AddTriangle();
	//获取下一个可用三角形
	Triangle& GetNextTriangle();
	//获取三角形数量
	int GetTriangleCount();
	//绘制所有的三角形
	void RenderAllTriangle();
	//清除现有的三角形
	void ClearTriangle();

protected:
private:
	int mCurIndex;//表示三角形的数量也表示下一个可用的三角形的索引
	static Triangle mSkyBoxRenderTriangle[24];
};
#define SRL SkyBoxRenderList::Instance()

#endif