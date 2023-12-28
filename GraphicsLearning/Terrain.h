#pragma once
#ifndef __MY_TERRAIN_H__
#define __MY_TERRAIN_H__
#include "Component.h"
#include "MeshData.h"
#include "UtilityHelper.h"
#include <deque>


//ÿ���Ĳ����Ľڵ����Ǽ�¼4���ǵ�����ĵ�
//��ö�ٸ�4���ǵ���
enum enCornerID
{
	ID_LT,//left top
	ID_RT,//right top
	ID_RB,//right bottom
	ID_LB//left bottom
};


//�����Ĳ����ڵ�ṹ��
typedef struct stTerrainQuadTreeNode
{
	//�ĸ��Ӻ��ӣ������ǰ�ڵ㲻�ɷָ�[Ҳ����Ҷ�ӽڵ�]ʱ��ÿ��ָ�붼Ϊnullptr
	stTerrainQuadTreeNode* mChildNodes[4];

	//�ڵ�����
	int mCornerId[4];//��Ӧ�ĸ��ǵ������ֵ
	int mCenterId;//���ĵ������ֵ
	int mLevel;//�Ĳ����ڵ����

	//��Χ������
	vec3f mBSCenter;//��Χ�е�Բ��
	float mBSRadius;//��Χ�еİ뾶

	~stTerrainQuadTreeNode()
	{
		//��Ϊ��ҵ������д
	}

	stTerrainQuadTreeNode()
	{
		//����δ��ʼ���Ľڵ������
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


	//�����Ĳ����Ľڵ�����
	void SetCorner(int lt, int rt, int rb, int lb)
	{
		mCornerId[ID_LT] = lt;
		mCornerId[ID_RT] = rt;
		mCornerId[ID_RB] = rb;
		mCornerId[ID_LB] = lb;
		mCenterId = (lt + rb) >> 1;
	}


	//�ж��Ƿ�Ҷ�ӽڵ�
	bool isLeaf()
	{
		//��Ϊ���ǵĵ����Ĳ��������һ���ģ����Կ���ֻ�жϿ�ȾͿ�����
		//������������С�ĸ��ӵ�λ���ҽڵ���������ֻ����1
		//�������������������ε���С�ڵ㣬Ӧ���Ǳ���С�ĸ��ӵ���һ��
		//�����������չ涨���ҽڵ�������Ϊ���յ�Ҷ�ӽڵ�
		return (mCornerId[ID_RT] - mCornerId[ID_LT]) <= 2;
	}



	//���Ĳ����ڵ���зָ�
	bool Split()
	{
		if (isLeaf()) { return false; }

		int mlrt = (mCornerId[ID_LT] + mCornerId[ID_RT]) >> 1;
		int mrr = (mCornerId[ID_RT] + mCornerId[ID_RB]) >> 1;
		int mlrb = (mCornerId[ID_LB] + mCornerId[ID_RB]) >> 1;
		int mll = (mCornerId[ID_LT] + mCornerId[ID_LB]) >> 1;

		//�����µ��Ĳ����ڵ�
		for (auto& n : mChildNodes)
		{
			if (n == nullptr)
			{
				n = new stTerrainQuadTreeNode;
			}
			n->mLevel = mLevel + 1;
		}

		// Ϊ�µĽڵ������ӽڵ�����
		mChildNodes[ID_LT]->SetCorner(mCornerId[ID_LT], mlrt, mCenterId, mll);
		mChildNodes[ID_RT]->SetCorner(mlrt, mCornerId[ID_RT], mrr, mCenterId);
		mChildNodes[ID_RB]->SetCorner(mCenterId, mrr, mCornerId[ID_RB], mlrb);
		mChildNodes[ID_LB]->SetCorner(mll, mCenterId, mlrb, mCornerId[ID_LB]);
		return true;
	}


	//�����Χ������
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

		//�ҵ��Ÿ�����������С��x,y,z
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
		//��ǰ�ڵ�İ�Χ������
		mBSRadius = (pmax - pmin).magnitude() * 0.5f;
		mBSCenter = (pmax + pmin) * 0.5f;
	}



	//ʹ���Ĳ����ڵ������������������ڻ���
	//һ�������һ���Ĳ����ڵ�����8��������
	void MakeTriangle(int* pIndex)
	{
		if (pIndex == nullptr) { return; }
		int mlrt = (mCornerId[ID_LT] + mCornerId[ID_RT]) >> 1;
		int mrr = (mCornerId[ID_RT] + mCornerId[ID_RB]) >> 1;
		int mlrb = (mCornerId[ID_LB] + mCornerId[ID_RB]) >> 1;
		int mll = (mCornerId[ID_LT] + mCornerId[ID_LB]) >> 1;

		//���Ƴ������ĵ�֮��İ˸����������մ����Ͻǿ�ʼ˳ʱ���˳��
		//�洢����ʱ��������,ע����������
		int temp[] = {
			mCornerId[ID_LT],mlrt,mCornerId[ID_RT],mrr,
			mCornerId[ID_RB],mlrb,mCornerId[ID_LB],mll,mCornerId[ID_LT]
		};

		//��������������
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
			delete mPTreeRoot;//ǰ������ҵ�Ľڵ��ɾ������ʵ����
		}
		mPTreeRoot = nullptr;
	}

	bool CreatTree(size_t nSize)
	{
		// ��������
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
		printf("���������Ĳ����Ĵ�С�����ϵ����Ĳ�����Ҫ��\n");
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
	LPTQTNODE mPTreeRoot;//�Ĳ����ĸ��ڵ�
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

	//���ظ߶�ͼ
	bool LoadHeightMap(
		const char* hm,
		float unitsize,//��λ��С��һ�����ӵ����ؿ�ߣ�������֮��ļ������
		float _floor,//��С�߶�
		float _top);//���߶�

	void DrawTerrain();   // �Լ�����

	void UpdateTerrain();

protected:
	void _Clear();

protected:
	LPMESHDATA mPMeshData;   // ���صĵ���ģ������
	vec3f* mPVertexts;  //�����еĶ�������

	int mSize;  //�ݺ����--Ϊ��ʹ������
	float mRealSize;//�����ʵ�������С
};

#endif