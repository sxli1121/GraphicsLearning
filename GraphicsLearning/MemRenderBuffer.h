#pragma once
#ifndef __MEMORY_RENDER_BUFFER_H__
#define __MEMORY_RENDER_BUFFER_H__
#include <vector>
#include <deque>
#include "SingletonTemplate.h"
#include "UtilityHelper.h"
#include "Geometric.h"
#include "./math3d/Vector2.h"
#include "math3d/Vector3.h"

using namespace Math;

using namespace Utility;

class MemRenderBuffer :public Singleton<MemRenderBuffer>
{
	PRIVATE_OPEN(MemRenderBuffer)
public:
	MemRenderBuffer();
	MemRenderBuffer(const MemRenderBuffer& that);
	MemRenderBuffer& operator = (const MemRenderBuffer& that);
public:
	~MemRenderBuffer();

	bool Create(int row, int col);

	unsigned char* BufferData() { return (unsigned char*)mBuffer; }

	//实现一些绘制相关的函数
	void ClearZ();   //清空深度值
	void FillColor(const COLOR32& color);
	void DrawPixel(int x, int y, const COLOR32& color);

	//绘制实心的矩形
	void DrawRectangleSolid(int x1, int y1, int x2, int y2, const COLOR32& color);
	//绘制空心的矩形
	void DrawRectangleEmpty(int x1, int y1, int x2, int y2, const COLOR32& color);

	//今天绘制直线的时候先不考虑直线的裁剪问题
	//DDA方法绘制直线(微分思想)
	void DrawLine(int x1, int y1, int x2, int y2, const COLOR32& color);

	//基于中间点的二分递归绘制直线
	void DrawLine_bin_mp(int x1, int y1, int x2, int y2, const COLOR32& color);

	//基于递推增量的中点画线算法
	void DrawLine_mp(int x1, int y1, int x2, int y2, const COLOR32& color);

	//Bresenham 直线绘制算法 斜率 0-1
	void DrawLine_Bresenham_C1(int x1, int y1, int x2, int y2, const COLOR32& color);
	//Bresenham 直线绘制算法任意象限
	void DrawLine_Bresenham(int x1, int y1, int x2, int y2, const COLOR32& color);

	//颜色渐变插值绘制直线
	void DrawLine_Bresenham(int x1, int y1, int x2, int y2, const COLOR32& color1, const COLOR32& color2);

	int Row() const { return mRow; }
	int Col() const { return mCol; }
	const Rect& MyRect() const { return mRect; }

	// 绘制任意阶的贝塞尔曲线
	void DrawBezierCurve(std::vector<vec2f>& ctrlpoint, const COLOR32& color,
		size_t SampleCount = 100);

	//绘制均匀B样条线
	void DrawBSpline1(std::vector<vec2f>& ctrlpoint, const COLOR32& color,
		size_t SampleCount = 100);
	//绘制准均匀B样条线
	void DrawBSpline2(std::vector<vec2f>& ctrlpoint, const COLOR32& color,
		size_t SampleCount = 100);


	//正负画圆法
	void DrawCircle_pn(int xc, int yc, int r, const COLOR32& color);
	//中点画圆法
	void DrawCircle_mp(int xc, int yc, int r, const COLOR32& color);
	//Bresenham画圆法
	void DrawCircle_Bresenham(int xc, int yc, int r, const COLOR32& color);



	//三角形相关
	//绘制空心的三角形
	void DrawTriangle_Empty(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color);
	void DrawTriangle_Empty(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);
	void DrawTriangle_Solid(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color);
	//作业实现颜色渐变的实心三角形绘制
	void DrawTriangle_Solid(int x1, int y1, int x2, int y2, int x3, int y3,
		 COLOR32 color1,  COLOR32 color2,  COLOR32  color3);
	//用重心权重的方式绘制颜色渐变的实心三角形
	void DrawTriangle_Solid_W(int x1, int y1, int x2, int y2, int x3, int y3,
		COLOR32 color1, COLOR32 color2, COLOR32 color3);

	//带深度和纹理贴图的绘制
	void DrawTriangle_Solid(
		int x1, int y1, float z1,
		int x2, int y2, float z2,
		int x3, int y3, float z3,
		vec2f uv1, vec2f uv2, vec2f uv3,
		class MyTexture2D* ptex);




	//色盘的绘制
	void DrawColorPlate(int cx, int cy, int r);
protected:
	int _GetColorPlateAngle(const int& cx, const int& cy, const int& r, const int& x, const int& y, float& angle);

	//绘制平底三角形
	void _DrawTriangle_Solid_Buttom(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color);
	//绘制平顶三角形
	void _DrawTriangle_Solid_Top(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color);
	//绘制平底三角形
	void _DrawTriangle_Solid_Buttom(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);
	//绘制平顶三角形
	void _DrawTriangle_Solid_Top(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);
	//绘制平底三角形
	void _DrawTriangle_Solid_Buttom_W(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);
	//绘制平顶三角形
	void _DrawTriangle_Solid_Top_W(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);


