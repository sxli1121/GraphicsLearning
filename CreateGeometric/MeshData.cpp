#include "MeshData.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "CommonDefine.h"

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
	if (this!=&that)
	{
		_Clear();
		_Copy(that);
	}
	return *this;
}

void MeshData::CreateStar(float radius1, float radius2, float height, size_t side)
{
	_Clear();//清除旧的数据
	if (side<2){side = 2;}
	radius1 = IS_FLOAT_ZERO(radius1) ? 1.0f : abs(radius1);
	radius2 = IS_FLOAT_ZERO(radius2) ? 0.5f : abs(radius2);
	height = IS_FLOAT_ZERO(height) ? 1.0f : abs(height);

	mVertextCount = side * 2 + 2;
	mTriangleCount = side * 2 * 2;//中间的三角形+ 上底面的三角形数量 + 下底面的三角形
	mIndexCount = mTriangleCount * 3;

	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	Matrix4x4 mat, mat2;
	mat.SetRotateY_d(-360.0f/side);//因为我们的Y轴是反的，所以反着转
	mat2.SetRotateY_d(-360.0f/side*0.5f);

	mPVertexts[0] = vector3d::Forward() * radius1;
	mPVertexts[1] = vector3d::Forward() * radius2 * mat2;

	for (int i=1;i<side;++i)
	{
		mPVertexts[i * 2] = mPVertexts[(i - 1) * 2] * mat;
		mPVertexts[i * 2+1] = mPVertexts[(i - 1) * 2+1] * mat;
	}

	mPVertexts[mVertextCount - 2] = vector3d(0, height * 0.5f, 0);//最上面的顶点
	mPVertexts[mVertextCount - 1] = vector3d(0, -height * 0.5f, 0);//最下面的顶点

	int* pTopIndex = mPIndex;
	int* pBottomIndex = mPIndex + side * 2 * 3;
	for (int i=0;i<side*2;++i)
	{
		int* pCurTopIndex = pTopIndex + i * 3;
		int* pCurBottomIndex = pBottomIndex + i * 3;

		pCurTopIndex[0] = mVertextCount - 2;
		pCurTopIndex[1] = (i + 1) % (side * 2);
		pCurTopIndex[2] = i;

		pCurBottomIndex[0] = mVertextCount - 1;
		pCurBottomIndex[1] = i;
		pCurBottomIndex[2] = (i + 1) % (side * 2);
	}
	_CalcBoundingBox();
}

void MeshData::CreateDiamond(float radius1, float radius2, float height1, float height2, size_t side)
{
	_Clear();//清除旧的数据
//我们的模型创建任务就是把顶点和三角形计算清楚
	if (side < 3) { side = 3; }
	radius1 = IS_FLOAT_ZERO(radius1) ? 0.3f : abs(radius1);
	height1 = IS_FLOAT_ZERO(height1) ? 0.1f : abs(height1);

	radius2 = IS_FLOAT_ZERO(radius2) ? 0.5f : abs(radius2);
	height2 = IS_FLOAT_ZERO(height2) ? 0.3f : abs(height2);

	mVertextCount = side * 2 + 1;//边数 x 2 + 1 加上最后的顶点
	mTriangleCount = side * 2 + (side - 2) + side;//中间的三角形+ 上底面的三角形数量 + 下底面的三角形
	mIndexCount = mTriangleCount * 3;

	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	Matrix4x4 mat,mat2;
	mat.SetRotateY_d(-360.0f / side);//因为我们的Y轴是反的，所以反着转
	mat2.SetRotateY_d(360.0f / side * 0.5f);

	mPVertexts[0] = vector3d::Forward() * radius1;
	mPVertexts[0].y = height1;
	mPVertexts[side] = vector3d::Forward() * radius2 * mat2;
	mPVertexts[mVertextCount - 1] = vector3d(0, -height2, 0);//最下面的顶点

	//顶点计算完毕
	for (int i = 1; i < side; ++i)
	{
		mPVertexts[i] = mPVertexts[i - 1] * mat;
		mPVertexts[i + side] = mPVertexts[i + side-1]*mat;
	}
	//中间的网格索引处理
	int* pCenterIndex = mPIndex;
	for (int i = 0; i < side; ++i)
	{
		int* pCurindex = pCenterIndex + i * 6;
		pCurindex[0] = i;
		pCurindex[1] = (i + 1) % side;
		pCurindex[2] = pCurindex[1] + side;
		pCurindex[3] = i;
		pCurindex[4] = pCurindex[2];
		pCurindex[5] = i + side;
	}

	int* pTopIndex = mPIndex + side * 6;
	for (int i = 0; i < side - 2; ++i)
	{
		int* curTopIndex = pTopIndex + i * 3;
		curTopIndex[0] = 0;
		curTopIndex[1] = i + 2;
		curTopIndex[2] = i + 1;
	}
	int* pBottomIndex = pTopIndex + (side - 2) * 3;
	for (int i=0;i<side;++i)
	{
		int* curBottomIndex = pBottomIndex + i * 3;
		curBottomIndex[0] = mVertextCount - 1;
		curBottomIndex[1] = i + side;
		curBottomIndex[2] = (i + 1)%side + side;
	}

	_CalcBoundingBox();

}

void MeshData::CreatePyramid(float radius, float height, size_t side)
{
	_Clear();//清除旧的数据
	//我们的模型创建任务就是把顶点和三角形计算清楚
	if (side < 3) { side = 3; }
	radius = IS_FLOAT_ZERO(radius) ? 1.0f : abs(radius);
	height = IS_FLOAT_ZERO(height) ? 1.0f : abs(height);

	mVertextCount = side + 1;
	mTriangleCount = side + (side - 2);
	mIndexCount = mTriangleCount * 3;

	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	Matrix4x4 mat;
	mat.SetRotateY_d(-360.0f / side);//因为我们的Y轴是反的，所以反着转

	mPVertexts[0] = vector3d::Forward() * radius;
	mPVertexts[side] = vector3d(0,height,0);

	//顶点计算完毕
	for (int i = 1; i < side; ++i)
	{
		mPVertexts[i] = mPVertexts[i - 1] * mat;
	}
	int* pBottomIndex = mPIndex;
	for (int i = 0; i < side - 2; ++i)
	{
		int* curBottomIndex = pBottomIndex + i * 3;
		curBottomIndex[0] = 0;
		curBottomIndex[1] = i + 1;
		curBottomIndex[2] = i + 2;
	}

	int* pTopIndex = mPIndex + (side - 2) * 3;
	for (int i = 0; i < side; ++i)
	{
		int* curTopIndex = pTopIndex + i * 3;
		curTopIndex[0] = mVertextCount - 1;
		curTopIndex[1] = (i + 1) % side;
		curTopIndex[2] = i;
	}

	_CalcBoundingBox();
}

void MeshData::_Clear()
{
	SAFE_DELETE_ARRAY_PTR(mPVertexts);
	SAFE_DELETE_ARRAY_PTR(mPIndex);

	mVertextCount = 0;
	mIndexCount = 0;
	mTriangleCount = 0;
}

void MeshData::_Copy(const MeshData& that)
{
	memcpy(this, &that, sizeof(MeshData));
	mPVertexts = nullptr;
	mPIndex = nullptr;
	if (that.mPVertexts != nullptr)
	{
		mPVertexts = new vector3d[mVertextCount];
		memcpy(mPVertexts, that.mPVertexts, sizeof(vector3d) * mVertextCount);
	}

	if (that.mPIndex != nullptr)
	{
		mPIndex = new int[mIndexCount];
		memcpy(mPIndex, that.mPIndex, sizeof(int) * mIndexCount);
	}

	memcpy(mBoundingBox, that.mBoundingBox, sizeof(vector3d) * 8);
}

