#include "MeshData.h"
#include "UtilityHelper.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include "MyTexture2D.h"
#include "math3d/Matrix4x4.h"


MeshData::MeshData()
{
	mVertextCount = 0;
	mIndexCount = 0;
	mTriangleCount = 0;
	mPVertexts = nullptr;
	mPIndex = nullptr;
	mPUVs = nullptr;
	mPTexture = nullptr;

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
	std::vector<vec2f> tempuv;

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
		if (linestrdata[0] == "t")
		{
			if (linestrdata.size() != 2)
			{
				return false;//���ݷǷ�
			}
			std::string respath = ".\\res\\texture\\"; // ��Դ·��
			std::string textpath = respath + linestrdata[1];  // ��ȡͼƬ��·��
			mPTexture = TextureManager::Instance().LoadTexture(textpath.c_str());  // ���ص�ͼƬ��������
		}
		if (linestrdata[0] == "v")
		{
			//��ʾΪ��������
			if (linestrdata.size() < 4)
			{
				//���ݷǷ���ֱ�ӷ��ؼ��ش���
				return false;
			}

			std::stringstream ts(linestr);
			ts >> temptext;//������һ��v
			vec3f temp;
			ts >> temp.x >> temp.y >> temp.z;//��ȡ��3��float
			tempvertext.push_back(temp);//�洢һ���������ݵ���ʱ����

			//���ݶ�ȡ����UV�������Ϣ
			if (linestrdata.size() >= 6)
			{
				vec2f uv;
				ts >> uv.u >> uv.v;
				tempuv.push_back(uv);
			}

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
	mPUVs = new vec2f[mVertextCount];

	memcpy(mPVertexts, &tempvertext[0], mVertextCount * sizeof(vec3f));
	memcpy(mPIndex, &tempindex[0], mIndexCount * sizeof(int));
	memcpy(mPUVs, &tempuv[0], mVertextCount * sizeof(vec2f));

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

bool MeshData::LoadGrayMap(const char* hm, float unitsize, float _floor, float _top)
{

	MyTexture2D graymap;
	if (graymap.LoadTexture(hm))
	{
		_Clear();
		const int& w = graymap.Width();
		const int& h = graymap.Height();

		//�ܹ���w�����أ���h�����أ���ô�������Ϊ w*h
		//�ֳɵĸ�������Ϊ (w-1)*(h-1)

		mVertextCount = w * h;
		mTriangleCount = (w - 1) * (h - 1) * 2;
		mIndexCount = mTriangleCount * 3;

		mPVertexts = new vec3f[mVertextCount];
		mPIndex = new int[mIndexCount];
		mPUVs = new vec2f[mVertextCount];

		float cz, cx, cy;//��������
		float fg;//�Ҷȱ���

		// ���ݻҶ�ֵ ����uv �����εĶ�������
		for (int z = 0; z < h; ++z)    // ��ʱ��ƽ��������x��z
		{
			cz = unitsize * (h - 1 - z);   // ��ǰ��h����z�����ϵ�ֵ  ��ǰ��z��ֵ = ÿһ�����ӵĿ�� * �����ӵ����� - z��  ���ŵ�
			for (int x = 0; x < w; ++x)
			{
				cx = unitsize * x;  
				fg = graymap.GetPixelGray(x, z) / 255.0f;
				cy = _floor + (_top - _floor) * fg;   // �޸ĸ߶ȵ�ֵ--���ݻҶ�ֵ
				mPVertexts[z * w + x] = vec3f(cx, cy, cz);
				mPUVs[z * w + x] = vec2f(float(x) / (w - 1), float(z) / (h - 1));  // ��uvֵ�޶���0-1֮��
			}
		}

		// �������� -- �������������
		int* pindex = nullptr;
		for (int i = 0; i < (h - 1); ++i)
		{
			for (int j = 0; j < (w - 1); ++j)
			{
				pindex = mPIndex + (i * (w - 1) + j) * 6;
				pindex[0] = i * w + j;
				pindex[1] = pindex[0] + 1;
				pindex[2] = pindex[1] + w;
				pindex[3] = i * w + j;
				pindex[4] = pindex[1] + w;
				pindex[5] = pindex[0] + w;
			}
		}
		return true;
	}
	return false;
}


void MeshData::_CalcBoundingBox()
{
	//����ģ�Ϳռ�İ�Χ����
	vec3f maxpoint = mPVertexts[0];
	vec3f minpoint = mPVertexts[0];

	for (int i = 1; i < mVertextCount; ++i)
	{
		if (mPVertexts[i].x > maxpoint.x) { maxpoint.x = mPVertexts[i].x; }
		if (mPVertexts[i].y > maxpoint.y) { maxpoint.y = mPVertexts[i].y; }
		if (mPVertexts[i].z > maxpoint.z) { maxpoint.z = mPVertexts[i].z; }

		if (mPVertexts[i].x < minpoint.x) { minpoint.x = mPVertexts[i].x; }
		if (mPVertexts[i].y < minpoint.y) { minpoint.y = mPVertexts[i].y; }
		if (mPVertexts[i].z < minpoint.z) { minpoint.z = mPVertexts[i].z; }
	}

	mBoundingBox[0] = vec3f(maxpoint.x, maxpoint.y, maxpoint.z);
	mBoundingBox[1] = vec3f(maxpoint.x, maxpoint.y, minpoint.z);
	mBoundingBox[2] = vec3f(minpoint.x, maxpoint.y, minpoint.z);
	mBoundingBox[3] = vec3f(minpoint.x, maxpoint.y, maxpoint.z);
	mBoundingBox[4] = vec3f(maxpoint.x, minpoint.y, maxpoint.z);
	mBoundingBox[5] = vec3f(maxpoint.x, minpoint.y, minpoint.z);
	mBoundingBox[6] = vec3f(minpoint.x, minpoint.y, maxpoint.z);
	mBoundingBox[7] = vec3f(minpoint.x, minpoint.y, minpoint.z);
}

void MeshData::_Clear()
{
	SAFE_DELETE_ARRAY_PTR(mPVertexts);
	SAFE_DELETE_ARRAY_PTR(mPIndex);
	SAFE_DELETE_ARRAY_PTR(mPUVs);

	mVertextCount = 0;
	mIndexCount = 0;
	mTriangleCount = 0;
	mPTexture = nullptr;
}

void MeshData::_Copy(const MeshData& that)
{
	memcpy(this, &that, sizeof(MeshData));
	mPVertexts = nullptr;
	mPIndex = nullptr;
	mPUVs = nullptr;
	mPTexture = that.mPTexture;
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

	if (that.mPUVs != nullptr)
	{
		mPUVs = new vec2f[mVertextCount];
		memcpy(mPUVs, that.mPUVs, sizeof(vec2f) * mVertextCount);
	}

	memcpy(mBoundingBox, that.mBoundingBox, sizeof(vec3f) * 8);

}