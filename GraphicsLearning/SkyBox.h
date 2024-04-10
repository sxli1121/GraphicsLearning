#pragma once
#ifndef __COMPONENT_SKYBOX_H__
#define __COMPONENT_SKYBOX_H__
#include "Component.h"
#include "./math3d/Vector3.h"
#include "./math3d/Vector2.h"
using namespace Math;

class SkyBox : public Component
{
public:
	SkyBox(GameObject* pobj) :Component(pobj)
	{
		mType = emComponentType::ComType_SkyBox;
		vec3f temp[36] = {
			//front
			{+1,+1,-1},//2
			{-1,+1,-1},//3
			{-1,-1,-1},//7
			{+1,+1,-1},//2
			{-1,-1,-1},//7
			{+1,-1,-1},//6

			//back
			{-1,+1,+1},//0
			{+1,+1,+1},//1
			{+1,-1,+1},//5
			{-1,+1,+1},//0
			{+1,-1,+1},//5
			{-1,-1,+1},//4

			//left
			{-1,+1,-1},//3
			{-1,+1,+1},//0
			{-1,-1,+1},//4
			{-1,+1,-1},//3
			{-1,-1,+1},//4
			{-1,-1,-1},//7

			//right
			{+1,+1,+1},//1
			{+1,+1,-1},//2
			{+1,-1,-1},//6
			{+1,+1,+1},//1
			{+1,-1,-1},//6
			{+1,-1,+1},//5

			//top
			{-1,+1,-1},//3
			{+1,+1,-1},//2
			{+1,+1,+1},//1
			{-1,+1,-1},//3
			{+1,+1,+1},//1
			{-1,+1,+1},//0


			//bottom
			{-1,-1,+1},//4
			{+1,-1,+1},//5
			{+1,-1,-1},//6
			{-1,-1,+1},//4
			{+1,-1,-1},//6
			{-1,-1,-1},//7
		};
		memcpy(mPModelVertexts, temp, sizeof(vec3f) * 36);

		int itemp[36] = {
			//2,3,7,2,7,6,//front
			//0,1,5,0,5,4,//back
			//3,0,4,3,4,7,//left
			//1,2,6,1,6,5,//right
			//3,2,1,3,1,0,//top
			//4,5,6,4,6,7//bottom
			0,1,2,3,4,5,
			6,7,8,9,10,11,
			12,13,14,15,16,17,
			18,19,20,21,22,23,
			24,25,26,27,28,29,
			30,31,32,33,34,35
		};
		memcpy(mIndexs, itemp, sizeof(int) * 36);

		vec2f vtemp[36] = {
		{0,1},{1,1},{1,0},{0,1},{1,0},{0,0},
		{0,1},{1,1},{1,0},{0,1},{1,0},{0,0},
		{0,1},{1,1},{1,0},{0,1},{1,0},{0,0},
		{0,1},{1,1},{1,0},{0,1},{1,0},{0,0},
		{0,1},{1,1},{1,0},{0,1},{1,0},{0,0},
		{0,1},{1,1},{1,0},{0,1},{1,0},{0,0}
		};
		memcpy(mPUVs, vtemp, sizeof(vec2f) * 36);

	}

	bool LoadSkyBox(const char* path);

	//计算世界中的顶点信息
	void CalculateWorldVertexts();
	//模型绘制
	void DrawSkyBox();

protected:
	vec2f mPUVs[36];//uv数据
	vec3f mPModelVertexts[36];//原始顶点数据
	vec3f mPVertexts[36];//存储世界变换以后的顶点数组 = 原始模型数据 * 世界变换矩阵
	int mIndexs[36];
	class MyTexture2D* mSkyTexture[6];

};

#endif