void MeshData::_CalcBoundingBox()
{
	//计算模型空间的包围盒数
	vector3d maxpoint = mPVertexts[0];
	vector3d minpoint = mPVertexts[0];

	for (int i = 1; i < mVertextCount; ++i)
	{
		if (mPVertexts[i].x > maxpoint.x) { maxpoint.x = mPVertexts[i].x; }
		if (mPVertexts[i].y > maxpoint.y) { maxpoint.y = mPVertexts[i].y; }
		if (mPVertexts[i].z > maxpoint.z) { maxpoint.z = mPVertexts[i].z; }

		if (mPVertexts[i].x < minpoint.x) { minpoint.x = mPVertexts[i].x; }
		if (mPVertexts[i].y < minpoint.y) { minpoint.y = mPVertexts[i].y; }
		if (mPVertexts[i].z < minpoint.z) { minpoint.z = mPVertexts[i].z; }
	}

	mBoundingBox[0] = vector3d(maxpoint.x, maxpoint.y, maxpoint.z);
	mBoundingBox[1] = vector3d(maxpoint.x, maxpoint.y, minpoint.z);
	mBoundingBox[2] = vector3d(minpoint.x, maxpoint.y, minpoint.z);
	mBoundingBox[3] = vector3d(minpoint.x, maxpoint.y, maxpoint.z);
	mBoundingBox[4] = vector3d(maxpoint.x, minpoint.y, maxpoint.z);
	mBoundingBox[5] = vector3d(maxpoint.x, minpoint.y, minpoint.z);
	mBoundingBox[6] = vector3d(minpoint.x, minpoint.y, maxpoint.z);
	mBoundingBox[7] = vector3d(minpoint.x, minpoint.y, minpoint.z);
}

void MeshData::_InvNormal()
{
	for (int i = 0; i < mTriangleCount; ++i)
	{
		mPIndex[i * 3 + 1] = mPIndex[i * 3 + 1] ^ mPIndex[i * 3 + 2];
		mPIndex[i * 3 + 2] = mPIndex[i * 3 + 1] ^ mPIndex[i * 3 + 2];
		mPIndex[i * 3 + 1] = mPIndex[i * 3 + 1] ^ mPIndex[i * 3 + 2];
	}
}

bool MeshData::LoadFromFile(const char* meshfile)
{
	if (meshfile == nullptr) { return false; }
	std::fstream filemesh;//文件流对象
	filemesh.open(meshfile);
	if (!filemesh.is_open()) { return false; }
	_Clear();//要重新加载文件了，所以先清除之前的内容,预防重复加载

	//解析模型的数据
	//# v f
	//获取顶点和面索引的数据
	//在解析完成之前我们是不知道具体有多少数据的
	//所以可以先使用动态数组临时的记录解析成功的数据
	//解析完成之后再根据实际的数量分配最终存储的内存

	std::vector<vector3d> tempvertext;//临时记录顶点数据的动态数组
	std::vector<int> tempindex;//临时记录索引数据的动态数组

	//逐行的解析模型的数据
	std::string linestr;//一行数据文本内容
	while (getline(filemesh,linestr))
	{
		//成功读取到一行的文本
		if ('#' == linestr[0]){continue;}//忽略注释行

		//如果不是注释
		std::stringstream ss(linestr);//生成了一个字符串流对象
		std::string temptext;
		std::vector<std::string> linestrdata;
		while (ss>>temptext)
		{
			linestrdata.push_back(temptext);
		}
		if (linestrdata[0] == "v")//注意比较的是字符串的内容了
		{
			if (linestrdata.size()!=4){
				return false;
			}//非法数据

			std::stringstream ts(linestr);
			ts >> temptext;//跳过开头的v
			vector3d temp;
			ts >> temp.x >> temp.y >> temp.z;//读入了三个float
			tempvertext.push_back(temp);
		}
		else if (linestrdata[0] == "f")//注意比较的是字符串的内容了
		{
			if (linestrdata.size() < 4) {
				return false;
			}//非法数据

			std::stringstream ts(linestr);
			ts >> temptext;//跳过开头的f
			std::vector<int> temp;
			int index;
			int size = int(linestrdata.size());

			for (int i=1;i<size;++i)
			{
				ts >> index;
				temp.push_back(index);
			}

			//temp中将面的顶点索引记录了下来
			//我们需要将面的数据还原成三角形的数据
			size = (int)temp.size() - 1;
			for (int i=1;i<size;++i)
			{
				tempindex.push_back(temp[0]);
				tempindex.push_back(temp[i]);
				tempindex.push_back(temp[i+1]);
			}
		}
	}

	mVertextCount = (int)tempvertext.size();
	mIndexCount = (int)tempindex.size();
	mTriangleCount = mIndexCount / 3;

	if (0 == mVertextCount || 0 == mIndexCount) { return false; }

	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	//vector的元素内存是连续排列的
	memcpy(mPVertexts, &tempvertext[0], mVertextCount * sizeof(vector3d));
	memcpy(mPIndex, &tempindex[0], mIndexCount * sizeof(int));


	_CalcBoundingBox();
	return true;
}

bool MeshData::Save2File(const char* meshfile)
{
	if (meshfile == nullptr) { return false; }
	FILE* pfile = nullptr;
	fopen_s(&pfile, meshfile, "wb+");
	if (pfile == nullptr){return false;}

	if (mPVertexts !=nullptr && mPIndex!= nullptr)
	{
		char buffer[256] = {};
		for (int i=0;i<mVertextCount;++i)
		{
			sprintf_s(buffer, 256, "v %f %f %f\n", mPVertexts[i].x, mPVertexts[i].y, mPVertexts[i].z);
			fwrite(buffer,strlen(buffer),1,pfile);
		}


		for (int i=0;i<mTriangleCount;++i)
		{
			sprintf_s(buffer, 256, "f %d %d %d\n", mPIndex[i * 3], mPIndex[i * 3 + 1], mPIndex[i * 3 + 2]);
			fwrite(buffer, strlen(buffer), 1, pfile);
		}
		fclose(pfile);
		pfile = nullptr;
		return true;
	}
	fclose(pfile);
	pfile = nullptr;
	return false;
}

void MeshData::CreatePlane(size_t size_x, size_t size_y, size_t precision)
{
	_Clear();//清除旧的数据
	//我们的模型创建任务就是把顶点和三角形计算清楚
	if (precision == 0) { precision = 1; }
	if (size_x == 0) { size_x = 1; }
	if (size_y == 0) { size_y = 1; }

	//计算数量
	mVertextCount = (precision + 1) * (precision + 1);
	mTriangleCount = precision * precision * 2;
	mIndexCount = mTriangleCount * 3;
	//分配空间
	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	//计算点的坐标
	float lx = -float(size_x) * 0.5f;
	float ty = size_y * 0.5f;
	float dx = size_x /float( precision);//格子的x大小
	float dy = size_y / float(precision);//格子的y大小

	//处理顶点的赋值
	for (int y=0;y<precision+1;++y)
	{
		for (int x=0;x<precision+1;++x)
		{
			mPVertexts[y * (precision + 1) + x] = vector3d(lx + dx * x, 0, ty - dy * y);
		}
	}

	//处理索引，按照精度也就是格子的数量处理
	for (int i = 0;i<precision;++i)
	{
		int* pGridLineIndex = mPIndex + i * precision * (3 * 2);
		for (int j=0;j<precision;++j)
		{
			int* pGridindex = pGridLineIndex + j * 6;
			pGridindex[0] = i * (precision + 1) + j;
			pGridindex[1] = pGridindex[0] + 1;
			pGridindex[2] = pGridindex[1] + (precision + 1);
			pGridindex[3] = pGridindex[0];
			pGridindex[4] = pGridindex[2];
			pGridindex[5] = pGridindex[0] + (precision + 1);
		}
	}

	_CalcBoundingBox();
}

