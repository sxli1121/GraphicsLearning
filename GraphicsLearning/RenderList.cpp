#include "RenderList.h"
#include "MemRenderBuffer.h"

Triangle RenderList::mRenderTriangle[MAX_TRANGLE_NUM];

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
	Mat4x4f vpv;
	Camera::mainCamera->GetVPV(vpv);

	for (int i = 0;i<mCurIndex;++i)
	{
		mRenderTriangle[i].Transform(vpv);

		MRB.DrawTriangle_Empty(
			mRenderTriangle[i].p1.x, mRenderTriangle[i].p1.y,
			mRenderTriangle[i].p2.x, mRenderTriangle[i].p2.y,
			mRenderTriangle[i].p3.x, mRenderTriangle[i].p3.y,
			COLOR32::White()
		);
	}

	printf("\r总共绘制了 %d 个三角形      ", mCurIndex);
}