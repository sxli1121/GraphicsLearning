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
	std::vector<vec2f> tempuv;

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
		if (linestrdata[0] == "t")
		{
			if (linestrdata.size() != 2)
			{
				return false;//数据非法
			}
			std::string respath = ".\\res\\texture\\"; // 资源路径
			std::string textpath = respath + linestrdata[1];  // 获取图片的路径
			mPTexture = TextureManager::Instance().LoadTexture(textpath.c_str());  // 加载到图片管理类中
		}
		if (linestrdata[0] == "v")
		{
			//表示为顶点数据
			if (linestrdata.size() < 4)
			{
				//数据非法，直接返回加载错误
				return false;
			}

			std::stringstream ts(linestr);
			ts >> temptext;//跳过第一个v
			vec3f temp;
			ts >> temp.x >> temp.y >> temp.z;//读取了3个float
			tempvertext.push_back(temp);//存储一个顶点数据到临时数组

			//兼容读取包括UV坐标的信息
			if (linestrdata.size() >= 6)
			{
				vec2f uv;
				ts >> uv.u >> uv.v;
				tempuv.push_back(uv);
			}

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
	mPUVs = new vec2f[mVertextCount];

	memcpy(mPVertexts, &tempvertext[0], mVertextCount * sizeof(vec3f));
	memcpy(mPIndex, &tempindex[0], mIndexCount * sizeof(int));
	memcpy(mPUVs, &tempuv[0], mVertextCount * sizeof(vec2f));

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

bool MeshData::LoadGrayMap(const char* hm, float unitsize, float _floor, float _top)
{

	MyTexture2D graymap;
	if (graymap.LoadTexture(hm))
	{
		_Clear();
		const int& w = graymap.Width();
		const int& h = graymap.Height();

		//总共宽w个像素，高h个像素，那么点的总数为 w*h
		//分成的格子数量为 (w-1)*(h-1)

		mVertextCount = w * h;
		mTriangleCount = (w - 1) * (h - 1) * 2;
		mIndexCount = mTriangleCount * 3;

		mPVertexts = new vec3f[mVertextCount];
		mPIndex = new int[mIndexCount];
		mPUVs = new vec2f[mVertextCount];

		float cz, cx, cy;//顶点坐标
		float fg;//灰度比例

		// 根据灰度值 更新uv 三角形的顶点坐标
		for (int z = 0; z < h; ++z)    // 此时的平面坐标是x和z
		{
			cz = unitsize * (h - 1 - z);   // 当前的h是在z方向上的值  当前的z的值 = 每一个格子的宽度 * （格子的数量 - z）  倒着的
			for (int x = 0; x < w; ++x)
			{
				cx = unitsize * x;  
				fg = graymap.GetPixelGray(x, z) / 255.0f;
				cy = _floor + (_top - _floor) * fg;   // 修改高度的值--根据灰度值
				mPVertexts[z * w + x] = vec3f(cx, cy, cz);
				mPUVs[z * w + x] = vec2f(float(x) / (w - 1), float(z) / (h - 1));  // 将uv值限定在0-1之间
			}
		}

		// 更新索引 -- 根据网格的数量
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
	//计算模型空间的包围盒数
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