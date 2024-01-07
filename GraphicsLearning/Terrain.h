#pragma once
#ifndef __MY_TERRAIN_H__
#define __MY_TERRAIN_H__
#include "Component.h"
#include "MeshData.h"
#include "UtilityHelper.h"
#include <deque>
#include "Geometric.h"

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
private:
	void _Clear()
	{
		//printf("�ͷŽڵ� level:%d,%p\n", mLevel, this);
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

	//��ƽ̹�̶�������
	bool CanBeSplitByC1(const vec3f* pVertexts, const vec3f& campos, float c1)
	{
		float L = (campos - pVertexts[mCenterId]).magnitude(); // ���������Զ��
		float D = pVertexts[mCornerId[ID_RT]].x - pVertexts[mCornerId[ID_LT]].x;  // ���εı߳�

		return L / D < c1;   // ����ֵ���������� �� �߳��ɷ���    ����ֵС���趨��ֵʱ ���Լ�����  ���ڵ�ʱ�� �Ͳ��ܷ���     ԽԶԽ���ܷ�  �߳�Խ�� Խ���Է�
	}

	//�Ӵֲڳ̶��������Ƿ���Ҫ����
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
			if (pts[i][0].y < pmin.y) { pmin.y = pts[i][0].y; }
			if (pts[i][0].z < pmin.z) { pmin.z = pts[i][0].z; }

			if (pts[i][0].x > pmax.x) { pmax.x = pts[i][0].x; }
			if (pts[i][0].y > pmax.y) { pmax.y = pts[i][0].y; }
			if (pts[i][0].z > pmax.z) { pmax.z = pts[i][0].z; }
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
			delete mPTreeRoot;//ǰ������ҵ�Ľڵ��ɾ������ʵ����
		}
		mPTreeRoot = nullptr;
	}


	bool CreateTree(size_t nSize, const vec3f* pVertexts)
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

				//�����Χ�������
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

	//���Ĳ����Ӿ���ü��ֲ�ϵ���������
	void IntersectFrustumLodByC2(
		const vec3f* pVertexts,
		const vec3f& campos,
		const Frustum& fr,
		int* pIndex,
		int& triangleCount,
		int& indexCount)
	{
		//ʹ��˫���еķ�ʽ�������ң�����ݹ����
		std::deque<LPTQTNODE> deque1;
		std::deque<LPTQTNODE> deque2;

		std::deque<LPTQTNODE> dequeRender;//�洢���Ҫ���ƵĽڵ�

		std::deque<LPTQTNODE>* pDequeCurLevel = &deque1;
		std::deque<LPTQTNODE>* pDequeNextLevel = &deque2;

		//�����ĸ��ڵ���뵽��ǰ��Ķ���
		pDequeCurLevel->push_back(mPTreeRoot);

		while ((!pDequeCurLevel->empty()))
		{
			while (!pDequeCurLevel->empty())
			{
				//�ӵ�ǰ�������ȡ����ǰ��Ҫ����Ľڵ�
				LPTQTNODE& pCurNode = pDequeCurLevel->front();

				//����Ѿ���Ҷ�ӽڵ��ֱ����ΪҪ���ƵĽڵ�
				if (pCurNode->isLeaf())
				{
					dequeRender.push_back(pCurNode);
				}
				else
				{
					//����C2�ж��Ƿ���Ҫ��������
					//�����Ҫ���֣����������һ����ָ�ڵ�
					//�����Ҫ���֣��������Ⱦ����
					if (!pCurNode->CanBeSplitByC2(pVertexts, mLod_C2))
					{
						dequeRender.push_back(pCurNode);
					}
					else
					{
						for (int i = 0; i < 4; ++i)
						{
							//�ж��ĸ��ӽڵ��Χ������������Ӿ������ཻ�ж�
							if (fr.SphereInFrustum(pCurNode->mChildNodes[i]->mBSCenter,
								pCurNode->mChildNodes[i]->mBSRadius))
							{
								//���ȷ�����Ӿ����ཻ����ô���뵽��һ��Ҫ���д���Ķ�����
								pDequeNextLevel->push_back(pCurNode->mChildNodes[i]);
							}
							else
							{
								////��ʾ�ýڵ����Ӿ��岻�뽻����ôҲ����ζ�Ų���Ҫ����
								//printf("�޳�����:");
								//pCurNode->mChildNodes[i]->Show();
							}
						}
					}
				}
				//��ʾ���ǵ�ǰ�ڵ� pCurNode �����������Ҫ�ӵ�ǰ������ɾ��
				pDequeCurLevel->pop_front();
			}
			//��ʾ��ǰһ��Ľڵ㴦������ˣ���Ҫ������һ��Ĵ���
			//����һ������ΪҪ����ĵ�ǰ�㣬��ǰ����ΪҪ�������һ�����ݵĶ���
			std::deque<LPTQTNODE>* temp = pDequeCurLevel;
			pDequeCurLevel = pDequeNextLevel;
			pDequeNextLevel = temp;
		}
		//���һ������ݴ���ڵ�ǰ���У�ȫ�����ǲ��ɷָ���ӽڵ㣬
		//Ҳ���Ǻ��Ӿ����ཻ�Ľڵ�
		//��ʱ���޳����ʣ��Ľڵ����ɵ��������������ڻ�������������

		triangleCount = 0;
		indexCount = 0;
		for (auto& node : dequeRender)
		{
			node->MakeTriangle(pIndex + indexCount);
			triangleCount += 8;//ÿ��һ���ڵ�����8��������
			indexCount += 24;//ÿ��һ���ڵ�����24������������
		}
		pDequeCurLevel->clear();
		pDequeNextLevel->clear();
		dequeRender.clear();
	}

	//���Ĳ����Ӿ���ü���ƽ̹ϵ���������
	void IntersectFrustumLodByC1(
		const vec3f* pVertexts,
		const vec3f& campos,
		const Frustum& fr,
		int* pIndex,
		int& triangleCount,
		int& indexCount)
	{
		//ʹ��˫���еķ�ʽ�������ң�����ݹ����
		std::deque<LPTQTNODE> deque1;
		std::deque<LPTQTNODE> deque2;

		std::deque<LPTQTNODE> dequeRender;//�洢���Ҫ���ƵĽڵ�

		std::deque<LPTQTNODE>* pDequeCurLevel = &deque1;
		std::deque<LPTQTNODE>* pDequeNextLevel = &deque2;

		//�����ĸ��ڵ���뵽��ǰ��Ķ���
		pDequeCurLevel->push_back(mPTreeRoot);

		while ((!pDequeCurLevel->empty()))
		{
			while (!pDequeCurLevel->empty())
			{
				//�ӵ�ǰ�������ȡ����ǰ��Ҫ����Ľڵ�
				LPTQTNODE& pCurNode = pDequeCurLevel->front();

				//����Ѿ���Ҷ�ӽڵ��ֱ����ΪҪ���ƵĽڵ�
				if (pCurNode->isLeaf())
				{
					dequeRender.push_back(pCurNode);
				}
				else
				{
					//����C1�ж��Ƿ���Ҫ��������
					//�����Ҫ���֣����������һ����ָ�ڵ�
					//�����Ҫ���֣��������Ⱦ����
					if (!pCurNode->CanBeSplitByC1(pVertexts, campos, mLod_C1))
					{
						dequeRender.push_back(pCurNode);
					}
					else
					{
						for (int i = 0; i < 4; ++i)
						{
							//�ж��ĸ��ӽڵ��Χ������������Ӿ������ཻ�ж�
							if (fr.SphereInFrustum(pCurNode->mChildNodes[i]->mBSCenter,
								pCurNode->mChildNodes[i]->mBSRadius))
							{
								//���ȷ�����Ӿ����ཻ����ô���뵽��һ��Ҫ���д���Ķ�����
								pDequeNextLevel->push_back(pCurNode->mChildNodes[i]);
							}
							else
							{
								////��ʾ�ýڵ����Ӿ��岻�뽻����ôҲ����ζ�Ų���Ҫ����
								//printf("�޳�����:");
								//pCurNode->mChildNodes[i]->Show();
							}
						}
					}
				}
				//��ʾ���ǵ�ǰ�ڵ� pCurNode �����������Ҫ�ӵ�ǰ������ɾ��
				pDequeCurLevel->pop_front();
			}
			//��ʾ��ǰһ��Ľڵ㴦������ˣ���Ҫ������һ��Ĵ���
			//����һ������ΪҪ����ĵ�ǰ�㣬��ǰ����ΪҪ�������һ�����ݵĶ���
			std::deque<LPTQTNODE>* temp = pDequeCurLevel;
			pDequeCurLevel = pDequeNextLevel;
			pDequeNextLevel = temp;
		}
		//���һ������ݴ���ڵ�ǰ���У�ȫ�����ǲ��ɷָ���ӽڵ㣬
		//Ҳ���Ǻ��Ӿ����ཻ�Ľڵ�
		//��ʱ���޳����ʣ��Ľڵ����ɵ��������������ڻ�������������

		triangleCount = 0;
		indexCount = 0;
		for (auto& node : dequeRender)
		{
			node->MakeTriangle(pIndex + indexCount);
			triangleCount += 8;//ÿ��һ���ڵ�����8��������
			indexCount += 24;//ÿ��һ���ڵ�����24������������
		}
		pDequeCurLevel->clear();
		pDequeNextLevel->clear();
		dequeRender.clear();
	}


	//�����Ĳ����Ӿ�����޳�
	void IntersectFrustum(const Frustum& fr,
		int* pIndex,
		int& triangleCount,
		int& indexCount)
	{
		//ʹ��˫���еķ�ʽ�������ң�����ݹ����
		std::deque<LPTQTNODE> deque1;
		std::deque<LPTQTNODE> deque2;
		std::deque<LPTQTNODE>* pDequeCurLevel = &deque1;
		std::deque<LPTQTNODE>* pDequeNextLevel = &deque2;

		//�����ĸ��ڵ���뵽��ǰ��Ķ���
		pDequeCurLevel->push_back(mPTreeRoot);

		while ((!pDequeCurLevel->empty()) && (!pDequeCurLevel->front()->isLeaf()))
		{
			while (!pDequeCurLevel->empty())
			{
				//�ӵ�ǰ�������ȡ����ǰ��Ҫ����Ľڵ�
				LPTQTNODE& pCurNode = pDequeCurLevel->front();
				for (int i = 0; i < 4; ++i)
				{
					//�ж��ĸ��ӽڵ��Χ������������Ӿ������ཻ�ж�
					if (fr.SphereInFrustum(pCurNode->mChildNodes[i]->mBSCenter,
						pCurNode->mChildNodes[i]->mBSRadius))
					{
						//���ȷ�����Ӿ����ཻ����ô���뵽��һ��Ҫ���д���Ķ�����
						pDequeNextLevel->push_back(pCurNode->mChildNodes[i]);
					}
					else
					{
						////��ʾ�ýڵ����Ӿ��岻�뽻����ôҲ����ζ�Ų���Ҫ����
						//printf("�޳�����:");
						//pCurNode->mChildNodes[i]->Show();
					}
				}
				//��ʾ���ǵ�ǰ�ڵ� pCurNode �����������Ҫ�ӵ�ǰ������ɾ��
				pDequeCurLevel->pop_front();
			}
			//��ʾ��ǰһ��Ľڵ㴦������ˣ���Ҫ������һ��Ĵ���
			//����һ������ΪҪ����ĵ�ǰ�㣬��ǰ����ΪҪ�������һ�����ݵĶ���
			std::deque<LPTQTNODE>* temp = pDequeCurLevel;
			pDequeCurLevel = pDequeNextLevel;
			pDequeNextLevel = temp;
		}
		//���һ������ݴ���ڵ�ǰ���У�ȫ�����ǲ��ɷָ���ӽڵ㣬
		//Ҳ���Ǻ��Ӿ����ཻ�Ľڵ�
		//��ʱ���޳����ʣ��Ľڵ����ɵ��������������ڻ�������������
		triangleCount = 0;
		indexCount = 0;
		for (auto& node : *pDequeCurLevel)
		{
			node->MakeTriangle(pIndex + indexCount);
			triangleCount += 8;//ÿ��һ���ڵ�����8��������
			indexCount += 24;//ÿ��һ���ڵ�����24������������
		}
		pDequeCurLevel->clear();
	}

	void SetC1(float c1) { mLod_C1 = c1; }
	void SetC2(float c2) { mLod_C2 = c2; }

private:
	LPTQTNODE mPTreeRoot;//�Ĳ����ĸ��ڵ�
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

	//���ظ߶�ͼ
	bool LoadHeightMap(
		const char* hm,
		float unitsize,//��λ��С��һ�����ӵ����ؿ�ߣ�������֮��ļ������
		float _floor,//��С�߶�
		float _top);//���߶�

	void DrawTerrain();   // �Լ�����

	void UpdateTerrain();

	void SetC1(float c1) { mTQuadTree.SetC1(c1); }
	void SetC2(float c2) { mTQuadTree.SetC2(c2); }

protected:
	void _Clear();
	void _CalcuVertext2World();
protected:
	LPMESHDATA mPMeshData;   // ���صĵ���ģ������
	vec3f* mPVertexts;  //�����еĶ�������

	int* mPIndex;//��̬�����Ĳ������ɵ������ε�����

	int mTriangleCount;//��¼��̬������������
	int mIndexCount;//��¼��̬����������

	int mSize;  //�ݺ����--Ϊ��ʹ������
	float mRealSize;//�����ʵ�������С

	TerrainQuadTree mTQuadTree; // �����Ĳ���
};

#endif