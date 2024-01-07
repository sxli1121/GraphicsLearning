#pragma once
#ifndef __MY_TERRAIN_H__
#define __MY_TERRAIN_H__
#include "Component.h"
#include "MeshData.h"
#include "UtilityHelper.h"
#include <deque>
#include "Geometric.h"

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
private:
	void _Clear()
	{
		//printf("释放节点 level:%d,%p\n", mLevel, this);
		for (auto& child : mChildNodes)
		{
			if (child != nullptr)
			{
				child->_Clear();
				delete child;
				child = nullptr;
			}
		}
	}

public:
	~stTerrainQuadTreeNode()
	{
		_Clear();
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

	//从平坦程度来评价
	bool CanBeSplitByC1(const vec3f* pVertexts, const vec3f& campos, float c1)
	{
		float L = (campos - pVertexts[mCenterId]).magnitude(); // 摄像机距离远近
		float D = pVertexts[mCornerId[ID_RT]].x - pVertexts[mCornerId[ID_LT]].x;  // 矩形的边长

		return L / D < c1;   // 此数值与距离成正比 与 边长成反比    当数值小于设定数值时 可以继续分  大于的时候 就不能分了     越远越不能分  边长越大 越可以分
	}

	//从粗糙程度来评价是否需要划分
	bool CanBeSplitByC2(const vec3f* pVertexts, float c2)
	{
		int mlrt = (mCornerId[ID_LT] + mCornerId[ID_RT]) >> 1;
		int mrr = (mCornerId[ID_RT] + mCornerId[ID_RB]) >> 1;
		int mlrb = (mCornerId[ID_LB] + mCornerId[ID_RB]) >> 1;
		int mll = (mCornerId[ID_LT] + mCornerId[ID_LB]) >> 1;

		float dh1 = abs(pVertexts[mlrt].y - (pVertexts[mCornerId[ID_LT]].y + pVertexts[mCornerId[ID_RT]].y) * 0.5f);
		float dh2 = abs(pVertexts[mrr].y - (pVertexts[mCornerId[ID_RT]].y + pVertexts[mCornerId[ID_RB]].y) * 0.5f);
		float dh3 = abs(pVertexts[mlrb].y - (pVertexts[mCornerId[ID_LB]].y + pVertexts[mCornerId[ID_RB]].y) * 0.5f);
		float dh4 = abs(pVertexts[mll].y - (pVertexts[mCornerId[ID_LT]].y + pVertexts[mCornerId[ID_LB]].y) * 0.5f);
		float dh5 = abs(pVertexts[mCenterId].y - (pVertexts[mCornerId[ID_LT]].y + pVertexts[mCornerId[ID_RB]].y) * 0.5f);
		float dh6 = abs(pVertexts[mCenterId].y - (pVertexts[mCornerId[ID_RT]].y + pVertexts[mCornerId[ID_LB]].y) * 0.5f);

		float maxdh = max(max(max(dh1, dh2), max(dh3, dh4)), max(dh5, dh6));
		return maxdh > c2;
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
			if (pts[i][0].y < pmin.y) { pmin.y = pts[i][0].y; }
			if (pts[i][0].z < pmin.z) { pmin.z = pts[i][0].z; }

			if (pts[i][0].x > pmax.x) { pmax.x = pts[i][0].x; }
			if (pts[i][0].y > pmax.y) { pmax.y = pts[i][0].y; }
			if (pts[i][0].z > pmax.z) { pmax.z = pts[i][0].z; }
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

		//if (!isLeaf())
		//{
		//	for (int i = 0; i < 4; ++i)
		//	{
		//		mChildNodes[i]->Show();
		//	}
		//}
	}

}TQTNODE, * LPTQTNODE;


