#pragma once
#ifndef __MY_TERRAIN_H__
#define __MY_TERRAIN_H__
#include "Component.h"
#include "MeshData.h"
#include "UtilityHelper.h"
#include <deque>


//每个四叉树的节点我们记录4个角点和中心点
//用枚举给4个角点编号
enum enCornerID
{
	ID_LT,//left top
	ID_RT,//right top
	ID_RB,//right bottom
	ID_LB//left bottom
};


//地形四叉树节点结构体
typedef struct stTerrainQuadTreeNode
{
	//四个子孩子，如果当前节点不可分割[也就是叶子节点]时，每个指针都为nullptr
	stTerrainQuadTreeNode* mChildNodes[4];

	//节点数据
	int mCornerId[4];//对应四个角点的索引值
	int mCenterId;//中心点的索引值
	int mLevel;//四叉树节点层数

	//包围盒数据
	vec3f mBSCenter;//包围盒的圆心
	float mBSRadius;//包围盒的半径

	~stTerrainQuadTreeNode()
	{
		//作为作业：你们写
	}

	stTerrainQuadTreeNode()
	{
		//对于未初始化的节点的设置
		for (int i = 0; i < 4; ++i)
		{
			mChildNodes[i] = nullptr;
			mCornerId[i] = -1;
		}

		mCenterId = -1;
		mLevel = -1;
		mBSCenter = vec3f::zero();
		mBSRadius = 0;
	}


	//设置四叉树的节点数据
	void SetCorner(int lt, int rt, int rb, int lb)
	{
		mCornerId[ID_LT] = lt;
		mCornerId[ID_RT] = rt;
		mCornerId[ID_RB] = rb;
		mCornerId[ID_LB] = lb;
		mCenterId = (lt + rb) >> 1;
	}


	//判断是否叶子节点
	bool isLeaf()
	{
		//因为我们的地形四叉树宽高是一样的，所以可以只判断宽度就可以了
		//正常来讲，最小的格子单位左右节点的索引相差只能是1
		//但是我们能生成三角形的最小节点，应该是比最小的格子的上一层
		//所以我们最终规定左右节点相差二作为最终的叶子节点
		return (mCornerId[ID_RT] - mCornerId[ID_LT]) <= 2;
	}



	//对四叉树节点进行分割
	bool Split()
	{
		if (isLeaf()) { return false; }

		int mlrt = (mCornerId[ID_LT] + mCornerId[ID_RT]) >> 1;
		int mrr = (mCornerId[ID_RT] + mCornerId[ID_RB]) >> 1;
		int mlrb = (mCornerId[ID_LB] + mCornerId[ID_RB]) >> 1;
		int mll = (mCornerId[ID_LT] + mCornerId[ID_LB]) >> 1;

		//产生新的四叉树节点
		for (auto& n : mChildNodes)
		{
			if (n == nullptr)
			{
				n = new stTerrainQuadTreeNode;
			}
			n->mLevel = mLevel + 1;
		}

		// 为新的节点设置子节点数据
		mChildNodes[ID_LT]->SetCorner(mCornerId[ID_LT], mlrt, mCenterId, mll);
		mChildNodes[ID_RT]->SetCorner(mlrt, mCornerId[ID_RT], mrr, mCenterId);
		mChildNodes[ID_RB]->SetCorner(mCenterId, mrr, mCornerId[ID_RB], mlrb);
		mChildNodes[ID_LB]->SetCorner(mll, mCenterId, mlrb, mCornerId[ID_LB]);
		return true;
	}


	//计算包围盒数据
	void CalcBoundingSphere(const vec3f* pVertexts)
	{
		if (pVertexts == nullptr) { return; }

		int mlrt = (mCornerId[ID_LT] + mCornerId[ID_RT]) >> 1;
		int mrr = (mCornerId[ID_RT] + mCornerId[ID_RB]) >> 1;
		int mlrb = (mCornerId[ID_LB] + mCornerId[ID_RB]) >> 1;
		int mll = (mCornerId[ID_LT] + mCornerId[ID_LB]) >> 1;

		const vec3f* pts[9] = {
			&pVertexts[mCornerId[ID_LT]],
			&pVertexts[mlrt],
			&pVertexts[mCornerId[ID_RT]],

			&pVertexts[mll],
			&pVertexts[mCenterId],
			&pVertexts[mrr],

			&pVertexts[mCornerId[ID_LB]],
			&pVertexts[mlrb],
			&pVertexts[mCornerId[ID_RB]],
		};

		//找到九个点中最大和最小的x,y,z
		vec3f pmax = *pts[0], pmin = *pts[0];
		for (int i = 1; i < 9; ++i)
		{
			if (pts[i][0].x < pmin.x) { pmin.x = pts[i][0].x; }
			if (pts[i][0].y < pmin.y) { pmin.x = pts[i][0].y; }
			if (pts[i][0].z < pmin.z) { pmin.x = pts[i][0].z; }

			if (pts[i][0].x > pmax.x) { pmax.x = pts[i][0].x; }
			if (pts[i][0].y > pmax.y) { pmax.x = pts[i][0].y; }
			if (pts[i][0].z > pmax.z) { pmax.x = pts[i][0].z; }
		}
		//当前节点的包围球数据
		mBSRadius = (pmax - pmin).magnitude() * 0.5f;
		mBSCenter = (pmax + pmin) * 0.5f;
	}



	//使用四叉树节点生成三角形索引用于绘制
	//一般情况下一个四叉树节点生成8个三角形
	void MakeTriangle(int* pIndex)
	{
		if (pIndex == nullptr) { return; }
		int mlrt = (mCornerId[ID_LT] + mCornerId[ID_RT]) >> 1;
		int mrr = (mCornerId[ID_RT] + mCornerId[ID_RB]) >> 1;
		int mlrb = (mCornerId[ID_LB] + mCornerId[ID_RB]) >> 1;
		int mll = (mCornerId[ID_LT] + mCornerId[ID_LB]) >> 1;

		//将移除了中心点之外的八个点索引按照从左上角开始顺时针的顺序
		//存储到临时的数组中,注意最后的索引
		int temp[] = {
			mCornerId[ID_LT],mlrt,mCornerId[ID_RT],mrr,
			mCornerId[ID_RB],mlrb,mCornerId[ID_LB],mll,mCornerId[ID_LT]
		};

		//计算三角形索引
		for (int i = 0; i < 8; ++i)
		{
			pIndex[i * 3] = mCenterId;
			pIndex[i * 3 + 1] = temp[i];
			pIndex[i * 3 + 2] = temp[i + 1];
		}
	}

	void Show()
	{
		printf("Level:%d - [%4d,%4d,%4d,%4d]\n", mLevel,
			mCornerId[0], mCornerId[1], mCornerId[2], mCornerId[3]);

		if (!isLeaf())
		{
			for (int i = 0; i < 4; ++i)
			{
				mChildNodes[i]->Show();
			}
		}
	}

}TQTNODE, * LPTQTNODE;