void MeshData::CreateCube(size_t size)
{
	_Clear();//清除旧的数据
	//我们的模型创建任务就是把顶点和三角形计算清楚
	if (size == 0) { size = 1; }

	mVertextCount = 8;
	mTriangleCount = 12;
	mIndexCount = 36;

	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	float halfsize = size * 0.5f;
	mPVertexts[0] = vector3d(-halfsize, +halfsize, +halfsize);
	mPVertexts[1] = vector3d(+halfsize, +halfsize, +halfsize);
	mPVertexts[2] = vector3d(+halfsize, +halfsize, -halfsize);
	mPVertexts[3] = vector3d(-halfsize, +halfsize, -halfsize);

	mPVertexts[4] = vector3d(-halfsize, -halfsize, +halfsize);
	mPVertexts[5] = vector3d(+halfsize, -halfsize, +halfsize);
	mPVertexts[6] = vector3d(+halfsize, -halfsize, -halfsize);
	mPVertexts[7] = vector3d(-halfsize, -halfsize, -halfsize);

	int index[36] = {
		0,1,2,0,2,3,4,7,6,4,6,5,0,4,5,0,5,1,2,6,7,2,7,3,2,1,5,2,5,6,0,3,7,0,7,4
	};
	memcpy(mPIndex, index, 36 * sizeof(int));

	_CalcBoundingBox();
}

void MeshData::CreateCylinder(float radius, float height, size_t side)
{
	_Clear();//清除旧的数据
	//我们的模型创建任务就是把顶点和三角形计算清楚
	if (side < 3) { side = 3; }
	radius = IS_FLOAT_ZERO(radius) ? 1.0f : abs(radius);
	height = IS_FLOAT_ZERO(height) ? 1.0f : abs(height);

	mVertextCount = side * 2;//边数 x 2：上下底面的顶点数一致
	mTriangleCount = side * 2 + (side - 2) * 2;//中间的三角形+ 上下底面的三角形数量
	mIndexCount = mTriangleCount * 3;

	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	Matrix4x4 mat;
	mat.SetRotateY_d(-360.0f/side);//因为我们的Y轴是反的，所以反着转

	mPVertexts[0] = vector3d::Forward() * radius;
	mPVertexts[0].y = height * 0.5f;
	mPVertexts[side] = mPVertexts[0];
	mPVertexts[side].y = -height * 0.5f;

	//顶点计算完毕
	for (int i = 1;i<side;++i)
	{
		mPVertexts[i] = mPVertexts[i - 1] * mat;
		mPVertexts[i + side] = mPVertexts[i];
		mPVertexts[i + side].y = -height * 0.5f;
	}
	//中间的网格索引处理
	int* pCenterIndex = mPIndex;
	for (int i = 0;i<side;++i)
	{
		int* pCurindex = pCenterIndex + i * 6;
		pCurindex[0] = i;
		pCurindex[1] = (i + 1)%side;
		pCurindex[2] = pCurindex[1] + side;
		pCurindex[3] = i;
		pCurindex[4] = pCurindex[2];
		pCurindex[5] = i+side;
	}

	int* pTopIndex = mPIndex + side * 6;
	int* pBottomIndex = pTopIndex + (side - 2) * 3;

	for (int i=0;i<side - 2;++i)
	{
		int* curTopIndex = pTopIndex + i * 3;
		int* curBottomIndex = pBottomIndex + i * 3;

		curTopIndex[0] = 0;
		curTopIndex[1] = i+2;
		curTopIndex[2] = i+1;

		curBottomIndex[0] = side;
		curBottomIndex[1] = side + i + 1;
		curBottomIndex[2] = side + i + 2;
	}

	_CalcBoundingBox();
}

