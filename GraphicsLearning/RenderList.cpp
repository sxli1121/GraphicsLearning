#include "RenderList.h"
#include "MemRenderBuffer.h"

Triangle RenderList::mRenderTriangle[MAX_TRANGLE_NUM];
Triangle TerrainRenderList::mTerrainRenderTriangle[MAX_TRANGLE_NUM];

RenderList::RenderList()
{
	mCurIndex = 0;
}

void RenderList::AddTriangle()
{
	if (mCurIndex + 1 < MAX_TRANGLE_NUM)
	{
		++mCurIndex;
	}
}

Triangle& RenderList::GetNextTriangle()
{
	return mRenderTriangle[mCurIndex];
}

int RenderList::GetTriangleCount()
{
	return mCurIndex;
}

void RenderList::ClearTriangle()
{
	mCurIndex = 0;
}

void RenderList::RenderAllTriangle()
{
	// 此处进行近截面的裁剪--摄像机坐标下 近截面 = znear 做比较
	const Mat4x4f& matView = Camera::mainCamera->MatrixView;
	const Mat4x4f& matPV = Camera::mainCamera->MatrixProjection
		* Camera::mainCamera->MatrixViewPort;   // 摄像机矩阵和  投影视口矩阵

	float& zNear = Camera::mainCamera->mZnear;
	float z1, z2, z3;

	Plane3D planeNear(vec3f(0, 0, zNear), vec3f::forward());  // 构建一个近截面

	//为了避免计算增加的三角形，将三角形当前的数量记录一下
	int oldIndex = mCurIndex;//还没有裁剪的三角形的数量

	int insideCount = 0;//点在内部的个数
	vec3f c1, c2;
	vec2f newuv1, newuv2;  // uv也需要裁剪(插值)
	float t1, t2;   // 插值的比例



	for (int i = 0; i < oldIndex; ++i)   // 遍历裁剪并绘制所有旧的三角形
	{
		//转换三角形坐标系: 世界 ——> 摄像机
		mRenderTriangle[i].Transform(matView);
		insideCount = (mRenderTriangle[i].p1.z > zNear ? 1 : 0)
			+ (mRenderTriangle[i].p2.z > zNear ? 1 : 0)
			+ (mRenderTriangle[i].p3.z > zNear ? 1 : 0);

		switch (insideCount)
		{
		case 0: {continue; }break;//点都在外面，不需要绘制
		case 3: {}break;//都在里面，后面直接绘制
		case 1://表示有一个点在内部
		{
			Triangle& t = mRenderTriangle[i];
			if (t.pts[0].z > zNear)//0内12外
			{
				planeNear.LineCrossPlane(t.pts[0], t.pts[1], c1, &t1);
				planeNear.LineCrossPlane(t.pts[0], t.pts[2], c2, &t2);
				t.pts[1] = c1;
				t.pts[2] = c2;
				t.uv[1] = t.uv[0].Lerp(t.uv[1], t1);
				t.uv[2] = t.uv[0].Lerp(t.uv[2], t2);
			}
			else if (t.pts[1].z > zNear)//1内02外
			{
				planeNear.LineCrossPlane(t.pts[1], t.pts[2], c1, &t1);
				planeNear.LineCrossPlane(t.pts[1], t.pts[0], c2, &t2);
				t.pts[2] = c1;
				t.pts[0] = c2;
				t.uv[2] = t.uv[1].Lerp(t.uv[2], t1);
				t.uv[0] = t.uv[1].Lerp(t.uv[0], t2);
			}
			else if (t.pts[2].z > zNear)//2内01外
			{
				planeNear.LineCrossPlane(t.pts[2], t.pts[0], c1, &t1);
				planeNear.LineCrossPlane(t.pts[2], t.pts[1], c2, &t2);
				t.pts[0] = c1;
				t.pts[1] = c2;
				t.uv[0] = t.uv[2].Lerp(t.uv[0], t1);
				t.uv[1] = t.uv[2].Lerp(t.uv[1], t2);
			}
		}break;
		case 2://表示有两个点在内部
		{
			Triangle& t = mRenderTriangle[i];
			if (t.pts[0].z < zNear)
			{
				planeNear.LineCrossPlane(t.pts[0], t.pts[2], c1, &t1);
				planeNear.LineCrossPlane(t.pts[0], t.pts[1], c2, &t2);
				newuv1 = t.uv[0].Lerp(t.uv[2], t1);
				newuv2 = t.uv[0].Lerp(t.uv[1], t2);

				t.pts[0] = c1;
				t.uv[0] = newuv1;

				//使用新增的两个交点和原来的一个交点构成新的三角形
				Triangle& tNew = GetNextTriangle();
				tNew.p1 = t.pts[1];
				tNew.p2 = c1;
				tNew.p3 = c2;

				tNew.uv1 = t.uv[1];
				tNew.uv2 = newuv1;
				tNew.uv3 = newuv2;
				tNew.ptexture = t.ptexture;  // 图片要给新的三角形

				AddTriangle();
			}
			else if (t.pts[1].z < zNear)
			{
				planeNear.LineCrossPlane(t.pts[1], t.pts[0], c1, &t1);
				planeNear.LineCrossPlane(t.pts[1], t.pts[2], c2, &t2);
				newuv1 = t.uv[1].Lerp(t.uv[0], t1);
				newuv2 = t.uv[1].Lerp(t.uv[2], t2);

				t.pts[1] = c1;
				t.uv[1] = newuv1;

				//使用新增的两个交点和原来的一个交点构成新的三角形
				Triangle& tNew = GetNextTriangle();
				tNew.p1 = t.pts[2];
				tNew.p2 = c1;
				tNew.p3 = c2;

				tNew.uv1 = t.uv[2];
				tNew.uv2 = newuv1;
				tNew.uv3 = newuv2;
				tNew.ptexture = t.ptexture;

				AddTriangle();
			}
			else if (t.pts[2].z < zNear)
			{
				planeNear.LineCrossPlane(t.pts[2], t.pts[1], c1, &t1);
				planeNear.LineCrossPlane(t.pts[2], t.pts[0], c2, &t2);
				newuv1 = t.uv[2].Lerp(t.uv[1], t1);
				newuv2 = t.uv[2].Lerp(t.uv[0], t2);

				t.pts[2] = c1;
				t.uv[2] = newuv1;

				//使用新增的两个交点和原来的一个交点构成新的三角形
				Triangle& tNew = GetNextTriangle();
				tNew.p1 = t.pts[0];
				tNew.p2 = c1;
				tNew.p3 = c2;

				tNew.uv1 = t.uv[0];
				tNew.uv2 = newuv1;
				tNew.uv3 = newuv2;
				tNew.ptexture = t.ptexture;

				AddTriangle();
			}
		}break;
		}

		z1 = mRenderTriangle[i].p1.z;  // 保存之前原有的深度值(摄像机坐标下)
		z2 = mRenderTriangle[i].p2.z;
		z3 = mRenderTriangle[i].p3.z;

		//能够在这里绘制的三角形，要么是不需要裁剪的，或者是已经被裁掉一部分的三角形
		mRenderTriangle[i].Transform(matPV);  // 经过所有矩阵的变换

		mRenderTriangle[i].p1.z = 1 / z1;  // 修改所有三角形顶点的z值(深度值)为倒数
		mRenderTriangle[i].p2.z = 1 / z2;
		mRenderTriangle[i].p3.z = 1 / z3;

		mRenderTriangle[i].uv1 /= z1;  // 本来的uv坐标要/ 对应的z值
		mRenderTriangle[i].uv2 /= z2;
		mRenderTriangle[i].uv3 /= z3;

		MRB.DrawTriangle_Solid(
			mRenderTriangle[i].p1.x, mRenderTriangle[i].p1.y, mRenderTriangle[i].p1.z,
			mRenderTriangle[i].p2.x, mRenderTriangle[i].p2.y, mRenderTriangle[i].p2.z,
			mRenderTriangle[i].p3.x, mRenderTriangle[i].p3.y, mRenderTriangle[i].p3.z,
			mRenderTriangle[i].uv1, mRenderTriangle[i].uv2, mRenderTriangle[i].uv3,
			mRenderTriangle[i].ptexture);
	}

	//绘制可能的新增的三角形
	for (int i = oldIndex; i < mCurIndex; ++i)
	{
		z1 = mRenderTriangle[i].p1.z;
		z2 = mRenderTriangle[i].p2.z;
		z3 = mRenderTriangle[i].p3.z;

		//能够在这里绘制的三角形，要么是不需要裁剪的，或者是已经被裁掉一部分的三角形
		mRenderTriangle[i].Transform(matPV);

		mRenderTriangle[i].p1.z = 1 / z1;
		mRenderTriangle[i].p2.z = 1 / z2;
		mRenderTriangle[i].p3.z = 1 / z3;

		mRenderTriangle[i].uv1 /= z1;
		mRenderTriangle[i].uv2 /= z2;
		mRenderTriangle[i].uv3 /= z3;

		MRB.DrawTriangle_Solid(
			mRenderTriangle[i].p1.x, mRenderTriangle[i].p1.y, mRenderTriangle[i].p1.z,
			mRenderTriangle[i].p2.x, mRenderTriangle[i].p2.y, mRenderTriangle[i].p2.z,
			mRenderTriangle[i].p3.x, mRenderTriangle[i].p3.y, mRenderTriangle[i].p3.z,
			mRenderTriangle[i].uv1, mRenderTriangle[i].uv2, mRenderTriangle[i].uv3,
			mRenderTriangle[i].ptexture);
	}
	printf("\r模型三角形数量：%d  地形三角形数量:%d", mCurIndex, TRL.mCurIndex);
}


