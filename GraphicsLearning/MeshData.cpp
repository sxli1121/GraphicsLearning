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
	// 以空格为分界的文件 用文件流读取比较方便 
	if (meshfile == nullptr) { return false; }
	std::fstream filemesh;
	filemesh.open(meshfile, std::ios::in);
	if (!filemesh.is_open()) { return false; }

	_Clear();//先清楚一下旧的模型数据

	//解析模型数据
	//# v f
	//获取顶点和面索引的数据
	//在解析完成之前我们是不知道具体有多少数据
	//所以可以先使用动态的数据临时的记录解析成功的数据
	//解析完成之后再根据实际的数量分配最终的存储内存空间

	std::vector<vec3f> tempvertext;//临时顶点数据数组
	std::vector<int> tempindex;//临时索引数据数组

	//逐行解析模型数据
	std::string linestr;//一行的文本数据
	while (getline(filemesh, linestr))
	{
		//成功读取到一行数据
		if ('#' == linestr[0]) { continue; }//忽略注释

		//如果不是注释
		std::stringstream ss(linestr);
		std::string temptext;
		std::vector<std::string> linestrdata;
		while (ss >> temptext)//将一行的数据以空格为界，分为单独的部分
		{
			linestrdata.push_back(temptext);
		}
		if (linestrdata[0] == "v")
		{
			//表示为顶点数据
			if (linestrdata.size() != 4)
			{
				//数据非法，直接返回加载错误
				return false;
			}

			std::stringstream ts(linestr);
			ts >> temptext;//跳过第一个v
			vec3f temp;
			ts >> temp.x >> temp.y >> temp.z;//读取了3个float
			tempvertext.push_back(temp);//存储一个顶点数据到临时数组
		}
		else if (linestrdata[0] == "f")
		{
			//表示为面数据
			int size = (int)linestrdata.size();
			if (size < 4)
			{
				//数据非法，直接返回加载错误
				return false;
			}

			std::stringstream ts(linestr);
			ts >> temptext;//跳过f
			std::vector<int> temp;//存储的是面的索引数据
			int index;
			for (int i = 1; i < size; ++i)
			{
				ts >> index;
				temp.push_back(index);
			}

			//我们需要的是三角形的索引，不是面的索引。除非这个面就是一个三角面
			//所以这里需要来还原三角形面索引
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


	// 计算模型空间中的包围盒
	vec3f maxpoint = mPVertexts[0]; 
	vec3f minpoint = mPVertexts[0]; 

	for (int i = 1; i < mVertextCount; ++i) // 遍历所有的顶点--找到其中最大的 和 最小的xyz 
	{
		if (mPVertexts[i].x > maxpoint.x) { maxpoint.x = mPVertexts[i].x; }
		if (mPVertexts[i].y > maxpoint.y) { maxpoint.y = mPVertexts[i].y; }
		if (mPVertexts[i].z > maxpoint.z) { maxpoint.z = mPVertexts[i].z; }

		if (mPVertexts[i].x < minpoint.x) { minpoint.x = mPVertexts[i].x; }
		if (mPVertexts[i].y < minpoint.y) { minpoint.y = mPVertexts[i].y; }
		if (mPVertexts[i].z < minpoint.z) { minpoint.z = mPVertexts[i].z; }
	}

	// 用上面计算出的值组成包围盒
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