void MeshData::CreateCapsule(float radius, float height, size_t precision)
{
	_Clear();//清除旧的数据
	//我们的模型创建任务就是把顶点和三角形计算清楚
	if (precision < 4) { precision = 4; }
	if (precision % 2) { precision = precision + 1; }
	radius = IS_FLOAT_ZERO(radius) ? 0.5f : abs(radius);
	height = IS_FLOAT_ZERO(height) ? 1.0f : abs(height);

	//上半球，由五个平面组成
	//一个平面由precision*precision 个格子组成
	//四个平面由precision*precision/2个格子组成

	//顶平面为(precision+1)*(precision+1)个顶点
	int vertextsCountOfPlane0 = (precision + 1) * (precision + 1);
	//四个平面为(precision+1)*(precision/2+1)*4个顶点
	int vertextsCountOfPlane1_4 = (precision + 1) * (precision / 2 + 1) * 4;
	//圆柱的上下圆周点数 = precision*precision * 2 个顶点
	int vertextsCountOfCylinder = precision * precision * 2;
	//总的顶点数 = 上下半球顶点数 + 圆柱上下顶点数（主要是为了方便计算，顶点重复也无所谓）
	mVertextCount = (vertextsCountOfPlane0 + vertextsCountOfPlane1_4) * 2 + vertextsCountOfCylinder;

	int triCountOfPlane0 = precision * precision * 2;
	int triCountOfPlane1_4 = (precision * (precision / 2) * 2) * 4;
	int triCountOfCylinder = precision * precision * 2;

	mTriangleCount = (triCountOfPlane0 + triCountOfPlane1_4) * 2 + triCountOfCylinder;
	mIndexCount = mTriangleCount * 3;

	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	memset(mPVertexts, 0, sizeof(vector3d) * mVertextCount);
	memset(mPIndex, 0, sizeof(int) * mIndexCount);

	float size_x = radius * 2, size_y = radius * 2;

	float lx = -size_x * 0.5f;
	float ty = size_y * 0.5f;
	float dx = size_x / float(precision);//格子的x大小
	float dy = size_y / float(precision);//格子的y大小

	vector3d* pPlane0Vertexts = mPVertexts;
	int* pPlane0Index = mPIndex;
	//处理顶点的赋值
	for (int z = 0; z < precision + 1; ++z)
	{
		for (int x = 0; x < precision + 1; ++x)
		{
			pPlane0Vertexts[z * (precision + 1) + x] = vector3d(lx + dx * x, radius, ty - dy * z);
		}
	}

	//处理顶平面索引，按照精度也就是格子的数量处理
	for (int i = 0; i < precision; ++i)
	{
		int* pGridLineIndex = pPlane0Index + i * precision * (3 * 2);
		for (int j = 0; j < precision; ++j)
		{
			int* pGridindex = pGridLineIndex + j * 6;
			pGridindex[0] = i * (precision + 1) + j;
			pGridindex[1] = pGridindex[0] + 1;
			pGridindex[2] = pGridindex[1] + (precision + 1);
			pGridindex[3] = pGridindex[0];
			pGridindex[4] = pGridindex[2];
			pGridindex[5] = pGridindex[0] + (precision + 1);
		}
	}

	//平面1的数据处理
	vector3d* pPlane1Vertexts = pPlane0Vertexts + vertextsCountOfPlane0;
	int* pPlane1Index = pPlane0Index + triCountOfPlane0 * 3;
	int indexOffset = vertextsCountOfPlane0;
	//处理顶点的赋值
	for (int y = 0; y < precision / 2 + 1; ++y)
	{
		for (int x = 0; x < precision + 1; ++x)
		{
			pPlane1Vertexts[y * (precision + 1) + x] = vector3d(lx + dx * x, ty - dy * y, -radius);
		}
	}

	//处理顶平面索引，按照精度也就是格子的数量处理
	for (int i = 0; i < precision / 2; ++i)
	{
		int* pGridLineIndex = pPlane1Index + i * precision * (3 * 2);
		for (int j = 0; j < precision; ++j)
		{
			int* pGridindex = pGridLineIndex + j * 6;
			pGridindex[0] = i * (precision + 1) + j;
			pGridindex[1] = pGridindex[0] + 1;
			pGridindex[2] = pGridindex[1] + (precision + 1);
			pGridindex[3] = pGridindex[0];
			pGridindex[4] = pGridindex[2];
			pGridindex[5] = pGridindex[0] + (precision + 1);
			for (int k = 0; k < 6; ++k)
			{
				pGridindex[k] += indexOffset;
			}
		}
	}

	//平面2的数据处理
	vector3d* pPlane2Vertexts = pPlane1Vertexts + (precision + 1) * (precision / 2 + 1);
	int* pPlane2Index = pPlane1Index + ((precision * (precision / 2) * 2)) * 3;
	indexOffset += (precision + 1) * (precision / 2 + 1);
	//处理顶点的赋值
	for (int y = 0; y < precision / 2 + 1; ++y)
	{
		for (int z = 0; z < precision + 1; ++z)
		{
			pPlane2Vertexts[y * (precision + 1) + z] = vector3d(radius, ty - dy * y, lx + dx * z);
		}
	}

	//处理顶平面索引，按照精度也就是格子的数量处理
	for (int i = 0; i < precision / 2; ++i)
	{
		int* pGridLineIndex = pPlane2Index + i * precision * (3 * 2);
		for (int j = 0; j < precision; ++j)
		{
			int* pGridindex = pGridLineIndex + j * 6;
			pGridindex[0] = i * (precision + 1) + j;
			pGridindex[1] = pGridindex[0] + 1;
			pGridindex[2] = pGridindex[1] + (precision + 1);
			pGridindex[3] = pGridindex[0];
			pGridindex[4] = pGridindex[2];
			pGridindex[5] = pGridindex[0] + (precision + 1);
			for (int k = 0; k < 6; ++k)
			{
				pGridindex[k] += indexOffset;
			}
		}
	}

	//平面3的数据处理
	vector3d* pPlane3Vertexts = pPlane2Vertexts + (precision + 1) * (precision / 2 + 1);
	int* pPlane3Index = pPlane2Index + ((precision * (precision / 2) * 2)) * 3;
	indexOffset += (precision + 1) * (precision / 2 + 1);
	//处理顶点的赋值
	for (int y = 0; y < precision / 2 + 1; ++y)
	{
		for (int x = 0; x < precision + 1; ++x)
		{
			pPlane3Vertexts[y * (precision + 1) + x] = vector3d(-lx - dx * x, ty - dy * y, radius);
		}
	}

	//处理顶平面索引，按照精度也就是格子的数量处理
	for (int i = 0; i < precision / 2; ++i)
	{
		int* pGridLineIndex = pPlane3Index + i * precision * (3 * 2);
		for (int j = 0; j < precision; ++j)
		{
			int* pGridindex = pGridLineIndex + j * 6;
			pGridindex[0] = i * (precision + 1) + j;
			pGridindex[1] = pGridindex[0] + 1;
			pGridindex[2] = pGridindex[1] + (precision + 1);
			pGridindex[3] = pGridindex[0];
			pGridindex[4] = pGridindex[2];
			pGridindex[5] = pGridindex[0] + (precision + 1);
			for (int k = 0; k < 6; ++k)
			{
				pGridindex[k] += indexOffset;
			}
		}
	}

	//平面4的数据处理
	vector3d* pPlane4Vertexts = pPlane3Vertexts + (precision + 1) * (precision / 2 + 1);
	int* pPlane4Index = pPlane3Index + ((precision * (precision / 2) * 2)) * 3;
	indexOffset += (precision + 1) * (precision / 2 + 1);
	//处理顶点的赋值
	for (int y = 0; y < precision / 2 + 1; ++y)
	{
		for (int z = 0; z < precision + 1; ++z)
		{
			pPlane4Vertexts[y * (precision + 1) + z] = vector3d(-radius, ty - dy * y, -lx - dx * z);
		}
	}

	//处理顶平面索引，按照精度也就是格子的数量处理
	for (int i = 0; i < precision / 2; ++i)
	{
		int* pGridLineIndex = pPlane4Index + i * precision * (3 * 2);
		for (int j = 0; j < precision; ++j)
		{
			int* pGridindex = pGridLineIndex + j * 6;
			pGridindex[0] = i * (precision + 1) + j;
			pGridindex[1] = pGridindex[0] + 1;
			pGridindex[2] = pGridindex[1] + (precision + 1);
			pGridindex[3] = pGridindex[0];
			pGridindex[4] = pGridindex[2];
			pGridindex[5] = pGridindex[0] + (precision + 1);
			for (int k = 0; k < 6; ++k)
			{
				pGridindex[k] += indexOffset;
			}
		}
	}

	//按照原点在(0,0,0)将上半球的顶点全部顶到球面,并向上平移height的一半
	Matrix4x4 mat;
	mat.SetTranslate(vector3d::Up() * height * 0.5f);
	vector3d pCenter = vector3d::Zero();
	for (int i = 0; i < vertextsCountOfPlane0 + vertextsCountOfPlane1_4; ++i)
	{
		mPVertexts[i] = (mPVertexts[i] - pCenter).Normalize() * radius * mat;
	}

	//下半球顶点的指针位置
	vector3d* pHalfBottomSphere = mPVertexts + vertextsCountOfPlane0 + vertextsCountOfPlane1_4;
	//复制上半球的数据，并绕z轴转180°
	memcpy(pHalfBottomSphere, mPVertexts, sizeof(vector3d) * (vertextsCountOfPlane0 + vertextsCountOfPlane1_4));
	mat.SetRotateZ_d(180);
	for (int i = 0; i < vertextsCountOfPlane0 + vertextsCountOfPlane1_4; ++i)
	{
		pHalfBottomSphere[i] = pHalfBottomSphere[i] * mat;
	}

	//跳过上半球的索引数，得到下半球的索引指针位置
	int* pHalfBottomSphereIndex = mPIndex + (triCountOfPlane0 + triCountOfPlane1_4) * 3;
	//复制上半球的数据并整体偏移上半球的顶点数
	memcpy(pHalfBottomSphereIndex, mPIndex, sizeof(int) * (triCountOfPlane0 + triCountOfPlane1_4) * 3);
	indexOffset = vertextsCountOfPlane0 + vertextsCountOfPlane1_4;
	for (int i = 0; i < (triCountOfPlane0 + triCountOfPlane1_4) * 3; ++i)
	{
		pHalfBottomSphereIndex[i] += indexOffset;
	}

	//圆周上半圈的顶点，从plane1-plane4的下围的一圈点，起始位置需要跳过上下半球的顶点数：(vertextsCountOfPlane0 + vertextsCountOfPlane1_4)*2
	vector3d* pCircle1 = mPVertexts + (vertextsCountOfPlane0 + vertextsCountOfPlane1_4) * 2;
	vector3d* pPlanes1_4[4] =
	{
		pPlane1Vertexts + (precision + 1) * (precision / 2),
		pPlane2Vertexts + (precision + 1) * (precision / 2),
		pPlane3Vertexts + (precision + 1) * (precision / 2),
		pPlane4Vertexts + (precision + 1) * (precision / 2)
	};
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < precision; ++j)
		{
			pCircle1[i * precision + j] = pPlanes1_4[i][j];
		}
	}
	vector3d* pCircle2 = pCircle1 + precision * 4;
	for (int i = 0; i < precision * 4; ++i)
	{
		pCircle2[i] = pCircle1[i] + vector3d::Down() * height;//将上半圈的圆周点下降
	}

	//上半圆周的索引位置，跳过上下半球的所有索引
	int* pCircleIndex = mPIndex + (triCountOfPlane0 + triCountOfPlane1_4) * 3 * 2;
	//跳过上下半球的顶点数作为圆周的顶点索引开始
	indexOffset = (vertextsCountOfPlane0 + vertextsCountOfPlane1_4) * 2;
	for (int i = 0; i < precision * 4; ++i)
	{
		int* curIndex = pCircleIndex + i * 6;
		curIndex[0] = i;
		curIndex[1] = (i + 1) % (precision * 4);
		curIndex[2] = curIndex[1] + (precision * 4);
		curIndex[3] = curIndex[0];
		curIndex[4] = curIndex[2];
		curIndex[5] = i + (precision * 4);
		for (int j = 0; j < 6; j++)
		{
			curIndex[j] += indexOffset;
		}
	}
}