	//绘制实心颜色渐变平顶平底三角形带深度值
	void _DrawTriangle_Solid_Top(
		int x1, int y1, float z1,
		int x2, int y2, float z2,
		int x3, int y3, float z3,
		vec2f uv1, vec2f uv2, vec2f uv3, class MyTexture2D* ptex);
	void _DrawTriangle_Solid_Bottom(
		int x1, int y1, float z1,
		int x2, int y2, float z2,
		int x3, int y3, float z3,
		vec2f uv1, vec2f uv2, vec2f uv3, class MyTexture2D* ptex);




	//八分画圆
	void _DrawCirclePoint8(int x, int y, int p, int q, const COLOR32& color);


	//绘制辅助函数
	//中点裁剪算法
	bool _ClipLine_mp(int& x1, int& y1, int& x2, int& y2);
	//中点裁剪算法的辅助函数
	int __EnCode(float x, float y); // 判断点的方位
	int __BorderCode(float x, float y); // 边界上的点的判断
	bool __NotFinish(float x, float y); // 还没有结束
	bool __ClipLineMP(float& px1, float& py1, float px2, float py2); // 一次求从p1到p2的交点


	//Cohen Suther Land  （科恩·萨瑟 兰德）
	bool _ClipLine_csl(int& x1, int& y1, int& x2, int& y2);


	// 先实现一次求贝塞尔曲线上点的几个函数(二阶、三阶、任意阶)
	void __BezierCurve2oft(vec2f& pt, const vec2f* pc, float t);

	void __BezierCurve3oft(vec2f& pt, const vec2f* pc, float t);

	void __BezierCurveoft(vec2f& pt, std::vector<vec2f>& ctrlpoint, float t);


	//计算三角形重心权重
	void _TriangleWeight(const int& x1, const int& y1, const int& x2, const int& y2,
		const int& x3, const int& y3, const int& px, const int& py, float& w1, float& w2, float& w3);

public:
	//======================================

	//作业：绘制带宽度的直线
	void DrawWidthLine1(int x1, int y1, int x2, int y2, int width, const COLOR32& color);
	void DrawWidthLine2(int x1, int y1, int x2, int y2, int width, const COLOR32& color);
	void DrawWidthLine3(int x1, int y1, int x2, int y2, float width, const COLOR32& color);
	void DrawWidthLine4(int x1, int y1, int x2, int y2, float width, const COLOR32& color);
	//增加的辅助函数
	//获取直线的点
	void GetLinePoints(int x1, int y1, int x2, int y2, std::vector<vec2f>& points);
	//填充圆形
	void FillCircleColor(int x, int y, int radius, const COLOR32& color);
	//=========================================

	//标准轴椭圆，长短轴在x或者y上
	//借鉴正负法画椭圆
	void DrawElliptic_pn(int cx, int cy, int a, int b, const COLOR32& color);

	//参考：https://zhuanlan.zhihu.com/p/421067969?utm_id=0
	// https://blog.csdn.net/qq_42185999/article/details/102377383
	//中点画椭圆
	void DrawElliptic_mp(int cx, int cy, int a, int b, const COLOR32& color);
	void DrawElliptic_Bresenham(int cx, int cy, int a, int b, const COLOR32& color);

	//旋转的椭圆
	void DrawElliptic_Bresenham(int cx, int cy, int a, int b, const COLOR32& color, float angle);

	//=====================================================
protected:
	void _Copy(const MemRenderBuffer& that); 
	void _Clear();
private:
	int mRow;//行数
	int mCol;//列数
	int mPixleSize;//总的像素个数 = 行 * 列
	int mLineCount;//一行字节数

	Rect mRect;

	//颜色数据
	LPCOLOR32 mBuffer;//使用一维数组存储所有的像素的颜色数据
	LPPCOLOR32 mBufferArray;//存储每一行颜色的首地址，可以使用该变量用二维的方式访问颜色数据


	/*
	深度缓冲区的大小应该是和视口最后覆盖的区域的像素大小一致
	此处我们的视口大小和窗口的客户区大小是一致，所以我们的深
	缓冲区的大小就是整个绘制区域的大小，也就是颜色缓冲区的行
	列数(一个深度值决定一个像素颜色值)
	
	因为我们在投影以后比较的是摄像机坐标下的z的倒数
	(投影后的z坐标关于1/z成线性关系)
	z的范围是：zNear ~ zFar
	depth的范围：1/zFar ~ 1/zNear
	可以肯定的是：0 < 1/zFar < 1/zNear;
	为了方便使用，我们将深度值一开始的时候用0清空
	*/

	float* mZBuffer;
};
#define MRB MemRenderBuffer::Instance()
#endif