TerrainRenderList::TerrainRenderList()
{
	mCurIndex = 0;
}

void TerrainRenderList::AddTriangle()
{
	if (mCurIndex + 1 < MAX_TRANGLE_NUM)
	{
		++mCurIndex;
	}
}

Triangle& TerrainRenderList::GetNextTriangle()
{
	return mTerrainRenderTriangle[mCurIndex];
}

int TerrainRenderList::GetTriangleCount()
{
	return mCurIndex;
}

void TerrainRenderList::RenderAllTriangle()
{
	// 此处进行近截面的裁剪--摄像机坐标下 近截面 = znear 做比较
	const Mat4x4f& matView = Camera::mainCamera->MatrixView;
	const Mat4x4f& matPV = Camera::mainCamera->MatrixProjection
		* Camera::mainCamera->MatrixViewPort;   // 摄像机矩阵和  投影视口矩阵

	float& zNear = Camera::mainCamera->mZnear;
	float z1, z2, z3;

	Plane3D planeNear(vec3f(0, 0, zNear), vec3f::forward());  // 构建一个近截面

	//为了避免计算增加的三角形，将三角形当前的数量记录一下
	int oldIndex = mCurIndex;//还没有裁剪的三角形的数量

	int insideCount = 0;//点在内部的个数
	vec3f c1, c2;
	vec2f newuv1, newuv2;  // uv也需要裁剪(插值)
	float t1, t2;   // 插值的比例


	for (int i = 0; i < oldIndex; ++i)   // 遍历裁剪并绘制所有旧的三角形
	{
		//转换三角形坐标系: 世界 ——> 摄像机
		mTerrainRenderTriangle[i].Transform(matView);
		insideCount = (mTerrainRenderTriangle[i].p1.z > zNear ? 1 : 0)
			+ (mTerrainRenderTriangle[i].p2.z > zNear ? 1 : 0)
			+ (mTerrainRenderTriangle[i].p3.z > zNear ? 1 : 0);

		switch (insideCount)
		{
		case 0: {continue; }break;//点都在外面，不需要绘制
		case 3: {}break;//都在里面，后面直接绘制
		case 1://表示有一个点在内部
		{
			Triangle& t = mTerrainRenderTriangle[i];
			if (t.pts[0].z > zNear)//0内12外
			{
				planeNear.LineCrossPlane(t.pts[0], t.pts[1], c1, &t1);
				planeNear.LineCrossPlane(t.pts[0], t.pts[2], c2, &t2);
				t.pts[1] = c1;
				t.pts[2] = c2;
				t.uv[1] = t.uv[0].Lerp(t.uv[1], t1);
				t.uv[2] = t.uv[0].Lerp(t.uv[2], t2);
			}
			else if (t.pts[1].z > zNear)//1内02外
			{
				planeNear.LineCrossPlane(t.pts[1], t.pts[2], c1, &t1);
				planeNear.LineCrossPlane(t.pts[1], t.pts[0], c2, &t2);
				t.pts[2] = c1;
				t.pts[0] = c2;
				t.uv[2] = t.uv[1].Lerp(t.uv[2], t1);
				t.uv[0] = t.uv[1].Lerp(t.uv[0], t2);
			}
			else if (t.pts[2].z > zNear)//2内01外
			{
				planeNear.LineCrossPlane(t.pts[2], t.pts[0], c1, &t1);
				planeNear.LineCrossPlane(t.pts[2], t.pts[1], c2, &t2);
				t.pts[0] = c1;
				t.pts[1] = c2;
				t.uv[0] = t.uv[2].Lerp(t.uv[0], t1);
				t.uv[1] = t.uv[2].Lerp(t.uv[1], t2);
			}
		}break;
		case 2://表示有两个点在内部
		{
			Triangle& t = mTerrainRenderTriangle[i];
			if (t.pts[0].z < zNear)
			{
				planeNear.LineCrossPlane(t.pts[0], t.pts[2], c1, &t1);
				planeNear.LineCrossPlane(t.pts[0], t.pts[1], c2, &t2);
				newuv1 = t.uv[0].Lerp(t.uv[2], t1);
				newuv2 = t.uv[0].Lerp(t.uv[1], t2);

				t.pts[0] = c1;
				t.uv[0] = newuv1;

				//使用新增的两个交点和原来的一个交点构成新的三角形
				Triangle& tNew = GetNextTriangle();
				tNew.p1 = t.pts[1];
				tNew.p2 = c1;
				tNew.p3 = c2;

				tNew.uv1 = t.uv[1];
				tNew.uv2 = newuv1;
				tNew.uv3 = newuv2;
				tNew.ptexture = t.ptexture;  // 图片要给新的三角形

				AddTriangle();
			}
			else if (t.pts[1].z < zNear)
			{
				planeNear.LineCrossPlane(t.pts[1], t.pts[0], c1, &t1);
				planeNear.LineCrossPlane(t.pts[1], t.pts[2], c2, &t2);
				newuv1 = t.uv[1].Lerp(t.uv[0], t1);
				newuv2 = t.uv[1].Lerp(t.uv[2], t2);

				t.pts[1] = c1;
				t.uv[1] = newuv1;

				//使用新增的两个交点和原来的一个交点构成新的三角形
				Triangle& tNew = GetNextTriangle();
				tNew.p1 = t.pts[2];
				tNew.p2 = c1;
				tNew.p3 = c2;

				tNew.uv1 = t.uv[2];
				tNew.uv2 = newuv1;
				tNew.uv3 = newuv2;
				tNew.ptexture = t.ptexture;

				AddTriangle();
			}
			else if (t.pts[2].z < zNear)
			{
				planeNear.LineCrossPlane(t.pts[2], t.pts[1], c1, &t1);
				planeNear.LineCrossPlane(t.pts[2], t.pts[0], c2, &t2);
				newuv1 = t.uv[2].Lerp(t.uv[1], t1);
				newuv2 = t.uv[2].Lerp(t.uv[0], t2);

				t.pts[2] = c1;
				t.uv[2] = newuv1;

				//使用新增的两个交点和原来的一个交点构成新的三角形
				Triangle& tNew = GetNextTriangle();
				tNew.p1 = t.pts[0];
				tNew.p2 = c1;
				tNew.p3 = c2;

				tNew.uv1 = t.uv[0];
				tNew.uv2 = newuv1;
				tNew.uv3 = newuv2;
				tNew.ptexture = t.ptexture;

				AddTriangle();
			}
		}break;
		}

		//z1 = mTerrainRenderTriangle[i].p1.z;  // 保存之前原有的深度值(摄像机坐标下)
		//z2 = mTerrainRenderTriangle[i].p2.z;
		//z3 = mTerrainRenderTriangle[i].p3.z;

		//能够在这里绘制的三角形，要么是不需要裁剪的，或者是已经被裁掉一部分的三角形
		mTerrainRenderTriangle[i].Transform(matPV);  // 经过所有矩阵的变换

		//mTerrainRenderTriangle[i].p1.z = 1 / z1;  // 修改所有三角形顶点的z值(深度值)为倒数
		//mTerrainRenderTriangle[i].p2.z = 1 / z2;
		//mTerrainRenderTriangle[i].p3.z = 1 / z3;

		//mTerrainRenderTriangle[i].uv1 /= z1;  // 本来的uv坐标要/ 对应的z值
		//mTerrainRenderTriangle[i].uv2 /= z2;
		//mTerrainRenderTriangle[i].uv3 /= z3;

		//MRB.DrawTriangle_Solid(
		//	mTerrainRenderTriangle[i].p1.x, mTerrainRenderTriangle[i].p1.y, mTerrainRenderTriangle[i].p1.z,
		//	mTerrainRenderTriangle[i].p2.x, mTerrainRenderTriangle[i].p2.y, mTerrainRenderTriangle[i].p2.z,
		//	mTerrainRenderTriangle[i].p3.x, mTerrainRenderTriangle[i].p3.y, mTerrainRenderTriangle[i].p3.z,
		//	mTerrainRenderTriangle[i].uv1, mTerrainRenderTriangle[i].uv2, mTerrainRenderTriangle[i].uv3,
		//	mTerrainRenderTriangle[i].ptexture);


		MRB.DrawTriangle_Empty(
			mTerrainRenderTriangle[i].p1.x, mTerrainRenderTriangle[i].p1.y,
			mTerrainRenderTriangle[i].p2.x, mTerrainRenderTriangle[i].p2.y,
			mTerrainRenderTriangle[i].p3.x, mTerrainRenderTriangle[i].p3.y,
			COLOR32::Pink());
	}

	//绘制可能的新增的三角形
	for (int i = oldIndex; i < mCurIndex; ++i)
	{
		//z1 = mTerrainRenderTriangle[i].p1.z;
		//z2 = mTerrainRenderTriangle[i].p2.z;
		//z3 = mTerrainRenderTriangle[i].p3.z;

		//能够在这里绘制的三角形，要么是不需要裁剪的，或者是已经被裁掉一部分的三角形
		mTerrainRenderTriangle[i].Transform(matPV);

		//mTerrainRenderTriangle[i].p1.z = 1 / z1;
		//mTerrainRenderTriangle[i].p2.z = 1 / z2;
		//mTerrainRenderTriangle[i].p3.z = 1 / z3;

		//mTerrainRenderTriangle[i].uv1 /= z1;
		//mTerrainRenderTriangle[i].uv2 /= z2;
		//mTerrainRenderTriangle[i].uv3 /= z3;

		//MRB.DrawTriangle_Solid(
		//	mTerrainRenderTriangle[i].p1.x, mTerrainRenderTriangle[i].p1.y, mTerrainRenderTriangle[i].p1.z,
		//	mTerrainRenderTriangle[i].p2.x, mTerrainRenderTriangle[i].p2.y, mTerrainRenderTriangle[i].p2.z,
		//	mTerrainRenderTriangle[i].p3.x, mTerrainRenderTriangle[i].p3.y, mTerrainRenderTriangle[i].p3.z,
		//	mTerrainRenderTriangle[i].uv1, mTerrainRenderTriangle[i].uv2, mTerrainRenderTriangle[i].uv3,
		//	mTerrainRenderTriangle[i].ptexture);


		MRB.DrawTriangle_Empty(
			mTerrainRenderTriangle[i].p1.x, mTerrainRenderTriangle[i].p1.y,
			mTerrainRenderTriangle[i].p2.x, mTerrainRenderTriangle[i].p2.y,
			mTerrainRenderTriangle[i].p3.x, mTerrainRenderTriangle[i].p3.y,
			COLOR32::Pink());
	}
}

void TerrainRenderList::ClearTriangle()
{
	mCurIndex = 0;
}
