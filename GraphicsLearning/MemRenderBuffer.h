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

	//ʵ��һЩ������صĺ���
	void ClearZ();   //������ֵ
	void FillColor(const COLOR32& color);
	void DrawPixel(int x, int y, const COLOR32& color);

	//����ʵ�ĵľ���
	void DrawRectangleSolid(int x1, int y1, int x2, int y2, const COLOR32& color);
	//���ƿ��ĵľ���
	void DrawRectangleEmpty(int x1, int y1, int x2, int y2, const COLOR32& color);

	//�������ֱ�ߵ�ʱ���Ȳ�����ֱ�ߵĲü�����
	//DDA��������ֱ��(΢��˼��)
	void DrawLine(int x1, int y1, int x2, int y2, const COLOR32& color);

	//�����м��Ķ��ֵݹ����ֱ��
	void DrawLine_bin_mp(int x1, int y1, int x2, int y2, const COLOR32& color);

	//���ڵ����������е㻭���㷨
	void DrawLine_mp(int x1, int y1, int x2, int y2, const COLOR32& color);

	//Bresenham ֱ�߻����㷨 б�� 0-1
	void DrawLine_Bresenham_C1(int x1, int y1, int x2, int y2, const COLOR32& color);
	//Bresenham ֱ�߻����㷨��������
	void DrawLine_Bresenham(int x1, int y1, int x2, int y2, const COLOR32& color);

	//��ɫ�����ֵ����ֱ��
	void DrawLine_Bresenham(int x1, int y1, int x2, int y2, const COLOR32& color1, const COLOR32& color2);

	int Row() const { return mRow; }
	int Col() const { return mCol; }
	const Rect& MyRect() const { return mRect; }

	// ��������׵ı���������
	void DrawBezierCurve(std::vector<vec2f>& ctrlpoint, const COLOR32& color,
		size_t SampleCount = 100);

	//���ƾ���B������
	void DrawBSpline1(std::vector<vec2f>& ctrlpoint, const COLOR32& color,
		size_t SampleCount = 100);
	//����׼����B������
	void DrawBSpline2(std::vector<vec2f>& ctrlpoint, const COLOR32& color,
		size_t SampleCount = 100);


	//������Բ��
	void DrawCircle_pn(int xc, int yc, int r, const COLOR32& color);
	//�е㻭Բ��
	void DrawCircle_mp(int xc, int yc, int r, const COLOR32& color);
	//Bresenham��Բ��
	void DrawCircle_Bresenham(int xc, int yc, int r, const COLOR32& color);



	//���������
	//���ƿ��ĵ�������
	void DrawTriangle_Empty(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color);
	void DrawTriangle_Empty(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);
	void DrawTriangle_Solid(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color);
	//��ҵʵ����ɫ�����ʵ�������λ���
	void DrawTriangle_Solid(int x1, int y1, int x2, int y2, int x3, int y3,
		 COLOR32 color1,  COLOR32 color2,  COLOR32  color3);
	//������Ȩ�صķ�ʽ������ɫ�����ʵ��������
	void DrawTriangle_Solid_W(int x1, int y1, int x2, int y2, int x3, int y3,
		COLOR32 color1, COLOR32 color2, COLOR32 color3);

	//����Ⱥ�������ͼ�Ļ���
	void DrawTriangle_Solid(
		int x1, int y1, float z1,
		int x2, int y2, float z2,
		int x3, int y3, float z3,
		vec2f uv1, vec2f uv2, vec2f uv3,
		class MyTexture2D* ptex);




	//ɫ�̵Ļ���
	void DrawColorPlate(int cx, int cy, int r);