void MeshData::CreateSphere_s6(float r, int s)
{
	//采用立方体挤出的方式实现
	//先搞定一个面的挤出，其余的方式使用矩阵旋转的方式实现
	//先搞定上面
	//球半径和棱长的关系 r = a * sqrt(3)/2;
	float a = r * 2 / sqrt(3);
	//设采样率是将棱分成的份数,那么samplerate = 4 就是将一个正方形分为4x4个格子
	// 将samplerate记为s
	//在这样的情况下，一个面的顶点数量为 (s + 1)*(s + 1)
	//所以在不考虑面与面之间重复的顶点的情况下
	//总的顶点个数为 (s+1)*(s+1)*6
	mVertextCount = (s + 1) * (s + 1) * 6;
	mIndexCount = s * s * 2 * 3 * 6;////s*s个格子,每个格子两个三角形，6个面
	mTriangleCount = s * s * 2 * 6;
	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];

	float dx = a / s;//一个格子的小边长
	float dz = -dx;//
	float sz = a * 0.5f;//
	for (int z = 0; z < s + 1; ++z)//上面的点
	{
		float sx = -sz;
		for (int x = 0; x < s + 1; ++x)
		{
			mPVertexts[z * (s + 1) + x] = vector3d(sx + dx * x, a / 2.0f, sz + dz * z);
			mPVertexts[z * (s + 1) + x] = mPVertexts[z * (s + 1) + x].Normalized() * r;
		}
	}

	for (int i = 0; i < s; ++i)
	{
		for (int j = 0; j < s; ++j)
		{
			int* pindx = mPIndex + (i * s + j) * 6;

			pindx[0] = (s + 1) * i + j;
			pindx[1] = pindx[0] + 1;
			pindx[2] = pindx[1] + (s + 1);
			pindx[3] = pindx[0];
			pindx[4] = pindx[1] + (s + 1);
			pindx[5] = pindx[0] + (s + 1);
		}
	}

	Matrix4x4 ro;
	//绕z轴转三次90度
	for (int i = 1; i <= 3; ++i)
	{
		ro.SetRotateZ_d(90 * i);
		for (int j = 0; j < (s + 1) * (s + 1); ++j)
		{
			mPVertexts[i * (s + 1) * (s + 1) + j] = mPVertexts[j] * ro;
		}
		for (int n = 0; n < s * s * 2 * 3; ++n)
		{
			mPIndex[i * (s * s * 2 * 3) + n] = mPIndex[n] + ((s + 1) * (s + 1)) * i;
		}
	}
	//绕x转正负90
	int k = 0;
	for (int i = 4; i <= 5; ++i, ++k)
	{
		ro.SetRotateX_d(-90 + 180 * k);
		for (int j = 0; j < (s + 1) * (s + 1); ++j)
		{
			mPVertexts[i * (s + 1) * (s + 1) + j] = mPVertexts[j] * ro;
		}
		for (int n = 0; n < s * s * 2 * 3; ++n)
		{
			mPIndex[i * (s * s * 2 * 3) + n] = mPIndex[n] + ((s + 1) * (s + 1)) * i;
		}
	}
}

void MeshData::CreateSphere_s20(float r, int level)
{
	//正二十面体的半径 r = sqrt(2*sqrt(5)+ 10)/4 * a;
	//a = r*4/sqrt(2*sqrt(5)+ 10);
	double d = r * 2;
	double a = r * 4 / sqrt(2 * sqrt(5) + 10);
	double b = sqrt(d * d - a * a);
	double h = a * b / d;
	double y = r - sqrt(a * a - h * h);

	Matrix4x4 ro;
	ro.SetRotateY_d(72);
	vector3d p[12];
	p[0] = vector3d(0, r, 0);
	p[1] = vector3d((float)h, y, 0);
	p[2] = p[1] * ro;
	p[3] = p[2] * ro;
	p[4] = p[3] * ro;
	p[5] = p[4] * ro;

	ro.SetRotateY_d(36);
	p[6] = p[1] * ro;
	p[7] = p[2] * ro;
	p[8] = p[3] * ro;
	p[9] = p[4] * ro;
	p[10] = p[5] * ro;
	p[6].y = -y;
	p[7].y = -y;
	p[8].y = -y;
	p[9].y = -y;
	p[10].y = -y;
	p[11] = vector3d(0, -r, 0);

	int index[] = {
		0,2,1,
		0,3,2,
		0,4,3,
		0,5,4,
		0,1,5,

		3,4,8,
		4,9,8,
		4,5,9,
		5,10,9,
		5,1,10,
		1,6,10,
		1,2,6,
		2,7,6,
		2,3,7,
		3,8,7,

		11,8,9,
		11,9,10,
		11,10,6,
		11,6,7,
		11,7,8
	};
	int tc = sizeof(index) / 4 / 3;

	std::vector<Triangle> ts1;
	std::vector<Triangle> ts2;
	std::vector<Triangle>* pCurTriangles = &ts1;
	std::vector<Triangle>* pNewTriangles = &ts2;

	for (int i = 0; i < tc; ++i)
	{
		ts1.push_back(Triangle(p[index[i * 3]], p[index[i * 3 + 1]], p[index[i * 3 + 2]]));
	}
	level = level < 1 ? 1 : level;
	vector3d t12, t23, t31;
	size_t tsize;
	std::vector<Triangle>* temp;

	float minAera = ts1[0].Area();

	float s1, s2, s3, s4;
	Triangle t1, t2, t3, t4;

	for (int i = 1; i < level; ++i)
	{
		pNewTriangles->clear();
		tsize = pCurTriangles->size();
		for (size_t n = 0; n < tsize; ++n)
		{
			Triangle& t = pCurTriangles[0][n];
			t12 = vector3d::Lerp(t.p1, t.p2, 0.5f).Normalized() * r;
			t23 = vector3d::Lerp(t.p2, t.p3, 0.5f).Normalized() * r;
			t31 = vector3d::Lerp(t.p3, t.p1, 0.5f).Normalized() * r;

			t1 = Triangle(t.p1, t12, t31);
			t2 = Triangle(t12, t.p2, t23);
			t3 = Triangle(t31, t12, t23);
			t4 = Triangle(t31, t23, t.p3);

			s1 = t1.Area();
			s2 = t2.Area();
			s3 = t3.Area();
			s4 = t4.Area();

			minAera = s1 < minAera ? s1 : minAera;
			minAera = s2 < minAera ? s2 : minAera;
			minAera = s3 < minAera ? s3 : minAera;
			minAera = s4 < minAera ? s4 : minAera;

			//一个三角形裂变为四个三角形
			pNewTriangles->push_back(t1);
			pNewTriangles->push_back(t2);
			pNewTriangles->push_back(t3);
			pNewTriangles->push_back(t4);
		}
		//新旧列表交换
		temp = pCurTriangles;
		pCurTriangles = pNewTriangles;
		pNewTriangles = temp;
	}

	//最后没有可以裂变的三角形了
	//所有的三角形也待在当前列表中
	//不考虑重复，每个三角形独立,顶点数量等于三角形数量 * 3
	//索引数量也与顶点数量相同
	mTriangleCount = (int)pCurTriangles->size();
	mVertextCount = mTriangleCount * 3;
	mIndexCount = mVertextCount;

	mPIndex = new int[mIndexCount];
	mPVertexts = new vector3d[mVertextCount];
	for (int i = 0; i < mTriangleCount; ++i)
	{
		mPVertexts[i * 3] = pCurTriangles[0][i].p1;
		mPVertexts[i * 3 + 1] = pCurTriangles[0][i].p2;
		mPVertexts[i * 3 + 2] = pCurTriangles[0][i].p3;

		mPIndex[i * 3] = i * 3;
		mPIndex[i * 3 + 1] = i * 3 + 1;
		mPIndex[i * 3 + 2] = i * 3 + 2;
	}

	_InvNormal();
}