typedef class TerrainQuadTree
{
public:
	TerrainQuadTree()
	{
		mPTreeRoot = nullptr;
	}
	~TerrainQuadTree()
	{
		if (mPTreeRoot != nullptr)
		{
			delete mPTreeRoot;//前提是作业的节点的删除代码实现了
		}
		mPTreeRoot = nullptr;
	}

	bool CreatTree(size_t nSize)
	{
		// 清理数据
		if (mPTreeRoot != nullptr)
		{
			delete mPTreeRoot;
			mPTreeRoot = nullptr;
		}

		if (nSize > 2 && IS_POWER_OF2(nSize - 1))
		{
			int maxid = nSize * nSize - 1; 
			std::deque<LPTQTNODE> nodes;

			mPTreeRoot = new TQTNODE;

			mPTreeRoot->mLevel = 0;
			mPTreeRoot->mCenterId = maxid >> 1;
			mPTreeRoot->SetCorner(0, nSize - 1, maxid, maxid - (nSize - 1));
			nodes.push_back(mPTreeRoot);

			while (!nodes.empty())
			{
				LPTQTNODE& curNode = nodes.front();
				if (curNode->Split())
				{
					nodes.push_back(curNode->mChildNodes[ID_LT]);
					nodes.push_back(curNode->mChildNodes[ID_RT]);
					nodes.push_back(curNode->mChildNodes[ID_RB]);
					nodes.push_back(curNode->mChildNodes[ID_LB]);
				}
				nodes.pop_front();
			}
			return true;
		}
		printf("欲创建的四叉树的大小不符合地形四叉树的要求\n");
		return false;
	}


	void Show()
	{
		if (mPTreeRoot)
		{
			mPTreeRoot->Show();
		}
	}
private:
	LPTQTNODE mPTreeRoot;//四叉树的根节点
}TQTREE, * LPTQTREE;



class Terrain : public Component
{
public:
	Terrain(class GameObject* obj) : Component(obj)
	{
		mType = COMTYPE::ComType_Terrain;
		mPMeshData = nullptr;
		mPVertexts = nullptr;
		mSize = 0;
		mRealSize = 0;
	}
	~Terrain();

	//加载高度图
	bool LoadHeightMap(
		const char* hm,
		float unitsize,//单位大小，一个格子的像素宽高，两个点之间的间隔距离
		float _floor,//最小高度
		float _top);//最大高度

	void DrawTerrain();   // 自己绘制

	void UpdateTerrain();

protected:
	void _Clear();

protected:
	LPMESHDATA mPMeshData;   // 加载的地形模型数据
	vec3f* mPVertexts;  //世界中的顶点数据

	int mSize;  //纵横点数--为了使用网格
	float mRealSize;//最后真实的世界大小
};

#endif