typedef class TerrainQuadTree
{
public:
	TerrainQuadTree()
	{
		mPTreeRoot = nullptr;
		mLod_C1 = 1;
		mLod_C2 = 1;
	}


	~TerrainQuadTree()
	{
		if (mPTreeRoot != nullptr)
		{
			delete mPTreeRoot;//前提是作业的节点的删除代码实现了
		}
		mPTreeRoot = nullptr;
	}


	bool CreateTree(size_t nSize, const vec3f* pVertexts)
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

				//计算包围球的数据
				curNode->CalcBoundingSphere(pVertexts);


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

	//将四叉树视景体裁剪粗糙系数结合起来
	void IntersectFrustumLodByC2(
		const vec3f* pVertexts,
		const vec3f& campos,
		const Frustum& fr,
		int* pIndex,
		int& triangleCount,
		int& indexCount)
	{
		//使用双队列的方式遍历查找，避免递归操作
		std::deque<LPTQTNODE> deque1;
		std::deque<LPTQTNODE> deque2;

		std::deque<LPTQTNODE> dequeRender;//存储最后要绘制的节点

		std::deque<LPTQTNODE>* pDequeCurLevel = &deque1;
		std::deque<LPTQTNODE>* pDequeNextLevel = &deque2;

		//将树的根节点加入到当前层的队列
		pDequeCurLevel->push_back(mPTreeRoot);

		while ((!pDequeCurLevel->empty()))
		{
			while (!pDequeCurLevel->empty())
			{
				//从当前层队列中取出当前需要处理的节点
				LPTQTNODE& pCurNode = pDequeCurLevel->front();

				//如果已经是叶子节点就直接作为要绘制的节点
				if (pCurNode->isLeaf())
				{
					dequeRender.push_back(pCurNode);
				}
				else
				{
					//先用C2判断是否需要继续划分
					//如果需要划分，则将其加入下一层待分割节点
					//如果不要划分，则加入渲染队列
					if (!pCurNode->CanBeSplitByC2(pVertexts, mLod_C2))
					{
						dequeRender.push_back(pCurNode);
					}
					else
					{
						for (int i = 0; i < 4; ++i)
						{
							//判断四个子节点包围球与摄像机的视景体做相交判断
							if (fr.SphereInFrustum(pCurNode->mChildNodes[i]->mBSCenter,
								pCurNode->mChildNodes[i]->mBSRadius))
							{
								//如果确定与视景体相交，那么加入到下一层要集中处理的队列中
								pDequeNextLevel->push_back(pCurNode->mChildNodes[i]);
							}
							else
							{
								////表示该节点与视景体不想交，那么也就意味着不需要绘制
								//printf("剔除区域:");
								//pCurNode->mChildNodes[i]->Show();
							}
						}
					}
				}
				//表示的是当前节点 pCurNode 处理结束，需要从当前队列中删除
				pDequeCurLevel->pop_front();
			}
			//表示当前一层的节点处理结束了，需要进行下一层的处理
			//将下一层设置为要处理的当前层，当前层作为要处理的下一层数据的队列
			std::deque<LPTQTNODE>* temp = pDequeCurLevel;
			pDequeCurLevel = pDequeNextLevel;
			pDequeNextLevel = temp;
		}
		//最后一层的数据存放在当前层中，全部都是不可分割的子节点，
		//也就是和视景体相交的节点
		//此时将剔除后的剩余的节点生成的三角形索引用于绘制三角形数据

		triangleCount = 0;
		indexCount = 0;
		for (auto& node : dequeRender)
		{
			node->MakeTriangle(pIndex + indexCount);
			triangleCount += 8;//每分一个节点增加8个三角形
			indexCount += 24;//每分一个节点增加24个三角形索引
		}
		pDequeCurLevel->clear();
		pDequeNextLevel->clear();
		dequeRender.clear();
	}

	//将四叉树视景体裁剪和平坦系数结合起来
	void IntersectFrustumLodByC1(
		const vec3f* pVertexts,
		const vec3f& campos,
		const Frustum& fr,
		int* pIndex,
		int& triangleCount,
		int& indexCount)
	{
		//使用双队列的方式遍历查找，避免递归操作
		std::deque<LPTQTNODE> deque1;
		std::deque<LPTQTNODE> deque2;

		std::deque<LPTQTNODE> dequeRender;//存储最后要绘制的节点

		std::deque<LPTQTNODE>* pDequeCurLevel = &deque1;
		std::deque<LPTQTNODE>* pDequeNextLevel = &deque2;

		//将树的根节点加入到当前层的队列
		pDequeCurLevel->push_back(mPTreeRoot);

		while ((!pDequeCurLevel->empty()))
		{
			while (!pDequeCurLevel->empty())
			{
				//从当前层队列中取出当前需要处理的节点
				LPTQTNODE& pCurNode = pDequeCurLevel->front();

				//如果已经是叶子节点就直接作为要绘制的节点
				if (pCurNode->isLeaf())
				{
					dequeRender.push_back(pCurNode);
				}
				else
				{
					//先用C1判断是否需要继续划分
					//如果需要划分，则将其加入下一层待分割节点
					//如果不要划分，则加入渲染队列
					if (!pCurNode->CanBeSplitByC1(pVertexts, campos, mLod_C1))
					{
						dequeRender.push_back(pCurNode);
					}
					else
					{
						for (int i = 0; i < 4; ++i)
						{
							//判断四个子节点包围球与摄像机的视景体做相交判断
							if (fr.SphereInFrustum(pCurNode->mChildNodes[i]->mBSCenter,
								pCurNode->mChildNodes[i]->mBSRadius))
							{
								//如果确定与视景体相交，那么加入到下一层要集中处理的队列中
								pDequeNextLevel->push_back(pCurNode->mChildNodes[i]);
							}
							else
							{
								////表示该节点与视景体不想交，那么也就意味着不需要绘制
								//printf("剔除区域:");
								//pCurNode->mChildNodes[i]->Show();
							}
						}
					}
				}
				//表示的是当前节点 pCurNode 处理结束，需要从当前队列中删除
				pDequeCurLevel->pop_front();
			}
			//表示当前一层的节点处理结束了，需要进行下一层的处理
			//将下一层设置为要处理的当前层，当前层作为要处理的下一层数据的队列
			std::deque<LPTQTNODE>* temp = pDequeCurLevel;
			pDequeCurLevel = pDequeNextLevel;
			pDequeNextLevel = temp;
		}
		//最后一层的数据存放在当前层中，全部都是不可分割的子节点，
		//也就是和视景体相交的节点
		//此时将剔除后的剩余的节点生成的三角形索引用于绘制三角形数据

		triangleCount = 0;
		indexCount = 0;
		for (auto& node : dequeRender)
		{
			node->MakeTriangle(pIndex + indexCount);
			triangleCount += 8;//每分一个节点增加8个三角形
			indexCount += 24;//每分一个节点增加24个三角形索引
		}
		pDequeCurLevel->clear();
		pDequeNextLevel->clear();
		dequeRender.clear();
	}


	//进行四叉树视景体的剔除
	void IntersectFrustum(const Frustum& fr,
		int* pIndex,
		int& triangleCount,
		int& indexCount)
	{
		//使用双队列的方式遍历查找，避免递归操作
		std::deque<LPTQTNODE> deque1;
		std::deque<LPTQTNODE> deque2;
		std::deque<LPTQTNODE>* pDequeCurLevel = &deque1;
		std::deque<LPTQTNODE>* pDequeNextLevel = &deque2;

		//将树的根节点加入到当前层的队列
		pDequeCurLevel->push_back(mPTreeRoot);

		while ((!pDequeCurLevel->empty()) && (!pDequeCurLevel->front()->isLeaf()))
		{
			while (!pDequeCurLevel->empty())
			{
				//从当前层队列中取出当前需要处理的节点
				LPTQTNODE& pCurNode = pDequeCurLevel->front();
				for (int i = 0; i < 4; ++i)
				{
					//判断四个子节点包围球与摄像机的视景体做相交判断
					if (fr.SphereInFrustum(pCurNode->mChildNodes[i]->mBSCenter,
						pCurNode->mChildNodes[i]->mBSRadius))
					{
						//如果确定与视景体相交，那么加入到下一层要集中处理的队列中
						pDequeNextLevel->push_back(pCurNode->mChildNodes[i]);
					}
					else
					{
						////表示该节点与视景体不想交，那么也就意味着不需要绘制
						//printf("剔除区域:");
						//pCurNode->mChildNodes[i]->Show();
					}
				}
				//表示的是当前节点 pCurNode 处理结束，需要从当前队列中删除
				pDequeCurLevel->pop_front();
			}
			//表示当前一层的节点处理结束了，需要进行下一层的处理
			//将下一层设置为要处理的当前层，当前层作为要处理的下一层数据的队列
			std::deque<LPTQTNODE>* temp = pDequeCurLevel;
			pDequeCurLevel = pDequeNextLevel;
			pDequeNextLevel = temp;
		}
		//最后一层的数据存放在当前层中，全部都是不可分割的子节点，
		//也就是和视景体相交的节点
		//此时将剔除后的剩余的节点生成的三角形索引用于绘制三角形数据
		triangleCount = 0;
		indexCount = 0;
		for (auto& node : *pDequeCurLevel)
		{
			node->MakeTriangle(pIndex + indexCount);
			triangleCount += 8;//每分一个节点增加8个三角形
			indexCount += 24;//每分一个节点增加24个三角形索引
		}
		pDequeCurLevel->clear();
	}

	void SetC1(float c1) { mLod_C1 = c1; }
	void SetC2(float c2) { mLod_C2 = c2; }

private:
	LPTQTNODE mPTreeRoot;//四叉树的根节点
	float mLod_C1;
	float mLod_C2;


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

	void SetC1(float c1) { mTQuadTree.SetC1(c1); }
	void SetC2(float c2) { mTQuadTree.SetC2(c2); }

protected:
	void _Clear();
	void _CalcuVertext2World();
protected:
	LPMESHDATA mPMeshData;   // 加载的地形模型数据
	vec3f* mPVertexts;  //世界中的顶点数据

	int* mPIndex;//动态的由四叉树生成的三角形的索引

	int mTriangleCount;//记录动态的三角形数量
	int mIndexCount;//记录动态的索引数量

	int mSize;  //纵横点数--为了使用网格
	float mRealSize;//最后真实的世界大小

	TerrainQuadTree mTQuadTree; // 地形四叉树
};

#endif