void MeshData::CreateSphere_uv(float r, int s1, int s2)
{
	s1 = s1 < 2 ? 2 : s1;
	s2 = s2 < 3 ? 3 : s2;

	//沿着经度方向将一条经线按照s1分成的弧长对应的圆心角
	float angle1 = 180.0f / s1;
	//沿着纬度方向将一条纬线按照s2分成的弧长对应的圆心角
	float angle2 = 360.0f / s2;

	//不考虑上下两极顶点重合的情况
	//认为每条经线都是从北极到南极被分成s1份，则一条经线的点数为s1 + 1
	//为了方便包裹UV贴图，让经线最后和起始的经线重合，避免最后的接缝
	//那么经线的数量为 s2 +1,其中最后一条经线和第一条经线重合
	//所以总的顶点数为(s1 + 1)*(s2 + 1)
	mVertextCount = (s1 + 1) * (s2 + 1);
	mTriangleCount = s1 * s2 * 2;
	mIndexCount = s1 * s2 * 2 * 3;
	mPVertexts = new vector3d[mVertextCount];
	//mPUVs = new vector2d[mVertextCount];
	mPIndex = new int[mIndexCount];


	//先产生第一条经线数据
	mPVertexts[0] = vector3d::Up() * r;
	float a = 0;
	//mPUVs[0].v = a;
	//mPUVs[0].u = 0;
	Matrix4x4 r1;
	r1.SetRotateZ_d(angle1);


	for (int i = 0; i < s1; ++i)
	{
		mPVertexts[i + 1] = mPVertexts[i] * r1;
		//mPUVs[i + 1].v = 1 - (a += angle1) / 180.0f;
		//mPUVs[i + 1].u = 0;
	}
	//产生其他经线
	Matrix4x4 r2;
	float u = 0;
	a = 0;
	for (int i = 0; i < s2; ++i)
	{

		u = (a += angle2) / 360.0f;
		r2.SetRotateY_d(angle2 * (i + 1));
		vector3d* p = mPVertexts + (s1 + 1) * (i + 1);
		//vector2d* uv = mPUVs + (s1 + 1) * (i + 1);
		for (int j = 0; j < s1 + 1; ++j)
		{
			p[j] = mPVertexts[j] * r2;
			//uv[j].u = u;
			//uv[j].v = mPUVs[j].v;
		}
	}

	for (int y = 0; y < s1; ++y)
	{
		int* pindex = mPIndex + s2 * 6 * y;
		for (int x = 0; x < s2; ++x)
		{
			int* curindex = pindex + 6 * x;
			curindex[0] = y + x * (s1 + 1);
			curindex[1] = curindex[0] + (s1 + 1);
			curindex[2] = curindex[1] + 1;
			curindex[3] = curindex[0];
			curindex[4] = curindex[1] + 1;
			curindex[5] = curindex[0] + 1;
		}
	}

	_InvNormal();
}

void MeshData::CreateRing(float radius1, float radius2, size_t precision1, size_t precision2)
{
	_Clear();//清除旧的数据
	if (precision1 < 3) { precision1 = 3; }
	if (precision2 < 3) { precision2 = 3; }

	radius1 = IS_FLOAT_ZERO(radius1) ? 0.5f : abs(radius1);
	radius2 = IS_FLOAT_ZERO(radius2) ? 1.0f : abs(radius2);
	radius2 = radius1 > radius2 ? radius1 : radius2;//大圆不能小于小圆

	//大圆的精度代表小圆的个数，小圆的精度代表小圆的顶点数
	mVertextCount = precision1 * precision2;
	mTriangleCount = precision1 * 2 * precision2;
	mIndexCount = mTriangleCount * 3;

	mPVertexts = new vector3d[mVertextCount];
	mPIndex = new int[mIndexCount];
	//先算一个小圆的数据
	mPVertexts[0] = vector3d::Up() * radius1;
	Matrix4x4 mat;
	//绕着z轴旋转一周
	mat.SetRotateZ_d(360.0f / precision1);
	for (int i = 1; i < precision1; ++i)
	{
		mPVertexts[i] = mPVertexts[i - 1] * mat;
	}
	//整体左移
	mat.SetTranslate(vector3d::Left() * radius2);

	for (int i = 0; i < precision1; ++i)
	{
		mPVertexts[i] = mPVertexts[i] * mat;
	}

	//将每个小圆环绕y轴旋转
	mat.SetRotateY_d(360.0f / precision2);
	for (int i = 1; i < precision2; ++i)
	{
		vector3d* pMinCirclePre = mPVertexts + (i - 1) * precision1;
		vector3d* pMinCircle = mPVertexts + i * precision1;
		for (int j = 0; j < precision1; ++j)
		{
			pMinCircle[j] = pMinCirclePre[j] * mat;
		}
	}

	//计算索引
	for (int j = 0; j < precision1; ++j)
	{
		int* curIndex = mPIndex + j * 6;
		curIndex[0] = j;
		curIndex[1] = (curIndex[0] + 1) % precision1;
		curIndex[2] = curIndex[1] + precision1;
		curIndex[3] = curIndex[0];
		curIndex[4] = curIndex[2];
		curIndex[5] = (curIndex[0] + precision1) % (precision1 * precision2);
	}
	int* preIndex = mPIndex;
	for (int i = 1; i < precision2; ++i)
	{
		int* curIndex = mPIndex + i * precision1 * 6;
		for (int j = 0; j < precision1 * 2 * 3; ++j)
		{
			curIndex[j] = (preIndex[j] + precision1) % (precision1 * precision2);
		}
		preIndex = curIndex;
	}
}

