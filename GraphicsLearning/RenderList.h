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
#endif