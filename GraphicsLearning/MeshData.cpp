#include "MeshData.h"
#include "UtilityHelper.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>


MeshData::MeshData()
{
	mVertextCount = 0;
	mIndexCount = 0;
	mTriangleCount = 0;
	mPVertexts = nullptr;
	mPIndex = nullptr;
}

MeshData::~MeshData()
{
	_Clear();
}

MeshData::MeshData(const MeshData& that)
{
	_Copy(that);
}

MeshData& MeshData::operator=(const MeshData& that)
{
	if (this != &that)
	{
		_Clear();
		_Copy(that);
	}
	return *this;
}


bool MeshData::LoadFromFile(const char* meshfile)
{
	// �Կո�Ϊ�ֽ���ļ� ���ļ�����ȡ�ȽϷ��� 
	if (meshfile == nullptr) { return false; }
	std::fstream filemesh;
	filemesh.open(meshfile, std::ios::in);
	if (!filemesh.is_open()) { return false; }

	_Clear();//�����һ�¾ɵ�ģ������

	//����ģ������
	//# v f
	//��ȡ�����������������
	//�ڽ������֮ǰ�����ǲ�֪�������ж�������
	//���Կ�����ʹ�ö�̬��������ʱ�ļ�¼�����ɹ�������
	//�������֮���ٸ���ʵ�ʵ������������յĴ洢�ڴ�ռ�

	std::vector<vec3f> tempvertext;//��ʱ������������
	std::vector<int> tempindex;//��ʱ������������

	//���н���ģ������
	std::string linestr;//һ�е��ı�����
	while (getline(filemesh, linestr))
	{
		//�ɹ���ȡ��һ������
		if ('#' == linestr[0]) { continue; }//����ע��

		//�������ע��
		std::stringstream ss(linestr);
		std::string temptext;
		std::vector<std::string> linestrdata;
		while (ss >> temptext)//��һ�е������Կո�Ϊ�磬��Ϊ�����Ĳ���
		{
			linestrdata.push_back(temptext);
		}
		if (linestrdata[0] == "v")
		{
			//��ʾΪ��������
			if (linestrdata.size() != 4)
			{
				//���ݷǷ���ֱ�ӷ��ؼ��ش���
				return false;
			}

			std::stringstream ts(linestr);
			ts >> temptext;//������һ��v
			vec3f temp;
			ts >> temp.x >> temp.y >> temp.z;//��ȡ��3��float
			tempvertext.push_back(temp);//�洢һ���������ݵ���ʱ����
		}
		else if (linestrdata[0] == "f")
		{
			//��ʾΪ������
			int size = (int)linestrdata.size();
			if (size < 4)
			{
				//���ݷǷ���ֱ�ӷ��ؼ��ش���
				return false;
			}

			std::stringstream ts(linestr);
			ts >> temptext;//����f
			std::vector<int> temp;//�洢���������������
			int index;
			for (int i = 1; i < size; ++i)
			{
				ts >> index;
				temp.push_back(index);
			}

			//������Ҫ���������ε��������������������������������һ��������
			//����������Ҫ����ԭ������������
			size = (int)temp.size() - 1;
			for (int i = 1; i < size; ++i)
			{
				tempindex.push_back(temp[0]);
				tempindex.push_back(temp[i]);
				tempindex.push_back(temp[i + 1]);
			}
		}
	}

	mVertextCount = (int)tempvertext.size();
	mIndexCount = (int)tempindex.size();
	mTriangleCount = mIndexCount / 3;

	if (0 == mVertextCount || 0 == mIndexCount) { return false; }

	mPVertexts = new vec3f[mVertextCount];
	mPIndex = new int[mIndexCount];

	memcpy(mPVertexts, &tempvertext[0], mVertextCount * sizeof(vec3f));
	memcpy(mPIndex, &tempindex[0], mIndexCount * sizeof(int));


	// ����ģ�Ϳռ��еİ�Χ��
	vec3f maxpoint = mPVertexts[0]; 
	vec3f minpoint = mPVertexts[0]; 

	for (int i = 1; i < mVertextCount; ++i) // �������еĶ���--�ҵ��������� �� ��С��xyz 
	{
		if (mPVertexts[i].x > maxpoint.x) { maxpoint.x = mPVertexts[i].x; }
		if (mPVertexts[i].y > maxpoint.y) { maxpoint.y = mPVertexts[i].y; }
		if (mPVertexts[i].z > maxpoint.z) { maxpoint.z = mPVertexts[i].z; }

		if (mPVertexts[i].x < minpoint.x) { minpoint.x = mPVertexts[i].x; }
		if (mPVertexts[i].y < minpoint.y) { minpoint.y = mPVertexts[i].y; }
		if (mPVertexts[i].z < minpoint.z) { minpoint.z = mPVertexts[i].z; }
	}

	// ������������ֵ��ɰ�Χ��
	mBoundingBox[0] = vec3f(maxpoint.x, maxpoint.y, maxpoint.z);
	mBoundingBox[1] = vec3f(maxpoint.x, maxpoint.y, minpoint.z);
	mBoundingBox[2] = vec3f(minpoint.x, maxpoint.y, minpoint.z);
	mBoundingBox[3] = vec3f(minpoint.x, maxpoint.y, maxpoint.z);
	mBoundingBox[4] = vec3f(maxpoint.x, minpoint.y, maxpoint.z);
	mBoundingBox[5] = vec3f(maxpoint.x, minpoint.y, minpoint.z);
	mBoundingBox[6] = vec3f(minpoint.x, minpoint.y, maxpoint.z);
	mBoundingBox[7] = vec3f(minpoint.x, minpoint.y, minpoint.z);

	return true;



}

void MeshData::_Clear()
{
	SAFE_DELETE_ARRAY_PTR(mPVertexts);
	SAFE_DELETE_ARRAY_PTR(mPIndex);
}

void MeshData::_Copy(const MeshData& that)
{
	memcpy(this, &that, sizeof(MeshData));
	mPVertexts = nullptr;
	mPIndex = nullptr;
	if (that.mPVertexts != nullptr)
	{
		mPVertexts = new vec3f[mVertextCount];
		memcpy(mPVertexts, that.mPVertexts, sizeof(vec3f) * mVertextCount);
	}

	if (that.mPIndex != nullptr)
	{
		mPIndex = new int[mIndexCount];
		memcpy(mPIndex, that.mPIndex, sizeof(int) * mIndexCount);
	}

	memcpy(mBoundingBox, that.mBoundingBox, sizeof(vec3f) * 8);
}