void MeshData::CreateSphere_s4(float r, int level)
{
	//如果正四面体的棱长为a
	//则外接球半径 r = a * sqrt(6)/4 
	//则 a = r * 4 / sqrt(6)
	//体高 h = a *  sqrt(6)/3 = r * 4 / 3;
	//体心在地面的投影与地面顶点的连线长度
	//x = sqrt(r*r - (r/3) * (r/3))
	//x = sqrt(8*r/9)
	//x = sqrt(2) * r * 2/3
	float x = sqrt(2) * r * 2 / 3.0f;
	vector3d p[4] =
	{
		vector3d(0,r,0),//上顶点
		vector3d(x,-r / 3,0),//右顶点
	};
	Matrix4x4 ro;
	ro.SetRotateY_d(120);
	p[2] = p[1] * ro;
	p[3] = p[2] * ro;

	std::vector<Triangle> ts1;
	std::vector<Triangle> ts2;
	std::vector<Triangle>* pCurTriangles = &ts1;
	std::vector<Triangle>* pNewTriangles = &ts2;

	ts1.push_back(Triangle(p[0], p[2], p[1]));
	ts1.push_back(Triangle(p[0], p[3], p[2]));
	ts1.push_back(Triangle(p[0], p[1], p[3]));
	ts1.push_back(Triangle(p[1], p[2], p[3]));

	level = level < 1 ? 1 : level;
	vector3d t12, t23, t31;
	size_t tsize;
	std::vector<Triangle>* temp;

	float maxAera = 0;
	float minAera = ts1[0].Area();

	float s1, s2, s3, s4;
	Triangle t1, t2, t3, t4;

	for (int i = 1; i < level; ++i)
	{
		pNewTriangles->clear();
		tsize = pCurTriangles->size();
		for (size_t n = 0; n < tsize; ++n)
		{
			Triangle& t = pCurTriangles[0][n];
			t12 = vector3d::Lerp(t.p1, t.p2, 0.5f).Normalized() * r;
			t23 = vector3d::Lerp(t.p2, t.p3, 0.5f).Normalized() * r;
			t31 = vector3d::Lerp(t.p3, t.p1, 0.5f).Normalized() * r;

			t1 = Triangle(t.p1, t12, t31);
			t2 = Triangle(t12, t.p2, t23);
			t3 = Triangle(t31, t12, t23);
			t4 = Triangle(t31, t23, t.p3);

			s1 = t1.Area();
			s2 = t2.Area();
			s3 = t3.Area();
			s4 = t4.Area();

			//maxAera = s1 > maxAera ? s1 : maxAera;
			//maxAera = s2 > maxAera ? s2 : maxAera;
			//maxAera = s3 > maxAera ? s3 : maxAera;
			//maxAera = s4 > maxAera ? s4 : maxAera;

			minAera = s1 < minAera ? s1 : minAera;
			minAera = s2 < minAera ? s2 : minAera;
			minAera = s3 < minAera ? s3 : minAera;
			minAera = s4 < minAera ? s4 : minAera;

			//一个三角形裂变为四个三角形
			pNewTriangles->push_back(t1);
			pNewTriangles->push_back(t2);
			pNewTriangles->push_back(t3);
			pNewTriangles->push_back(t4);
		}
		//新旧列表交换
		temp = pCurTriangles;
		pCurTriangles = pNewTriangles;
		pNewTriangles = temp;
	}
	//最后的成果在当前三角形列表中
	//但是此时有些三角形的大小比较大，有的比较小
	//将超过最小面积2倍的三角形进行再次裂变
	int count = 1;
	float curarea;
	minAera *= 2;
	while (count > 0)
	{
		count = 0;
		pNewTriangles->clear();
		tsize = pCurTriangles->size();
		for (size_t n = 0; n < tsize; ++n)
		{
			curarea = pCurTriangles[0][n].Area();
			if (curarea > minAera)
			{
				Triangle& t = pCurTriangles[0][n];
				t12 = vector3d::Lerp(t.p1, t.p2, 0.5f).Normalized() * r;
				t23 = vector3d::Lerp(t.p2, t.p3, 0.5f).Normalized() * r;
				t31 = vector3d::Lerp(t.p3, t.p1, 0.5f).Normalized() * r;

				t1 = Triangle(t.p1, t12, t31);
				t2 = Triangle(t12, t.p2, t23);
				t3 = Triangle(t31, t12, t23);
				t4 = Triangle(t31, t23, t.p3);

				//一个三角形裂变为四个三角形
				pNewTriangles->push_back(t1);
				pNewTriangles->push_back(t2);
				pNewTriangles->push_back(t3);
				pNewTriangles->push_back(t4);
				//发生了裂变就将count++
				count++;
			}
			else
			{
				//不裂变的三角形原样加入
				pNewTriangles->push_back(pCurTriangles[0][n]);
			}


		}
		//新旧列表交换
		temp = pCurTriangles;
		pCurTriangles = pNewTriangles;
		pNewTriangles = temp;
	}
	//最后没有可以裂变的三角形了
	//所有的三角形也待在当前列表中
	//不考虑重复，每个三角形独立,顶点数量等于三角形数量 * 3
	//索引数量也与顶点数量相同
	mTriangleCount = (int)pCurTriangles->size();
	mVertextCount = mTriangleCount * 3;
	mIndexCount = mVertextCount;

	mPIndex = new int[mIndexCount];
	mPVertexts = new vector3d[mVertextCount];
	for (int i = 0; i < mTriangleCount; ++i)
	{
		mPVertexts[i * 3] = pCurTriangles[0][i].p1;
		mPVertexts[i * 3 + 1] = pCurTriangles[0][i].p2;
		mPVertexts[i * 3 + 2] = pCurTriangles[0][i].p3;

		mPIndex[i * 3] = i * 3;
		mPIndex[i * 3 + 1] = i * 3 + 1;
		mPIndex[i * 3 + 2] = i * 3 + 2;
	}
	_InvNormal();
}