protected:
	int _GetColorPlateAngle(const int& cx, const int& cy, const int& r, const int& x, const int& y, float& angle);

	//����ƽ��������
	void _DrawTriangle_Solid_Buttom(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color);
	//����ƽ��������
	void _DrawTriangle_Solid_Top(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color);
	//����ƽ��������
	void _DrawTriangle_Solid_Buttom(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);
	//����ƽ��������
	void _DrawTriangle_Solid_Top(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);
	//����ƽ��������
	void _DrawTriangle_Solid_Buttom_W(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);
	//����ƽ��������
	void _DrawTriangle_Solid_Top_W(int x1, int y1, int x2, int y2, int x3, int y3,
		const COLOR32& color1, const COLOR32& color2, const COLOR32& color3);


	//����ʵ����ɫ����ƽ��ƽ�������δ����ֵ
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




	//�˷ֻ�Բ
	void _DrawCirclePoint8(int x, int y, int p, int q, const COLOR32& color);


	//���Ƹ�������
	//�е�ü��㷨
	bool _ClipLine_mp(int& x1, int& y1, int& x2, int& y2);
	//�е�ü��㷨�ĸ�������
	int __EnCode(float x, float y); // �жϵ�ķ�λ
	int __BorderCode(float x, float y); // �߽��ϵĵ���ж�
	bool __NotFinish(float x, float y); // ��û�н���
	bool __ClipLineMP(float& px1, float& py1, float px2, float py2); // һ�����p1��p2�Ľ���


	//Cohen Suther Land  ���ƶ�����ɪ ���£�
	bool _ClipLine_csl(int& x1, int& y1, int& x2, int& y2);


	// ��ʵ��һ�������������ϵ�ļ�������(���ס����ס������)
	void __BezierCurve2oft(vec2f& pt, const vec2f* pc, float t);

	void __BezierCurve3oft(vec2f& pt, const vec2f* pc, float t);

	void __BezierCurveoft(vec2f& pt, std::vector<vec2f>& ctrlpoint, float t);


	//��������������Ȩ��
	void _TriangleWeight(const int& x1, const int& y1, const int& x2, const int& y2,
		const int& x3, const int& y3, const int& px, const int& py, float& w1, float& w2, float& w3);

public:
	//======================================

	//��ҵ�����ƴ���ȵ�ֱ��
	void DrawWidthLine1(int x1, int y1, int x2, int y2, int width, const COLOR32& color);
	void DrawWidthLine2(int x1, int y1, int x2, int y2, int width, const COLOR32& color);
	void DrawWidthLine3(int x1, int y1, int x2, int y2, float width, const COLOR32& color);
	void DrawWidthLine4(int x1, int y1, int x2, int y2, float width, const COLOR32& color);
	//���ӵĸ�������
	//��ȡֱ�ߵĵ�
	void GetLinePoints(int x1, int y1, int x2, int y2, std::vector<vec2f>& points);
	//���Բ��
	void FillCircleColor(int x, int y, int radius, const COLOR32& color);
	//=========================================

	//��׼����Բ����������x����y��
	//�������������Բ
	void DrawElliptic_pn(int cx, int cy, int a, int b, const COLOR32& color);

	//�ο���https://zhuanlan.zhihu.com/p/421067969?utm_id=0
	// https://blog.csdn.net/qq_42185999/article/details/102377383
	//�е㻭��Բ
	void DrawElliptic_mp(int cx, int cy, int a, int b, const COLOR32& color);
	void DrawElliptic_Bresenham(int cx, int cy, int a, int b, const COLOR32& color);

	//��ת����Բ
	void DrawElliptic_Bresenham(int cx, int cy, int a, int b, const COLOR32& color, float angle);

	//=====================================================
protected:
	void _Copy(const MemRenderBuffer& that); 
	void _Clear();
private:
	int mRow;//����
	int mCol;//����
	int mPixleSize;//�ܵ����ظ��� = �� * ��
	int mLineCount;//һ���ֽ���

	Rect mRect;

	//��ɫ����
	LPCOLOR32 mBuffer;//ʹ��һά����洢���е����ص���ɫ����
	LPPCOLOR32 mBufferArray;//�洢ÿһ����ɫ���׵�ַ������ʹ�øñ����ö�ά�ķ�ʽ������ɫ����


	/*
	��Ȼ������Ĵ�СӦ���Ǻ��ӿ���󸲸ǵ���������ش�Сһ��
	�˴����ǵ��ӿڴ�С�ʹ��ڵĿͻ�����С��һ�£��������ǵ���
	�������Ĵ�С����������������Ĵ�С��Ҳ������ɫ����������
	����(һ�����ֵ����һ��������ɫֵ)
	
	��Ϊ������ͶӰ�Ժ�Ƚϵ�������������µ�z�ĵ���
	(ͶӰ���z�������1/z�����Թ�ϵ)
	z�ķ�Χ�ǣ�zNear ~ zFar
	depth�ķ�Χ��1/zFar ~ 1/zNear
	���Կ϶����ǣ�0 < 1/zFar < 1/zNear;
	Ϊ�˷���ʹ�ã����ǽ����ֵһ��ʼ��ʱ����0���
	*/

	float* mZBuffer;
};
#define MRB MemRenderBuffer::Instance()
#endif