void MeshData::CreateSphere_s8(float r, int level)
{
	//正八面体的棱长a = sqrt(2) * r;
	float a = sqrt(2) * r;
	vector3d p[6] =
	{
		vector3d(0,r,0),//上顶点
		vector3d(r,0,0),//右顶点
		vector3d::Zero(),
		vector3d::Zero(),
		vector3d::Zero(),
		vector3d(0,-r,0)//下顶点
	};
	Matrix4x4 ro;
	ro.SetRotateY_d(90);
	p[2] = p[1] * ro;
	p[3] = p[2] * ro;
	p[4] = p[3] * ro;


	std::vector<Triangle> ts1;
	std::vector<Triangle> ts2;
	std::vector<Triangle>* pCurTriangles = &ts1;
	std::vector<Triangle>* pNewTriangles = &ts2;

	ts1.push_back(Triangle(p[0], p[2], p[1]));
	ts1.push_back(Triangle(p[0], p[3], p[2]));
	ts1.push_back(Triangle(p[0], p[4], p[3]));
	ts1.push_back(Triangle(p[0], p[1], p[4]));
	ts1.push_back(Triangle(p[5], p[1], p[2]));
	ts1.push_back(Triangle(p[5], p[2], p[3]));
	ts1.push_back(Triangle(p[5], p[3], p[4]));
	ts1.push_back(Triangle(p[5], p[4], p[1]));

	level = level < 1 ? 1 : level;
	vector3d t12, t23, t31;
	size_t tsize;
	std::vector<Triangle>* temp;

	float minAera = ts1[0].Area();

	float s1, s2, s3, s4;
	Triangle t1, t2, t3, t4;

	for (int i = 1; i < level; ++i)
	{
		pNewTriangles->clear();
		tsize = pCurTriangles->size();
		for (size_t n = 0; n < tsize; ++n)
		{
			Triangle& t = pCurTriangles[0][n];
			t12 = vector3d::Lerp(t.p1, t.p2, 0.5f).Normalized() * r;
			t23 = vector3d::Lerp(t.p2, t.p3, 0.5f).Normalized() * r;
			t31 = vector3d::Lerp(t.p3, t.p1, 0.5f).Normalized() * r;

			t1 = Triangle(t.p1, t12, t31);
			t2 = Triangle(t12, t.p2, t23);
			t3 = Triangle(t31, t12, t23);
			t4 = Triangle(t31, t23, t.p3);

			s1 = t1.Area();
			s2 = t2.Area();
			s3 = t3.Area();
			s4 = t4.Area();

			minAera = s1 < minAera ? s1 : minAera;
			minAera = s2 < minAera ? s2 : minAera;
			minAera = s3 < minAera ? s3 : minAera;
			minAera = s4 < minAera ? s4 : minAera;

			//一个三角形裂变为四个三角形
			pNewTriangles->push_back(t1);
			pNewTriangles->push_back(t2);
			pNewTriangles->push_back(t3);
			pNewTriangles->push_back(t4);
		}
		//新旧列表交换
		temp = pCurTriangles;
		pCurTriangles = pNewTriangles;
		pNewTriangles = temp;
	}

	//最后没有可以裂变的三角形了
	//所有的三角形也待在当前列表中
	//不考虑重复，每个三角形独立,顶点数量等于三角形数量 * 3
	//索引数量也与顶点数量相同
	mTriangleCount = (int)pCurTriangles->size();
	mVertextCount = mTriangleCount * 3;
	mIndexCount = mVertextCount;

	mPIndex = new int[mIndexCount];
	mPVertexts = new vector3d[mVertextCount];
	for (int i = 0; i < mTriangleCount; ++i)
	{
		mPVertexts[i * 3] = pCurTriangles[0][i].p1;
		mPVertexts[i * 3 + 1] = pCurTriangles[0][i].p2;
		mPVertexts[i * 3 + 2] = pCurTriangles[0][i].p3;

		mPIndex[i * 3] = i * 3;
		mPIndex[i * 3 + 1] = i * 3 + 1;
		mPIndex[i * 3 + 2] = i * 3 + 2;
	}
	_InvNormal();
}

void MeshData::CreateSphere_s12(float r, int level)
{
	float fai = (1 + sqrt(5)) / 2.0f;

	vector3d p[20] = {
		vector3d(-1.0f, +1.0f, +1.0f).Normalized() * r,
		vector3d(+1.0f, +1.0f, +1.0f).Normalized() * r,
		vector3d(+1.0f, -1.0f, +1.0f).Normalized() * r,
		vector3d(-1.0f, -1.0f, +1.0f).Normalized() * r,

		vector3d(-1.0f, +1.0f, -1.0f).Normalized() * r,
		vector3d(+1.0f, +1.0f, -1.0f).Normalized() * r,
		vector3d(+1.0f, -1.0f, -1.0f).Normalized() * r,
		vector3d(-1.0f, -1.0f, -1.0f).Normalized() * r,

		vector3d(0, fai, 1 / fai).Normalized() * r,
		vector3d(0, fai, -1 / fai).Normalized() * r,
		vector3d(0, -fai, 1 / fai).Normalized() * r,
		vector3d(0, -fai, -1 / fai).Normalized() * r,

		vector3d(1 / fai,0, fai).Normalized() * r,
		vector3d(1 / fai,0, -fai).Normalized() * r,
		vector3d(-1 / fai,0, fai).Normalized() * r,
		vector3d(-1 / fai,0, -fai).Normalized() * r,

		vector3d(fai, 1 / fai, 0).Normalized() * r,
		vector3d(fai, -1 / fai, 0).Normalized() * r,
		vector3d(-fai, 1 / fai, 0).Normalized() * r,
		vector3d(-fai, -1 / fai, 0).Normalized() * r
	};
	int index[60] = {
		9, 8, 1, 16, 5,
		8, 0, 14, 12, 1,
		8, 9, 4, 18, 0,
		9, 5, 13, 15, 4,
		5, 16, 17, 6, 13,
		17, 16, 1, 12, 2,
		2, 12, 14, 3, 10,
		3, 14, 0, 18, 19,
		19, 18, 4, 15, 7,
		7, 15, 13, 6, 11,
		10, 3, 19, 7, 11,
		11, 6, 17, 2, 10
	};
	std::vector<Triangle> ts1;
	std::vector<Triangle> ts2;
	std::vector<Triangle>* pCurTriangles = &ts1;
	std::vector<Triangle>* pNewTriangles = &ts2;

	for (int i = 0; i < 12; ++i)
	{
		vector3d center = p[index[i * 5]]
			+ p[index[i * 5 + 1]]
			+ p[index[i * 5 + 2]]
			+ p[index[i * 5 + 3]]
			+ p[index[i * 5 + 4]];
		center = center * 0.2f;
		center = center.Normalized() * r;

		ts1.push_back(Triangle(center, p[index[i * 5]], p[index[i * 5 + 1]]));
		ts1.push_back(Triangle(center, p[index[i * 5 + 1]], p[index[i * 5 + 2]]));
		ts1.push_back(Triangle(center, p[index[i * 5 + 2]], p[index[i * 5 + 3]]));
		ts1.push_back(Triangle(center, p[index[i * 5 + 3]], p[index[i * 5 + 4]]));
		ts1.push_back(Triangle(center, p[index[i * 5 + 4]], p[index[i * 5 + 0]]));
	}
	level = level < 1 ? 1 : level;
	vector3d t12, t23, t31;
	size_t tsize;
	std::vector<Triangle>* temp;
	Triangle t1, t2, t3, t4;

	for (int i = 1; i < level; ++i)
	{
		pNewTriangles->clear();
		tsize = pCurTriangles->size();
		for (size_t n = 0; n < tsize; ++n)
		{
			Triangle& t = pCurTriangles[0][n];
			t12 = vector3d::Lerp(t.p1, t.p2, 0.5f).Normalized() * r;
			t23 = vector3d::Lerp(t.p2, t.p3, 0.5f).Normalized() * r;
			t31 = vector3d::Lerp(t.p3, t.p1, 0.5f).Normalized() * r;

			t1 = Triangle(t.p1, t12, t31);
			t2 = Triangle(t12, t.p2, t23);
			t3 = Triangle(t31, t12, t23);
			t4 = Triangle(t31, t23, t.p3);

			//一个三角形裂变为四个三角形
			pNewTriangles->push_back(t1);
			pNewTriangles->push_back(t2);
			pNewTriangles->push_back(t3);
			pNewTriangles->push_back(t4);
		}
		//新旧列表交换
		temp = pCurTriangles;
		pCurTriangles = pNewTriangles;
		pNewTriangles = temp;
	}

	//最后没有可以裂变的三角形了
	//所有的三角形也待在当前列表中
	//不考虑重复，每个三角形独立,顶点数量等于三角形数量 * 3
	//索引数量也与顶点数量相同
	mTriangleCount = (int)pCurTriangles->size();
	mVertextCount = mTriangleCount * 3;
	mIndexCount = mVertextCount;

	mPIndex = new int[mIndexCount];
	mPVertexts = new vector3d[mVertextCount];
	for (int i = 0; i < mTriangleCount; ++i)
	{
		mPVertexts[i * 3] = pCurTriangles[0][i].p1;
		mPVertexts[i * 3 + 1] = pCurTriangles[0][i].p2;
		mPVertexts[i * 3 + 2] = pCurTriangles[0][i].p3;

		mPIndex[i * 3] = i * 3;
		mPIndex[i * 3 + 1] = i * 3 + 1;
		mPIndex[i * 3 + 2] = i * 3 + 2;
	}
	//_InvNormal();
}