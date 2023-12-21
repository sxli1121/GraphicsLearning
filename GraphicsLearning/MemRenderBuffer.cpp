#include "MemRenderBuffer.h"
#include <iostream>
#include "./math3d/Vector3.h"
#include "math3d/Matrix3x3.h"
#include "MyTexture2D.h"


MemRenderBuffer::MemRenderBuffer()
{
	mBuffer = nullptr;
	mBufferArray = nullptr;
	mZBuffer = nullptr;
	mLineCount = mPixleSize = mCol = mRow = 0;
}

MemRenderBuffer::MemRenderBuffer(const MemRenderBuffer& that)
{
	_Copy(that);
}

MemRenderBuffer& MemRenderBuffer::operator=(const MemRenderBuffer& that)
{
	if (this != &that)
	{
		_Clear();
		_Copy(that);
	}
	return *this;
}

MemRenderBuffer::~MemRenderBuffer()
{
	_Clear();
}

bool MemRenderBuffer::Create(int row, int col)
{
	if (row <= 0 || col <= 0) { return false; }
	if (row == mRow && col == mCol) { return true; };
	_Clear();
	mRow = row;
	mCol = col;
	mRect.Set(0, 0, mCol, mRow);  // ���ھ���
	mPixleSize = mRow * mCol;
	mBuffer = new COLOR32[mPixleSize];
	mBufferArray = new LPCOLOR32[mRow];
	mLineCount = mCol * sizeof(COLOR32);
	mZBuffer = new float[mPixleSize];

	for (int i = 0; i < mRow; ++i)
	{
		//ÿһ�е��׵�ַ
		mBufferArray[i] = mBuffer + i * mCol;
	}
	return true;
}

void MemRenderBuffer::ClearZ()
{
	memset(mZBuffer, 0, sizeof(float) * mPixleSize);
}

void MemRenderBuffer::FillColor(const COLOR32& color)
{
	if (mBuffer == nullptr) { return; }
	//�����һ����ɫ����
	for (int i = 0; i < mCol; ++i)
	{
		memcpy(&mBuffer[i], &color, sizeof(COLOR32));
	}

	//����һ�е����ݸ��Ƶ�ʣ�µ�ÿһ��
	for (int i = 1; i < mRow; ++i)
	{
		memcpy(mBufferArray[i], mBufferArray[0], mLineCount);
	}
}

void MemRenderBuffer::DrawPixel(int x, int y, const COLOR32& color)
{
	if (mBuffer == nullptr) { return; }
	if (x < 0 || x >= mCol || y < 0 || y >= mRow) { return; }
	memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
}

void MemRenderBuffer::DrawRectangleSolid(int x1, int y1, int x2, int y2, const COLOR32& color)
{
	Rect current(x1, y1, x2, y2);
	Rect cross;
	if (current.CrossRect(mRect, cross))
	{
		int w = cross.width();
		int h = cross.height();

		for (int i = 0; i < w; ++i)
		{
			memcpy(&mBufferArray[cross.top][cross.left + i], &color, sizeof(COLOR32));
		}

		size_t linecount = sizeof(COLOR32) * w;
		for (int i = 1; i < h; ++i)
		{
			memcpy(&mBufferArray[cross.top + i][cross.left], &mBufferArray[cross.top][cross.left], linecount);
		}
	}
}

void MemRenderBuffer::DrawRectangleEmpty(int x1, int y1, int x2, int y2, const COLOR32& color)
{
	Rect current(x1, y1, x2, y2);
	Rect cross;
	if (current.CrossRect(mRect, cross))
	{
		int w = cross.width();
		int h = cross.height();

		for (int i = 0; i < w; ++i)
		{
			memcpy(&mBufferArray[cross.top][cross.left + i], &color, sizeof(COLOR32));
			memcpy(&mBufferArray[cross.bottom][cross.left + i], &color, sizeof(COLOR32));
		}


		for (int i = 1; i < h; ++i)
		{
			memcpy(&mBufferArray[cross.top +i][cross.left], &color, sizeof(COLOR32));
			memcpy(&mBufferArray[cross.top+i][cross.right-1], &color, sizeof(COLOR32));
		}
	}
}

void MemRenderBuffer::DrawLine(int x1, int y1, int x2, int y2, const COLOR32& color)
{
	if (x1 == x2)
	{
		int ymin, ymax;
		ymin = min(y1, y2);
		ymax = max(y1, y2);
		for (int y = ymin; y <= ymax; ++y)
		{
			memcpy(&mBufferArray[y][x1], &color, sizeof(COLOR32));
		}
		return;
	}
	else if (y1 == y2)
	{
		int xmin, xmax;
		xmin = min(x1, x2);
		xmax = max(x1, x2);
		for (int x = xmin; x <= xmax; ++x)
		{
			memcpy(&mBufferArray[y1][x], &color, sizeof(COLOR32));
		}
		return;
	}


	int delta_y = y1 - y2;
	int delta_x = x1 - x2;

	int delta_ay = abs(delta_y);
	int delta_ax = abs(delta_x);

	if (delta_ay < delta_ax) //x 
	{
		float k = float(delta_y) / delta_x;
		int xmin, xmax;
		float y;
		if (x1 < x2)
		{
			xmin = x1;
			xmax = x2;
			y = float(y1);
		}
		else
		{
			xmin = x2;
			xmax = x1;
			y = float(y2);
		}
		for (int x = xmin; x <= xmax; x++)
		{
			memcpy(&mBufferArray[int(y)][x], &color, sizeof(COLOR32));
			y += k;
		}
	}
	else  //y
	{
		float k = float(delta_x) / delta_y;
		int ymin, ymax;
		float x;
		if (y1 < y2)
		{
			ymin = y1;
			ymax = y2;
			x = float(x1);
		}
		else
		{
			ymin = y2;
			ymax = y1;
			x = float(x2);
		}

		for (int y = ymin; y <= ymax; ++y)
		{
			memcpy(&mBufferArray[y][int(x)], &color, sizeof(COLOR32));
			x += k;
		}
	}
}

void MemRenderBuffer::DrawLine_bin_mp(int x1, int y1, int x2, int y2, const COLOR32& color)
{
	// �Ƴ�����
	int dx = x2 - x1;
	int dy = y2 - y1;
	if (dx * dx + dy * dy <= 2)
	{
		mBufferArray[y1][x1] = color;
		mBufferArray[y2][x2] = color;
		return;
	}

	// �����е�
	int x = (x1 + x2) / 2;
	int y = (y1 + y2) / 2;
	mBufferArray[y][x] = color;

	DrawLine_bin_mp(x1, y1, x, y, color);
	DrawLine_bin_mp(x, y, x2, y2, color);
}

void MemRenderBuffer::DrawLine_mp(int x1, int y1, int x2, int y2, const COLOR32& color)
{
	//ֱ�߻���
	//�ȴ���һЩ��������
	if (x1 == x2)//����
	{
		int ymin, ymax;
		ymin = min(y1, y2);
		ymax = max(y1, y2);
		for (int y = ymin; y <= ymax; ++y)
		{
			memcpy(&mBufferArray[y][x1], &color, sizeof(COLOR32));
		}
		return;
	}
	else if (y1 == y2)//����
	{
		int xmin, xmax;
		xmin = min(x1, x2);
		xmax = max(x1, x2);
		for (int x = xmin; x <= xmax; ++x)
		{
			memcpy(&mBufferArray[y1][x], &color, sizeof(COLOR32));
		}
		return;
	}

	int x, y, xEnd, yEnd;
	if (x1 < x2)
	{
		x = x1; xEnd = x2; y = y1; yEnd = y2;
	}
	else
	{
		x = x2; xEnd = x1; y = y2; yEnd = y1;
	}

	int a = y - yEnd;
	int b = xEnd - x;
	if (yEnd >= y && (yEnd - y) <= (xEnd - x))
	{
		int _2a = a + a;
		int d = _2a + b;   // ����x ����y  ÿ�δ����a ����b �ǲ�ͬ��
		int _2b = b + b;

		while (x <= xEnd)
		{
			memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
			if (d >= 0)
			{
				++x;
				d = d + _2a;
			}
			else
			{
				++x;
				++y;
				d = d + _2a + _2b;
			}
		}
	}
	else if (yEnd < y && (y - yEnd) <= (xEnd - x))   // �����·����߶� �Ƕ�< 45��
	{
		int _2a = a + a;
		int d = _2a - b;//d0 = a+0.5b;//2d0=2a+b ��2d0 Ϊ d
		int _2b = -b - b;

		while (x <= xEnd)
		{
			memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
			if (d >= 0)    //  �е����� ������ ѡ��
			{
				++x;
				--y;
				d = d + _2a + _2b;
			}
			else   //  �е����� ƽ����  ѡ��
			{
				++x;
				d = d + _2a;
			}
		}
	}
	else if (yEnd >= y && (yEnd - y) > (xEnd - x))   // �����Ϸ����߶� �Ƕ�> 45��
	{
		int _2a = a + a;
		int _2b = b + b;
		int d = a + _2b;

		while (y <= yEnd)
		{
			memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
			if (d >= 0)  // �е����� ������ ѡ��
			{
				++x;
				++y;
				d = d + _2a + _2b;
			}
			else  //  �е����� ƽ���� ѡ��
			{
				++y;
				d = d + _2b;
			}
		}
	}
	else if (yEnd<y && (y - yEnd)>(xEnd - x))  // �����·����߶� �Ƕ�> 45��
	{
		int _2a = a + a;
		int _2b = -b - b;
		int d = a - b - b;
		while (y >= yEnd)
		{
			memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
			if (d >= 0)   // �е����� ƽ���� ѡ��
			{
				--y;
				d = d + _2b;
			}
			else      // �е����� ������ ѡ��
			{
				--y;
				++x;
				d = d + _2a + _2b;
			}
		}
	}
}

void MemRenderBuffer::DrawLine_Bresenham_C1(int x1, int y1, int x2, int y2, const COLOR32& color)
{
	int dx, dy, x = x1, y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	int e = dy + dy - dx;
	int _2dx = dx + dx;
	int _2dy = dy + dy;

	while (x < x2)
	{
		mBufferArray[y][x] = color;
		if (e > 0)
		{
			e = e + _2dy - _2dx;
			++y;
		}
		else
		{
			e = e + _2dy;
		}
		++x;
	}
}

void MemRenderBuffer::DrawLine_Bresenham(int x1, int y1, int x2, int y2, const COLOR32& color)
{
	if (!_ClipLine_mp(x1, y1, x2, y2)) // �ü�
	{
		return; 
	}

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	bool bInterchange = false;   // �Ƿ񽻻�x y ��ֵ
	int e, temp, signx, signy;   // signx, signy 

	// �ж� x2��x1 �Ĺ�ϵ  x2����x1Ϊ1 ����Ϊ0 С��Ϊ-1  
	signx = (x2 > x1) ? 1 : (x2 < x1) ? -1 : 0;  // ���x2 > x1  signx=1   x2 ��> x1  �ж� x2 < x1   ���x2 < x1 signx=-1  ���� signx =0��
	signy = (y2 > y1) ? 1 : (y2 < y1) ? -1 : 0;

	if (dy > dx) // y�ı仯������x�ı仯�� ����x��y�ı仯��  ����y�ƶ�
 	{
		temp = dx;
		dx = dy;
		dy = temp;
		bInterchange = true;
	}

	e = -dx;
	int x = x1, y = y1;
	int _2dy = dy + dy;
	int _2dx = dx + dx;

	if (!bInterchange) // ԭ���ǰ���x�ķ����ƶ�
	{
		for (int i = 0; i < dx; ++i)
		{
			mBufferArray[y][x] = color;
			x += signx;  
			e += _2dy;
			if (e >= 0)
			{
				y += signy;
				e -= _2dx;
			}
		}
	}
	else
	{
		for (int i = 0; i < dx; ++i)
		{
			mBufferArray[y][x] = color;
			y += signy;
			e += _2dy;
			if (e >= 0)
			{
				x += signx;
				e -= _2dx;
			}
		}
	}
}

void MemRenderBuffer::DrawLine_Bresenham(int x1, int y1, int x2, int y2, const COLOR32& color1, const COLOR32& color2)
{
	vec2f p1(x1, y1);
	vec2f p2(x2, y2);
	float len = (p2 - p1).magnitude();

	if (!_ClipLine_mp(x1, y1, x2, y2)) // �ü�
	{
		return;
	}

	// �ü��Ժ��
	vec2f _p1(x1, y1);
	vec2f _p2(x2, y2);
	float lenp1 = (_p1 - p1).magnitude();
	float lenp2 = (_p2 - p1).magnitude();

	float t1 = lenp1 / len;
	float t2 = lenp2 / len;

	// 
	vec3f c1 = color1;
	vec3f c2 = color2;

	vec3f newc1 = c1.Lerp(c2, t1);
	vec3f newc2 = c1.Lerp(c2, t2);


	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	bool bInterchange = false;   // �Ƿ񽻻�x y ��ֵ
	int e, temp, signx, signy;   // signx, signy 

	// �ж� x2��x1 �Ĺ�ϵ  x2����x1Ϊ1 ����Ϊ0 С��Ϊ-1  
	signx = (x2 > x1) ? 1 : (x2 < x1) ? -1 : 0;  // ���x2 > x1  signx=1   x2 ��> x1  �ж� x2 < x1   ���x2 < x1 signx=-1  ���� signx =0��
	signy = (y2 > y1) ? 1 : (y2 < y1) ? -1 : 0;

	if (dy > dx) // y�ı仯������x�ı仯�� ����x��y�ı仯��  ����y�ƶ�
	{
		temp = dx;
		dx = dy;
		dy = temp;
		bInterchange = true;
	}

	e = -dx;
	int x = x1, y = y1;
	int _2dy = dy + dy;
	int _2dx = dx + dx;

	if (!bInterchange) // ԭ���ǰ���x�ķ����ƶ�
	{
		float t = 0;
		for (int i = 0; i < dx; ++i)
		{
			t = i / float(dx - 1);
			mBufferArray[y][x] = newc1.Lerp(newc2, t);
			x += signx;
			e += _2dy;
			if (e >= 0)
			{
				y += signy;
				e -= _2dx;
			}
		}
	}
	else
	{
		float t = 0;
		for (int i = 0; i < dx; ++i)
		{
			t = i / float(dx - 1);
			mBufferArray[y][x] = newc1.Lerp(newc2, t);
			y += signy;
			e += _2dy;
			if (e >= 0)
			{
				x += signx;
				e -= _2dx;
			}
		}
	}
}

void MemRenderBuffer::DrawBezierCurve(std::vector<vec2f>& ctrlpoint, const COLOR32& color, size_t SampleCount)
{
	size_t n = ctrlpoint.size();
	if (n < 2) { return; }
	if (n == 2)
	{
		DrawLine_Bresenham(ctrlpoint[0].x, ctrlpoint[0].y, ctrlpoint[1].x, ctrlpoint[1].y, color);
	}
	if (SampleCount < 5)
	{
		SampleCount = 5;
	}

	size_t fsize = SampleCount - 1;  // ���Ƽ����߶�
	float dt = 1.0f / fsize;
	float curt = dt;
	vec2f p1 = ctrlpoint[0];
	vec2f p2;

	for (size_t i = 1; i < fsize; i++)
	{
		__BezierCurveoft(p2, ctrlpoint, curt);
		curt += dt;
		DrawLine_Bresenham(p1.x, p1.y, p2.x, p2.y, color);
		p1 = p2;
	}
	DrawLine_Bresenham(p1.x, p1.y, ctrlpoint[n - 1].x, ctrlpoint[n - 1].y, color);
}

void MemRenderBuffer::DrawBSpline1(std::vector<vec2f>& ctrlpoint, const COLOR32& color, size_t SampleCount)
{
	//�µĿ��Ƶ�
	vec2f newcps[3];
	size_t size = ctrlpoint.size();
	if (size < 3) { return; }
	size_t nsize = size - 2;//���ݿ��Ƶ�������������Ҫ���ƶ������߶�

	size_t fsize = SampleCount - 1;
	float dt = 1.0f / fsize, ct = dt;
	vec2f p1, p2;

	for (int i = 0; i < nsize; ++i)
	{
		newcps[0] = (ctrlpoint[i] + ctrlpoint[i + 1]) * 0.5f;
		newcps[1] = ctrlpoint[i + 1];
		newcps[2] = (ctrlpoint[i + 1] + ctrlpoint[i + 2]) * 0.5f;

		p1 = newcps[0]; ct = dt;
		for (size_t j = 1; j < fsize; ++j)
		{
			__BezierCurve2oft(p2, newcps, ct);
			ct += dt;
			DrawLine_Bresenham(p1.x, p1.y, p2.x, p2.y, color);
			p1 = p2;
		}
		DrawLine_Bresenham(p1.x, p1.y, newcps[2].x, newcps[2].y, color);
	}
}

void MemRenderBuffer::DrawBSpline2(std::vector<vec2f>& ctrlpoint, const COLOR32& color, size_t SampleCount)
{
	size_t size = ctrlpoint.size();
	if (size < 3) { return; }
	std::vector<vec2f> ncv = ctrlpoint;
	//��������ų���ڶ�����ķ����ӳ�һ����Ϊ�µĿ��Ƶ����
	ncv[0] = ctrlpoint[0] + (ctrlpoint[0] - ctrlpoint[1]);
	//ͬ�����յ����ų������ڶ�����ķ����ӳ�һ����Ϊ�µĿ��Ƶ��յ�
	ncv.back() = ctrlpoint[size - 1] + (ctrlpoint[size - 1] - ctrlpoint[size - 2]);
	DrawBSpline1(ncv, color, SampleCount);
}

void MemRenderBuffer::DrawCircle_pn(int xc, int yc, int r, const COLOR32& color)
{
	//�����һ����(F(x0,y0))
	int x = 0, y = r;
	//F(x0,y0) => x0^2 + y0^2 - r^2 = 0
	int f = 0;
	//����F(x,y) <=0 �����ߣ�����F(x,y)������
	//��������ߣ���һ��fֵ���� F(x,y)+2x+1
	//����������ߣ���һ��fֵ���� F(x,y)-2y+1

	int count = 0;
	while (x <= y)
	{
		_DrawCirclePoint8(xc, yc, x, y, color);
		if (f <= 0)//��ʾ��Բ�ϻ���Բ��
		{
			f += x + x + 1;
			++x;
		}
		else
		{
			f += 1 - y - y;
			--y;
		}
		++count;
	}
	printf("������Բ��������%d\n", count);
}

void MemRenderBuffer::DrawCircle_mp(int xc, int yc, int r, const COLOR32& color)
{
	int x = 0, y = r;
	float d = 1.25f - r;
	_DrawCirclePoint8(xc, yc, x, y, color);
	int count = 1;
	while (x < y)
	{
		if (d < 0)//������
		{
			d += x + x + 3;
		}
		else//������
		{
			d += x + x - y - y + 5;
			--y;
		}
		++x;
		_DrawCirclePoint8(xc, yc, x, y, color);
		++count;
	}
	printf("�е㻭Բ��������%d\n", count);
}

void MemRenderBuffer::DrawCircle_Bresenham(int xc, int yc, int r, const COLOR32& color)
{
	int x = 0, y = r;
	int d = 3 - r - r;

	_DrawCirclePoint8(xc, yc, x, y, color);
	int count = 1;
	while (x < y)
	{
		if (d < 0)//������
		{

			d += x + x + x + x + 6;
		}
		else//������
		{
			d += x + x + x + x - y - y - y - y + 10;
			--y;
		}
		++x;
		_DrawCirclePoint8(xc, yc, x, y, color);
		++count;
	}
	printf("Bresenham��Բ��������%d\n", count);
}

void MemRenderBuffer::DrawTriangle_Empty(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color)
{
	DrawLine_Bresenham(x1, y1, x2, y2, color);
	DrawLine_Bresenham(x2, y2, x3, y3, color);
	DrawLine_Bresenham(x3, y3, x1, y1, color);
}

void MemRenderBuffer::DrawTriangle_Empty(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color1, const COLOR32& color2, const COLOR32& color3)
{
	DrawLine_Bresenham(x1, y1, x2, y2, color1, color2);
	DrawLine_Bresenham(x2, y2, x3, y3, color2, color3);
	DrawLine_Bresenham(x3, y3, x1, y1, color3, color1);
}

void MemRenderBuffer::DrawTriangle_Solid(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color)
{
	//����һЩ��������
	if (y1 == y2 && y2 == y3)//ˮƽ��
	{
		int minx = min(min(x1, x2), x3);
		int maxx = max(max(x1, x2), x3);
		DrawLine_Bresenham(minx, y1, maxx, y1, color);
		return;
	}
	else if (x1 == x2 && x2 == x3)
	{
		int miny = min(min(y1, y2), y3);
		int maxy = max(max(y1, y2), y3);
		DrawLine_Bresenham(x1, miny, x1, maxy, color);
		return;
	}
	//���Ҫ��������б�ŵ�ֱ�ߵ�ʱ�򣬲鿴б���Ƿ���ȾͿ�����
	//����͵��ʡ����

	//���ǵ������η�Ϊһ�������κ�ƽ���������Լ�ƽ��������
	//����Ҫ��ȷ�������ε�����
	//Ϊ�˱�֤�жϵ��߼�һ�£����ǽ�����Ĳ�������һ������Yֵ�Ƚϵ���˳��
	//�������֤ y1 <= y2 <= y3
	if (y2 > y3)
	{
		int x = x2; x2 = x3; x3 = x;
		int y = y2; y2 = y3; y3 = y;
	}
	if (y1 > y2)
	{
		int x = x2; x2 = x1; x1 = x;
		int y = y2; y2 = y1; y1 = y;
	}
	if (y2 > y3)
	{
		int x = x2; x2 = x3; x3 = x;
		int y = y2; y2 = y3; y3 = y;
	}
	//�ж�����
	if (y2 == y3)//ƽ��������
	{
		if (x2 < x3)
		{
			_DrawTriangle_Solid_Buttom(x1, y1, x2, y2, x3, y3, color);
		}
		else
		{
			_DrawTriangle_Solid_Buttom(x1, y1, x3, y3, x2, y2, color);
		}
	}
	else if (y1 == y2)//ƽ��������
	{
		if (x1 < x2)
		{
			_DrawTriangle_Solid_Top(x1, y1, x2, y2, x3, y3, color);
		}
		else
		{
			_DrawTriangle_Solid_Top(x2, y2, x1, y1, x3, y3, color);
		}
	}
	else//��Ҫ��һ��������ηֽ�Ϊƽ����ƽ��������
	{
		int ny = y2;
		int nx = x1 + float(x3 - x1) * (y2 - y1) / (y3 - y1);
		if (nx < x2)
		{
			_DrawTriangle_Solid_Buttom(x1, y1, nx, ny, x2, y2, color);
			_DrawTriangle_Solid_Top(nx, ny, x2, y2, x3, y3, color);
		}
		else
		{
			_DrawTriangle_Solid_Buttom(x1, y1, x2, y2, nx, ny, color);
			_DrawTriangle_Solid_Top(x2, y2, nx, ny, x3, y3, color);
		}
	}
}

void MemRenderBuffer::DrawTriangle_Solid(int x1, int y1, int x2, int y2, int x3, int y3, 
	 COLOR32 color1,  COLOR32 color2,  COLOR32 color3)
{
	//����һЩ��������
	if (y1 == y2 && y2 == y3)//ˮƽ��
	{
		int minx = min(min(x1, x2), x3);
		int maxx = max(max(x1, x2), x3);

		COLOR32 c1 = minx == x1 ? color1 : minx == x2 ? color2 : color3;
		COLOR32 c2 = maxx == x1 ? color1 : maxx == x2 ? color2 : color3;
		DrawLine_Bresenham(minx, y1, maxx, y1, c1, c2);
		return;
	}
	else if (x1 == x2 && x2 == x3)
	{
		int miny = min(min(y1, y2), y3);
		int maxy = max(max(y1, y2), y3);
		COLOR32 c1 = miny == y1 ? color1 : miny == y2 ? color2 : color3;
		COLOR32 c2 = maxy == y1 ? color1 : maxy == y2 ? color2 : color3;
		DrawLine_Bresenham(x1, miny, x1, maxy, c1, c2);
		return;
	}
	//���Ҫ��������б�ŵ�ֱ�ߵ�ʱ�򣬲鿴б���Ƿ���ȾͿ�����
	//����͵��ʡ����

	//���ǵ������η�Ϊһ�������κ�ƽ���������Լ�ƽ��������
	//����Ҫ��ȷ�������ε�����
	//Ϊ�˱�֤�жϵ��߼�һ�£����ǽ�����Ĳ�������һ������Yֵ�Ƚϵ���˳��
	//�������֤ y1 <= y2 <= y3
	if (y2 > y3)
	{
		int x = x2; x2 = x3; x3 = x;
		int y = y2; y2 = y3; y3 = y;
		COLOR32 c = color2; color2 = color3; color3 = c;
	}
	if (y1 > y2)
	{
		int x = x2; x2 = x1; x1 = x;
		int y = y2; y2 = y1; y1 = y;
		COLOR32 c = color2; color2 = color1; color1 = c;
	}
	if (y2 > y3)
	{
		int x = x2; x2 = x3; x3 = x;
		int y = y2; y2 = y3; y3 = y;
		COLOR32 c = color2; color2 = color3; color3 = c;
	}
	//�ж�����
	if (y2 == y3)//ƽ��������
	{
		if (x2 < x3)
		{
			_DrawTriangle_Solid_Buttom(x1, y1, x2, y2, x3, y3, color1, color2, color3);
		}
		else
		{
			_DrawTriangle_Solid_Buttom(x1, y1, x3, y3, x2, y2, color1, color3, color2);
		}
	}
	else if (y1 == y2)//ƽ��������
	{
		if (x1 < x2)
		{
			_DrawTriangle_Solid_Top(x1, y1, x2, y2, x3, y3, color1, color2, color3);
		}
		else
		{
			_DrawTriangle_Solid_Top(x2, y2, x1, y1, x3, y3, color2, color1, color3);
		}
	}
	else//��Ҫ��һ��������ηֽ�Ϊƽ����ƽ��������
	{
		vec3f c1 = color1;
		vec3f c3 = color3;

		int ny = y2;
		float k = (y2 - y1) / float(y3 - y1);
		int nx = x1 + float(x3 - x1) * k;
		COLOR32 nc = c1.Lerp(c3, k);

		if (nx < x2)
		{
			_DrawTriangle_Solid_Buttom(x1, y1, nx, ny, x2, y2, color1, nc, color2);
			_DrawTriangle_Solid_Top(nx, ny, x2, y2, x3, y3, nc, color2, color3);
		}
		else
		{
			_DrawTriangle_Solid_Buttom(x1, y1, x2, y2, nx, ny, color1, color2, nc);
			_DrawTriangle_Solid_Top(x2, y2, nx, ny, x3, y3, color2, nc, color3);
		}
	}
}

void MemRenderBuffer::DrawTriangle_Solid_W(int x1, int y1, int x2, int y2, int x3, int y3,
	COLOR32 color1, COLOR32 color2, COLOR32 color3)
{
	//����һЩ��������
	if (y1 == y2 && y2 == y3)//ˮƽ��
	{
		int minx = min(min(x1, x2), x3);
		int maxx = max(max(x1, x2), x3);

		COLOR32 c1 = minx == x1 ? color1 : minx == x2 ? color2 : color3;
		COLOR32 c2 = maxx == x1 ? color1 : maxx == x2 ? color2 : color3;
		DrawLine_Bresenham(minx, y1, maxx, y1, c1, c2);
		return;
	}
	else if (x1 == x2 && x2 == x3)
	{
		int miny = min(min(y1, y2), y3);
		int maxy = max(max(y1, y2), y3);
		COLOR32 c1 = miny == y1 ? color1 : miny == y2 ? color2 : color3;
		COLOR32 c2 = maxy == y1 ? color1 : maxy == y2 ? color2 : color3;
		DrawLine_Bresenham(x1, miny, x1, maxy, c1, c2);
		return;
	}
	//���Ҫ��������б�ŵ�ֱ�ߵ�ʱ�򣬲鿴б���Ƿ���ȾͿ�����
	//����͵��ʡ����

	//���ǵ������η�Ϊһ�������κ�ƽ���������Լ�ƽ��������
	//����Ҫ��ȷ�������ε�����
	//Ϊ�˱�֤�жϵ��߼�һ�£����ǽ�����Ĳ�������һ������Yֵ�Ƚϵ���˳��
	//�������֤ y1 <= y2 <= y3
	if (y2 > y3)
	{
		int x = x2; x2 = x3; x3 = x;
		int y = y2; y2 = y3; y3 = y;
		COLOR32 c = color2; color2 = color3; color3 = c;
	}
	if (y1 > y2)
	{
		int x = x2; x2 = x1; x1 = x;
		int y = y2; y2 = y1; y1 = y;
		COLOR32 c = color2; color2 = color1; color1 = c;
	}
	if (y2 > y3)
	{
		int x = x2; x2 = x3; x3 = x;
		int y = y2; y2 = y3; y3 = y;
		COLOR32 c = color2; color2 = color3; color3 = c;
	}
	//�ж�����
	if (y2 == y3)//ƽ��������
	{
		if (x2 < x3)
		{
			_DrawTriangle_Solid_Buttom_W(x1, y1, x2, y2, x3, y3, color1, color2, color3);
		}
		else
		{
			_DrawTriangle_Solid_Buttom_W(x1, y1, x3, y3, x2, y2, color1, color3, color2);
		}
	}
	else if (y1 == y2)//ƽ��������
	{
		if (x1 < x2)
		{
			_DrawTriangle_Solid_Top_W(x1, y1, x2, y2, x3, y3, color1, color2, color3);
		}
		else
		{
			_DrawTriangle_Solid_Top_W(x2, y2, x1, y1, x3, y3, color2, color1, color3);
		}
	}
	else//��Ҫ��һ��������ηֽ�Ϊƽ����ƽ��������
	{
		vec3f c1 = color1;
		vec3f c3 = color3;

		int ny = y2;
		float k = (y2 - y1) / float(y3 - y1);
		int nx = x1 + float(x3 - x1) * k;
		COLOR32 nc = c1.Lerp(c3, k);

		if (nx < x2)
		{
			_DrawTriangle_Solid_Buttom_W(x1, y1, nx, ny, x2, y2, color1, nc, color2);
			_DrawTriangle_Solid_Top_W(nx, ny, x2, y2, x3, y3, nc, color2, color3);
		}
		else
		{
			_DrawTriangle_Solid_Buttom_W(x1, y1, x2, y2, nx, ny, color1, color2, nc);
			_DrawTriangle_Solid_Top_W(x2, y2, nx, ny, x3, y3, color2, nc, color3);
		}
	}
}
void MemRenderBuffer::DrawTriangle_Solid(
	int x1, int y1, float z1, 
	int x2, int y2, float z2,
	int x3, int y3, float z3, 
	vec2f uv1, vec2f uv2, vec2f uv3, 
	MyTexture2D* ptex)
{
	//����һЩ��������
	if (y1 == y2 && y2 == y3)//ˮƽ��ֱ��
	{
		int minx = min(min(x1, x2), x3);
		int maxx = max(max(x1, x2), x3);
		//DrawLine_Bresenham(minx, y1, maxx, y1, color);
		//COLOR32 c1 = minx == x1 ? color1 : minx == x2 ? color2 : color3;
		//COLOR32 c2 = maxx == x1 ? color1 : maxx == x2 ? color2 : color3;

		vec2f _uv1 = minx == x1 ? uv1 : minx == x2 ? uv2 : uv3;
		vec2f _uv2 = maxx == x1 ? uv1 : maxx == x2 ? uv2 : uv3;
		COLOR32 c1 = ptex->GetUVColor(_uv1);
		COLOR32 c2 = ptex->GetUVColor(_uv2);

		DrawLine_Bresenham(minx, y1, maxx, y1, c1, c2);
	}
	else if (x1 == x2 && x2 == x3)//��ֱ��ֱ��
	{
		int miny = min(min(y1, y2), y3);
		int maxy = max(max(y1, y2), y3);
		//DrawLine_Bresenham(x1, miny, x1, maxy, color);
		//COLOR32 c1 = miny == y1 ? color1 : miny == y2 ? color2 : color3;
		//COLOR32 c2 = maxy == y1 ? color1 : maxy == y2 ? color2 : color3;

		vec2f _uv1 = miny == y1 ? uv1 : miny == y2 ? uv2 : uv3;
		vec2f _uv2 = maxy == y1 ? uv1 : maxy == y2 ? uv2 : uv3;

		COLOR32 c1 = ptex->GetUVColor(_uv1);
		COLOR32 c2 = ptex->GetUVColor(_uv2);
		DrawLine_Bresenham(x1, miny, x1, maxy, c1, c2);
	}

	//�ڻ���������֮ǰ���ж������ε�����
	//���ͣ�һ�������Σ�ƽ�������Σ�ƽ��������
	//ƽ�������κ�ƽ�������ο���ֱ�ӻ��ƣ�һ����������Ҫ���Ϊ����������
	//Ϊ�˱�֤�жϵ�ʱ��ͳһ���ɣ�������Ĳ�������һ���������
	//��֤:y1 <= y2 <= y3
	if (y2 > y3)
	{
		int x = x2; x2 = x3; x3 = x;
		int y = y2; y2 = y3; y3 = y;
		float z = z2; z2 = z3; z3 = z;
		//COLOR32 c = color2; color2 = color3; color3 = c;
		vec2f uv = uv2; uv2 = uv3; uv3 = uv;
	}
	if (y1 > y2)
	{
		int x = x2; x2 = x1; x1 = x;
		int y = y2; y2 = y1; y1 = y;
		float z = z2; z2 = z1; z1 = z;
		//COLOR32 c = color2; color2 = color1; color1 = c;
		vec2f uv = uv2; uv2 = uv1; uv1 = uv;
	}
	if (y2 > y3)
	{
		int x = x2; x2 = x3; x3 = x;
		int y = y2; y2 = y3; y3 = y;
		float z = z2; z2 = z3; z3 = z;
		//COLOR32 c = color2; color2 = color3; color3 = c;
		vec2f uv = uv2; uv2 = uv3; uv3 = uv;
	}


	//�ж�����������
	if (y2 == y3)//һ����ƽ��������
	{
		if (x2 < x3)
		{
			_DrawTriangle_Solid_Bottom(x1, y1, z1, x2, y2, z2, x3, y3, z3, uv1, uv2, uv3, ptex);
		}
		else
		{
			_DrawTriangle_Solid_Bottom(x1, y1, z1, x3, y3, z3, x2, y2, z2, uv1, uv3, uv2, ptex);
		}
	}
	else if (y1 == y2)//һ����ƽ��������
	{
		if (x1 < x2)
		{
			_DrawTriangle_Solid_Top(x1, y1, z1, x2, y2, z2, x3, y3, z3, uv1, uv2, uv3, ptex);
		}
		else
		{
			_DrawTriangle_Solid_Top(x2, y2, z2, x1, y1, z1, x3, y3, z3, uv2, uv1, uv3, ptex);
		}
	}
	else//һ���������
	{
		//vec3f c1 = color1;
		//vec3f c3 = color3;
		//�����ֵ��x������
		float k = (y2 - y1) / float(y3 - y1);
		int nx = x1 + (x3 - x1) * k;
		float nz = z1 + (z3 - z1) * k;
		vec2f nuv = vec2f::Lerp(uv1, uv3, k);

		//��ֵ��ֵ����ɫ
		//COLOR32 nc = vec3f::Lerp(c1, c3, k);
		int ny = y2;
		if (nx < x2)
		{
			_DrawTriangle_Solid_Bottom(x1, y1, z1, nx, ny, nz, x2, y2, z2, uv1, nuv, uv2, ptex);
			_DrawTriangle_Solid_Top(nx, ny, nz, x2, y2, z2, x3, y3, z3, nuv, uv2, uv3, ptex);
		}
		else
		{
			_DrawTriangle_Solid_Bottom(x1, y1, z1, x2, y2, z2, nx, ny, nz, uv1, uv2, nuv, ptex);
			_DrawTriangle_Solid_Top(x2, y2, z2, nx, ny, nz, x3, y3, z3, uv2, nuv, uv3, ptex);
		}
	}


}
void MemRenderBuffer::DrawColorPlate(int cx, int cy, int r)
{
	//��ɫ��0�㣬��ɫ��120�㣬��ɫ��240�㣬Բ��Ϊ��ɫ
	//��ȷ����{x,y}�Ļ�ɫ����ɫʹ�����ڽǶ��ж��ڶ�Ӧ�������ɫ��ֵ
	//0~120����̲�ֵ��120��~240��������ֵ......
	//��ֵ���ջ����������У������������ڻ��ȱ��������ȱ������ڽǶȱ���������ֱ��ʹ�ýǶȼ���
	//�õ���ɫ�Ժ������ɫ��ֵ���õ�x,y��ȷ����ɫ

	float rs = r * r, dx, angle;
	float lx = cx, rx = cx + 0.5f;
	int ac;//������ɫ
	int yy = cy + r;
	vec3f cr = COLOR32(RGB(255, 0, 0)), cg = COLOR32(RGB(0, 255, 0)), cb = COLOR32(RGB(0, 0, 255));
	vec3f center = COLOR32(RGB(255, 255, 255));
	COLOR32 cc;
	vec2f pc(cx, cy);


	for (int y = cy - r; y < yy; ++y)   // ѭ����y ����Сֵ��y �����ֵ
	{
		dx = sqrtf(rs - (y - cy) * (y - cy));  // Բ�ı��� y��ľ���
		rx = cx + dx + 0.5f;
		lx = cx - dx;
		for (int x = lx; x < rx; ++x)  // x������
		{
			ac = _GetColorPlateAngle(cx, cy, r, x, y, angle);
			angle /= 120;//0~1.0f �ı���
			switch (ac)
			{
			case 0: {cc = cr.Lerp(cg, angle); }break;
			case 1: {cc = cg.Lerp(cb, angle); }break;
			case 2: {cc = cb.Lerp(cr, angle); }break;
			default: {cc = RGB(255, 255, 255); }
			}
			cc = center.Lerp(cc, (vec2f(x, y) - pc).magnitude() / r);
			DrawPixel(x, y, cc);
		}
	}

}

int MemRenderBuffer::_GetColorPlateAngle(const int& cx, const int& cy, const int& r, const int& x, const int& y, float& angle)
{
#define  COLOR_RANGE_RG 0
#define  COLOR_RANGE_GB 1
#define  COLOR_RANGE_BR 2
#define  COLOR_RANGE_CC 3
	if (cx == x && cy == y) { return COLOR_RANGE_CC; }//��Բ��
	vec2f p(x, y), c(cx, cy);
	vec2f v = p - c;
	angle = v.AngleDegree(vec2f::right());
	if (y < cy)
	{
		angle = 360 - angle;
	}
	if (angle <= 120)
	{
		return COLOR_RANGE_RG;
	}
	else if (angle <= 240)
	{
		angle -= 120;
		return COLOR_RANGE_GB;
	}
	else
	{
		angle -= 240;
		return COLOR_RANGE_BR;
	}
	return COLOR_RANGE_CC;
}

void MemRenderBuffer::_DrawTriangle_Solid_Buttom(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color)
{
	//��Ҫ�涨����ĵ�Ĳ�����x2<x3.y2==y3
	float height = y2 - y1;
	float dx_left = (x2 - x1) / height;//�������y����x�ı仯��
	float dx_right = (x3 - x1) / height;//�Ҳ�����y����x�ı仯��
	//����ÿһ�����ص������յ�
	float sx = x1;
	float ex = x1 + 0.5f;

	//�ж��������Ƿ����ӿڷ�Χ��
	//������������㶼�ڴ����ڣ���ô����ü�ȫ������
	if (mRect.PtInRect(x1, y1) && mRect.PtInRect(x2, y2) && mRect.PtInRect(x3, y3))
	{
		for (int y = y1; y <= y2; ++y)
		{
			for (int x = sx; x < ex; ++x)
			{
				memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
			}
			//ÿ����һ�У���Ҫ����ÿһ�е����յ�λ��
			sx += dx_left;
			ex += dx_right;
		}
	}
	else
	{
		//�������߽�ĵط����вü�
		//�ȴ������±߽�Ĳü����
		if (y1 < 0)
		{
			sx += dx_left * (0 - y1);
			ex += dx_right * (0 - y1);
			y1 = 0;
		}
		//�±߽�
		if (y2 > mRow - 1)
		{
			y2 = y3 = mRow - 1;
		}
		//���ж�̬���ж����ҵ����
		for (int y = y1; y <= y2; ++y)
		{
			//Ϊ������x�Ĳü����
			//�������յ��Ϊ���۵������յ�(����ԭ�б仯�����ɵ����յ�)�Լ�ʵ�ʻ��Ƶ����յ�(ͨ���ü��Ժ�õ������յ�)

			//һ��ʼ�����۵����յ��ʵ�ʵ����յ�
			int left = sx;
			int right = ex;

			//������ǰ�ļ�����һ�е�����ֵ
			sx += dx_left;
			ex += dx_right;

			//��ʵ��ֵ���вü��ж�
			if (left < 0)
			{
				left = 0;
				if (right < 0)
				{
					continue;
				}
			}
			if (left > mCol - 1)
			{
				continue;
			}
			if (right > mCol - 1)
			{
				right = mCol - 1;
			}

			//����ֱ��
			for (int x = left; x <= right; ++x)
			{
				memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
			}
		}
	}
}

void MemRenderBuffer::_DrawTriangle_Solid_Top(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color)
{
	//��Ҫ�涨����ĵ�Ĳ�����x1<x2 ,y1==y2
	float height = y3 - y1;
	float dx_left = (x3 - x1) / height;//�������y����x�ı仯��
	float dx_right = (x3 - x2) / height;//�Ҳ�����y����x�ı仯��
	//����ÿһ�����ص������յ�
	float sx = x1;
	float ex = x2 + 0.5f;

	//�ж��������Ƿ����ӿڷ�Χ��
	//������������㶼�ڴ����ڣ���ô����ü�ȫ������
	if (mRect.PtInRect(x1, y1) && mRect.PtInRect(x2, y2) && mRect.PtInRect(x3, y3))
	{
		for (int y = y1; y <= y3; ++y)
		{
			for (int x = sx; x < ex; ++x)
			{
				memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
			}
			//ÿ����һ�У���Ҫ����ÿһ�е����յ�λ��
			sx += dx_left;
			ex += dx_right;
		}
	}
	else
	{
		//�������߽�ĵط����вü�
		//�ȴ������±߽�Ĳü����
		if (y1 < 0)
		{
			sx += dx_left * (0 - y1);
			ex += dx_right * (0 - y2);
			y1 = y2 = 0;
		}
		//�±߽�
		if (y3 > mRow - 1)
		{
			y3 = mRow - 1;
		}
		//���ж�̬���ж����ҵ����
		for (int y = y1; y <= y3; ++y)
		{
			//Ϊ������x�Ĳü����
			//�������յ��Ϊ���۵������յ�(����ԭ�б仯�����ɵ����յ�)�Լ�ʵ�ʻ��Ƶ����յ�(ͨ���ü��Ժ�õ������յ�)

			//һ��ʼ�����۵����յ��ʵ�ʵ����յ�
			int left = sx;
			int right = ex;

			//������ǰ�ļ�����һ�е�����ֵ
			sx += dx_left;
			ex += dx_right;

			//��ʵ��ֵ���вü��ж�
			if (left < 0)
			{
				left = 0;
				if (right < 0)
				{
					continue;
				}
			}
			if (left > mCol - 1)
			{
				continue;
			}
			if (right > mCol - 1)
			{
				right = mCol - 1;
			}

			//����ֱ��
			for (int x = left; x <= right; ++x)
			{
				memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
			}
		}
	}
}

void MemRenderBuffer::_DrawTriangle_Solid_Buttom(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color1, const COLOR32& color2, const COLOR32& color3)
{
	vec3f c1 = color1, c2 = color2, c3 = color3;
	//��Ҫ�涨����ĵ�Ĳ�����x2<x3.y2==y3
	float height = y2 - y1;
	float dx_left = (x2 - x1) / height;//�������y����x�ı仯��
	float dx_right = (x3 - x1) / height;//�Ҳ�����y����x�ı仯��
	vec3f dc_left = (c2 - c1) / height;
	vec3f dc_right = (c3 - c1) / height;
	//����ÿһ�����ص������յ�
	float sx = x1;
	float ex = x1 + 0.5f;
	vec3f sc = c1;
	vec3f ec = c1;

	vec3f dch;
	vec3f cc;
	COLOR32 color;

	//�ж��������Ƿ����ӿڷ�Χ��
	//������������㶼�ڴ����ڣ���ô����ü�ȫ������
	if (mRect.PtInRect(x1, y1) && mRect.PtInRect(x2, y2) && mRect.PtInRect(x3, y3))
	{
		for (int y = y1; y <= y2; ++y)
		{
			dch = (ec - sc) / (ex - sx);  // 
			cc = sc;
			for (int x = sx; x < ex; ++x)
			{
				color = cc;
				memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
				cc += dch;
			}
			//ÿ����һ�У���Ҫ����ÿһ�е����յ�λ��
			sx += dx_left;
			ex += dx_right;
			sc += dc_left;
			ec += dc_right;
		}
	}
	else
	{
		//�������߽�ĵط����вü�
		//�ȴ������±߽�Ĳü����
		if (y1 < 0)
		{
			sx += dx_left * (0 - y1);
			ex += dx_right * (0 - y1);
			sc += dc_left * (0 - y1);
			ec += dc_right * (0 - y1);
			y1 = 0;
		}
		//�±߽�
		if (y2 > mRow - 1)
		{
			y2 = y3 = mRow - 1;
		}
		//���ж�̬���ж����ҵ����
		for (int y = y1; y <= y2; ++y)
		{
			//Ϊ������x�Ĳü����
			//�������յ��Ϊ���۵������յ�(����ԭ�б仯�����ɵ����յ�)�Լ�ʵ�ʻ��Ƶ����յ�(ͨ���ü��Ժ�õ������յ�)

			//һ��ʼ�����۵����յ��ʵ�ʵ����յ�
			int left = sx;
			int right = ex;
			vec3f lc = sc;
			vec3f rc = ec;

			float len = ex - sx;
			dch = (ec - sc) / len;
			int oldx = sx;
			//������ǰ�ļ�����һ�е�����ֵ
			sx += dx_left;
			ex += dx_right;
			sc += dc_left;
			ec += dc_right;

			//��ʵ��ֵ���вü��ж�
			if (left < 0)
			{
				left = 0;
				lc += dch * (0 - oldx);
				if (right < 0)
				{
					continue;
				}
			}
			if (left > mCol - 1)
			{
				continue;
			}
			if (right > mCol - 1)
			{
				right = mCol - 1;
			}
			cc = lc;
			//����ֱ��
			for (int x = left; x <= right; ++x)
			{
				color = cc;
				memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
				cc += dch;
			}
		}
	}
}

void MemRenderBuffer::_DrawTriangle_Solid_Top(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color1, const COLOR32& color2, const COLOR32& color3)
{
	vec3f c1 = color1, c2 = color2, c3 = color3;
	//��Ҫ�涨����ĵ�Ĳ�����x1<x2 ,y1==y2
	float height = y3 - y1;
	float dx_left = (x3 - x1) / height;//�������y����x�ı仯��
	float dx_right = (x3 - x2) / height;//�Ҳ�����y����x�ı仯��
	vec3f dc_left = (c3 - c1) / height;
	vec3f dc_right = (c3 - c2) / height;
	//����ÿһ�����ص������յ�
	float sx = x1;
	float ex = x2 + 0.5f;

	vec3f sc = c1;
	vec3f ec = c2;

	vec3f dch;
	vec3f cc;
	COLOR32 color;

	//�ж��������Ƿ����ӿڷ�Χ��
	//������������㶼�ڴ����ڣ���ô����ü�ȫ������
	if (mRect.PtInRect(x1, y1) && mRect.PtInRect(x2, y2) && mRect.PtInRect(x3, y3))
	{
		for (int y = y1; y <= y3; ++y)
		{
			dch = (ec - sc) / (ex - sx);
			cc = sc;
			for (int x = sx; x < ex; ++x)
			{
				color = cc;
				memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
				cc += dch;
			}
			//ÿ����һ�У���Ҫ����ÿһ�е����յ�λ��
			sx += dx_left;
			ex += dx_right;
			sc += dc_left;
			ec += dc_right;
		}
	}
	else
	{
		//�������߽�ĵط����вü�
		//�ȴ������±߽�Ĳü����
		if (y1 < 0)
		{
			sx += dx_left * (0 - y1);
			ex += dx_right * (0 - y2);
			sc += dc_left * (0 - y1);
			ec += dc_right * (0 - y2);
			y1 = y2 = 0;
		}
		//�±߽�
		if (y3 > mRow - 1)
		{
			y3 = mRow - 1;
		}
		//���ж�̬���ж����ҵ����
		for (int y = y1; y <= y3; ++y)
		{
			//Ϊ������x�Ĳü����
			//�������յ��Ϊ���۵������յ�(����ԭ�б仯�����ɵ����յ�)�Լ�ʵ�ʻ��Ƶ����յ�(ͨ���ü��Ժ�õ������յ�)

			//һ��ʼ�����۵����յ��ʵ�ʵ����յ�
			int left = sx;
			int right = ex;
			vec3f lc = sc;
			vec3f rc = ec;
			float len = ex - sx;
			dch = (ec - sc) / len;
			int oldx = sx;
			//������ǰ�ļ�����һ�е�����ֵ
			sx += dx_left;
			ex += dx_right;
			sc += dc_left;
			ec += dc_right;

			//��ʵ��ֵ���вü��ж�
			if (left < 0)
			{
				left = 0;
				lc += dch * (0 - oldx);
				if (right < 0)
				{
					continue;
				}
			}
			if (left > mCol - 1)
			{
				continue;
			}
			if (right > mCol - 1)
			{
				right = mCol - 1;
			}
			cc = lc;
			//����ֱ��
			for (int x = left; x <= right; ++x)
			{
				color = cc;
				memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
				cc += dch;
			}
		}
	}
}

void MemRenderBuffer::_DrawTriangle_Solid_Buttom_W(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color1, const COLOR32& color2, const COLOR32& color3)
{
	//��Ҫ�������x2һ��С��x3��
	float height = y2 - y1;//�õ����Ǹ߶ȣ����仰˵�������ص�����
	float dx_left = (x2 - x1) / height;//�õ������������y����λ����x�ı仯��
	float dx_right = (x3 - x1) / height;//�õ������ұ�����y����λ����x�ı仯��
	//����ÿһ�����ػ��Ƶ������յ�
	float sx = x1;
	float ex = x1 + 0.5f;

	float w1, w2, w3;
	vec3f c1 = color1, c2 = color2, c3 = color3;
	COLOR32 c;

	//�ж��������Ƿ����ӿڷ�Χ��
	//����ڷ�Χ�ڣ�ֱ�ӻ��ƣ�������ڷ�Χ�ڣ��жϲü�������߲�����
	if (mRect.PtInRect(x1, y1) && mRect.PtInRect(x2, y2) && mRect.PtInRect(x3, y3))
	{
		//�����ӿڷ�Χ�ڣ�ֱ�ӻ���
		for (int y = y1; y <= y2; ++y)
		{
			for (int x = sx; x < ex; ++x)
			{
				_TriangleWeight(x1, y1, x2, y2, x3, y3, x, y, w1, w2, w3);
				c = c1 * w1 + c2 * w2 + c3 * w3;
				memcpy(&mBufferArray[y][x], &c, sizeof(COLOR32));
			}
			//ÿ����һ��,������һ�е���ʼ�ͽ���λ��
			sx += dx_left;
			ex += dx_right;
		}
	}
	else
	{
		//�������߽�ĵط����вü�
		//�������±߽�ü����
		if (y1 < 0)//�ϱ߽�Խ��
		{
			sx += dx_left * (0 - y1);
			ex += dx_right * (0 - y1);
			y1 = 0;
		}
		//�±߽�Խ��
		if (y2 > mRow - 1)
		{
			y2 = y3 = mRow - 1;
		}

		for (int y = y1; y <= y2; ++y)
		{
			//����Խ��������������ÿһ�н��вü�
			//Ϊ������x����Ĳü�
			//�����Ƶ�һ�е����ص������յ㻮��Ϊ
			//1.�����ϵ����յ� sx,ex ��ʼ�հ��������ε�x�仯��,���б仯��
			//2.ʵ�ʵ����յ� left,right;

			int left = sx;
			int right = ex;

			//������ǰ������һ�е��������յ�
			sx += dx_left;
			ex += dx_right;

			if (left < 0)
			{
				left = 0;
				if (right < 0)
				{
					continue;//˵����һ�в���Ҫ������
				}
			}
			if (left > mCol - 1)
			{
				continue;
			}
			if (right > mCol - 1)
			{
				right = mCol - 1;
			}

			//���Ʊ��е�ֱ��
			for (int x = left; x <= right; ++x)
			{
				_TriangleWeight(x1, y1, x2, y2, x3, y3, x, y, w1, w2, w3);
				c = c1 * w1 + c2 * w2 + c3 * w3;
				memcpy(&mBufferArray[y][x], &c, sizeof(COLOR32));
			}
		}
	}
}

void MemRenderBuffer::_DrawTriangle_Solid_Top_W(int x1, int y1, int x2, int y2, int x3, int y3, const COLOR32& color1, const COLOR32& color2, const COLOR32& color3)
{
	vec3f c1 = color1, c2 = color2, c3 = color3;
	float w1, w2, w3;
	//��Ҫ�涨����ĵ�Ĳ�����x1<x2 ,y1==y2
	float height = y3 - y1;
	float dx_left = (x3 - x1) / height;//�������y����x�ı仯��
	float dx_right = (x3 - x2) / height;//�Ҳ�����y����x�ı仯��
	//����ÿһ�����ص������յ�
	float sx = x1;
	float ex = x2 + 0.5f;
	COLOR32 c;

	//�ж��������Ƿ����ӿڷ�Χ��
	//������������㶼�ڴ����ڣ���ô����ü�ȫ������
	if (mRect.PtInRect(x1, y1) && mRect.PtInRect(x2, y2) && mRect.PtInRect(x3, y3))
	{
		//�����ӿڷ�Χ�ڣ�ֱ�ӻ���
		for (int y = y1; y <= y3; ++y)
		{
			for (int x = sx; x < ex; ++x)
			{
				_TriangleWeight(x1, y1, x2, y2, x3, y3, x, y, w1, w2, w3);
				c = c1 * w1 + c2 * w2 + c3 * w3;
				memcpy(&mBufferArray[y][x], &c, sizeof(COLOR32));
			}
			//ÿ����һ��,������һ�е���ʼ�ͽ���λ��
			sx += dx_left;
			ex += dx_right;
		}
	}
	else
	{
		//�������߽�ĵط����вü�
		//�������±߽�ü����
		if (y1 < 0)//�ϱ߽�Խ��
		{
			sx += dx_left * (0 - y1);
			ex += dx_right * (0 - y2);
			y1 = y2 = 0;
		}
		//�±߽�Խ��
		if (y3 > mRow - 1)
		{
			y3 = mRow - 1;
		}

		for (int y = y1; y <= y3; ++y)
		{
			//����Խ��������������ÿһ�н��вü�
			//Ϊ������x����Ĳü�
			//�����Ƶ�һ�е����ص������յ㻮��Ϊ
			//1.�����ϵ����յ� sx,ex ��ʼ�հ��������ε�x�仯��,���б仯��
			//2.ʵ�ʵ����յ� left,right;

			int left = sx;
			int right = ex;

			//������ǰ������һ�е��������յ�
			sx += dx_left;
			ex += dx_right;

			if (left < 0)
			{
				left = 0;
				if (right < 0)
				{
					continue;//˵����һ�в���Ҫ������
				}
			}
			if (left > mCol - 1)
			{
				continue;
			}
			if (right > mCol - 1)
			{
				right = mCol - 1;
			}

			//���Ʊ��е�ֱ��
			for (int x = left; x <= right; ++x)
			{
				_TriangleWeight(x1, y1, x2, y2, x3, y3, x, y, w1, w2, w3);
				c = c1 * w1 + c2 * w2 + c3 * w3;
				memcpy(&mBufferArray[y][x], &c, sizeof(COLOR32));
			}
		}
	}
}

void MemRenderBuffer::_DrawTriangle_Solid_Top(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, vec2f uv1, vec2f uv2, vec2f uv3, MyTexture2D* ptex)
{
	//vec3f c1 = color1, c2 = color2, c3 = color3;
	//��Ҫ�������x1һ��С��x2��
	float height = y3 - y1;//�õ����Ǹ߶ȣ����仰˵�������ص�����
	float dx_left = (x3 - x1) / height;//�õ������������y����λ����x�ı仯��
	float dx_right = (x3 - x2) / height;//�õ������ұ�����y����λ����x�ı仯��
	//vec3f dc_left = (c3 - c1) / height;//�õ������������y����λ������ɫ�ı仯��
	//vec3f dc_right = (c3 - c2) / height;//�õ������ұ�����y����λ������ɫ�ı仯
	float dz_left = (z3 - z1) / height;
	float dz_right = (z3 - z2) / height;
	vec2f duv_left = (uv3 - uv1) / height;
	vec2f duv_right = (uv3 - uv2) / height;

	//����ÿһ�����ػ��Ƶ������յ�
	float sx = x1;
	float ex = x2 + 0.5f;
	//ÿһ��������ɫ�������յ�
	//vec3f sc = c1;
	//vec3f ec = c2;
	vec2f suv = uv1;
	vec2f euv = uv2;

	float sz = z1;
	float ez = z2;

	//ÿһ�д�sc��ec�ĵ�λ���ض�Ӧ�ı仯��
	//vec3f dch = (ec - sc)/(ex - sx);
	//vec3f dch;
	//vec3f cc;//��ǰ����ɫ
	vec2f duv_h;
	vec2f cuv;
	float dzh;
	float cz;
	COLOR32 color;


	//�ж��������Ƿ����ӿڷ�Χ��
	//����ڷ�Χ�ڣ�ֱ�ӻ��ƣ�������ڷ�Χ�ڣ��жϲü�������߲�����
	if (mRect.PtInRect(x1, y1) && mRect.PtInRect(x2, y2) && mRect.PtInRect(x3, y3))
	{
		//�����ӿڷ�Χ�ڣ�ֱ�ӻ���
		for (int y = y1; y <= y3; ++y)
		{
			//dch = (ec - sc) / (ex - sx);
			duv_h = (euv - suv) / (ex - sx);

			dzh = (ez - sz) / (ex - sx);
			//cc = sc;
			cuv = suv;
			cz = sz;

			for (int x = sx; x < ex; ++x)
			{
				//�ڻ�����ɫ֮ǰ��Ҫ�Ƚ��Ƿ������ɫ���ݵ�д��
				//�����ǰzֵ���Ѿ��ڻ�������zֵ����ô���Խ�����ɫ��д��
				if (cz > mZBuffer[y * mCol + x])
				{
					//color = cc;
					color = ptex->GetUVColor(cuv / cz);
					memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
					mZBuffer[y * mCol + x] = cz;//ÿ�θ�����ɫ��ʱ��Ҫ�����ֵһ�����
				}
				//cc += dch;
				cuv += duv_h;
				cz += dzh;
			}
			//ÿ����һ��,������һ�е���ʼ�ͽ���λ��
			sx += dx_left;
			ex += dx_right;
			//sc += dc_left;
			//ec += dc_right;
			suv += duv_left;
			euv += duv_right;
			sz += dz_left;
			ez += dz_right;
		}
	}
	else
	{
		//�������߽�ĵط����вü�
		//�������±߽�ü����
		if (y1 < 0)//�ϱ߽�Խ��
		{
			sx += dx_left * (0 - y1);
			ex += dx_right * (0 - y2);
			//sc += dc_left * (0 - y1);
			//ec += dc_right * (0 - y2);
			suv += duv_left * (0 - y1);
			euv += duv_right * (0 - y2);
			sz += dz_left * (0 - y1);
			ez += dz_right * (0 - y2);
			y1 = y2 = 0;
		}
		//�±߽�Խ��
		if (y3 > mRow - 1)
		{
			y3 = mRow - 1;
		}

		for (int y = y1; y <= y3; ++y)
		{
			//����Խ��������������ÿһ�н��вü�
			//Ϊ������x����Ĳü�
			//�����Ƶ�һ�е����ص������յ㻮��Ϊ
			//1.�����ϵ����յ� sx,ex ��ʼ�հ��������ε�x�仯��,���б仯��
			//2.ʵ�ʵ����յ� left,right;

			int left = sx;
			int right = ex;
			//vec3f lc = sc;
			//vec3f rc = ec;
			vec2f luv = suv;
			vec2f ruv = euv;
			float zleft = sz;
			float len = ex - sx;
			//dch = (ec - sc) / len;
			duv_h = (euv - suv) / len;
			dzh = (ez - sz) / len;

			int oldx = sx;
			//������ǰ������һ�е��������յ�
			sx += dx_left;
			ex += dx_right;
			//sc += dc_left;
			//ec += dc_right;
			suv += duv_left;
			euv += duv_right;
			sz += dz_left;
			ez += dz_right;


			if (left < 0)
			{
				left = 0;
				//lc += dch * (0 - oldx);
				luv += duv_h * (0 - oldx);
				zleft += dzh * (0 - oldx);
				if (right < 0)
				{
					continue;//˵����һ�в���Ҫ������
				}
			}
			if (left > mCol - 1)
			{
				continue;
			}
			if (right > mCol - 1)
			{
				right = mCol - 1;
			}
			//cc = lc;
			cuv = luv;
			cz = zleft;
			//���Ʊ��е�ֱ��
			for (int x = left; x <= right; ++x)
			{
				if (cz > mZBuffer[y * mCol + x])
				{
					//color = cc;
					color = ptex->GetUVColor(cuv / cz);
					memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
					mZBuffer[y * mCol + x] = cz;
				}

				//cc += dch;
				cuv += duv_h;
				cz += dzh;
			}
		}
	}
}

void MemRenderBuffer::_DrawTriangle_Solid_Bottom(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, vec2f uv1, vec2f uv2, vec2f uv3, MyTexture2D* ptex)
{
	//vec3f c1 = color1, c2 = color2, c3 = color3;
	//��Ҫ�������x2һ��С��x3��
	float height = y2 - y1;//�õ����Ǹ߶ȣ����仰˵�������ص�����
	float dx_left = (x2 - x1) / height;//�õ������������y����λ����x�ı仯��
	float dx_right = (x3 - x1) / height;//�õ������ұ�����y����λ����x�ı仯��
	//vec3f dc_left = (c2 - c1) / height;
	//vec3f dc_right = (c3 - c1) / height;
	vec2f duv_left = (uv2 - uv1) / height;
	vec2f duv_right = (uv3 - uv1) / height;

	float dz_left = (z2 - z1) / height;
	float dz_right = (z3 - z1) / height;
	//����ÿһ�����ػ��Ƶ������յ�
	float sx = x1;
	float ex = x1 + 0.5f;
	//vec3f sc = c1;
	//vec3f ec = c1;
	vec2f suv = uv1;
	vec2f euv = uv1;
	float sz = z1;
	float ez = z1;
	vec3f dch;
	float dzh;
	float cz;
	vec2f duvh;
	//vec3f cc;
	vec2f cuv;
	COLOR32 color;

	//�ж��������Ƿ����ӿڷ�Χ��
	//����ڷ�Χ�ڣ�ֱ�ӻ��ƣ�������ڷ�Χ�ڣ��жϲü�������߲�����
	if (mRect.PtInRect(x1, y1) && mRect.PtInRect(x2, y2) && mRect.PtInRect(x3, y3))
	{
		//�����ӿڷ�Χ�ڣ�ֱ�ӻ���
		for (int y = y1; y <= y2; ++y)
		{
			//dch = (ec - sc) / (ex - sx);
			duvh = (euv - suv) / (ex - sx);
			dzh = (ez - sz) / (ex - sx);
			//cc = sc;
			cuv = suv;
			cz = sz;

			for (int x = sx; x < ex; ++x)
			{
				if (cz > mZBuffer[y * mCol + x])
				{
					//color = cc;
					color = ptex->GetUVColor(cuv / cz);
					memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
					mZBuffer[y * mCol + x] = cz;
				}
				//cc += dch;
				cuv += duvh;
				cz += dzh;
			}
			//ÿ����һ��,������һ�е���ʼ�ͽ���λ��
			sx += dx_left;
			ex += dx_right;
			//sc += dc_left;
			//ec += dc_right;
			suv += duv_left;
			euv += duv_right;
			sz += dz_left;
			ez += dz_right;
		}
	}
	else
	{
		//�������߽�ĵط����вü�
		//�������±߽�ü����
		if (y1 < 0)//�ϱ߽�Խ��
		{
			sx += dx_left * (0 - y1);
			ex += dx_right * (0 - y1);
			//sc += dc_left * (0 - y1);
			//ec += dc_right * (0 - y1);
			suv += duv_left * (0 - y1);
			euv += duv_right * (0 - y1);
			sz += dz_left * (0 - y1);
			ez += dz_right * (0 - y1);
			y1 = 0;
		}
		//�±߽�Խ��
		if (y2 > mRow - 1)
		{
			y2 = y3 = mRow - 1;
		}

		for (int y = y1; y <= y2; ++y)
		{
			//����Խ��������������ÿһ�н��вü�
			//Ϊ������x����Ĳü�
			//�����Ƶ�һ�е����ص������յ㻮��Ϊ
			//1.�����ϵ����յ� sx,ex ��ʼ�հ��������ε�x�仯��,���б仯��
			//2.ʵ�ʵ����յ� left,right;

			int left = sx;
			int right = ex;
			float zleft = sz;
			//vec3f lc = sc;
			//vec3f rc = ec;
			vec2f luv = suv;
			vec2f ruv = euv;
			float len = ex - sx;
			//dch = (ec - sc) / len;
			duvh = (euv - suv) / len;
			dzh = (ez - sz) / len;
			int oldx = sx;
			//������ǰ������һ�е��������յ�
			sx += dx_left;
			ex += dx_right;
			//sc += dc_left;
			//ec += dc_right;
			suv += duv_left;
			euv += duv_right;
			sz += dz_left;
			ez += dz_right;

			if (left < 0)
			{
				left = 0;
				zleft += dzh * (0 - oldx);
				//lc += dch * (0 - oldx);
				luv += duvh * (0 - oldx);
				if (right < 0)
				{
					continue;//˵����һ�в���Ҫ������
				}
			}
			if (left > mCol - 1)
			{
				continue;
			}
			if (right > mCol - 1)
			{
				right = mCol - 1;
			}
			//cc = lc;
			cuv = luv;
			cz = zleft;
			//���Ʊ��е�ֱ��
			for (int x = left; x <= right; ++x)
			{
				if (cz > mZBuffer[y * mCol + x])
				{
					//color = cc;
					color = ptex->GetUVColor(cuv / cz);
					memcpy(&mBufferArray[y][x], &color, sizeof(COLOR32));
					mZBuffer[y * mCol + x] = cz;
				}
				//cc += dch;
				cuv += duvh;
				cz += dzh;
			}
		}
	}
}

void MemRenderBuffer::_DrawCirclePoint8(int x, int y, int p, int q, const COLOR32& color)
{
	DrawPixel(x + p, y + q, color);
	DrawPixel(x - p, y + q, color);
	DrawPixel(x + p, y - q, color);
	DrawPixel(x - p, y - q, color);

	DrawPixel(x + q, y + p, color);
	DrawPixel(x - q, y + p, color);
	DrawPixel(x + q, y - p, color);
	DrawPixel(x - q, y - p, color);
}

bool MemRenderBuffer::_ClipLine_mp(int& x1, int& y1, int& x2, int& y2)
{
	float px1 = x1, py1 = y1, px2 = x2, py2 = y2;
	bool b1 = __ClipLineMP(px1, py1, px2, py2);
	if (b1)
	{
		x1 = px1;
		y1 = py1;
	}
	bool b2 = __ClipLineMP(px2, py2, px1, py1);
	if (b2)
	{
		x2 = px2;
		y2 = py2;
	}
	return b1 && b2;
}

//�����е�ü��㷨�еķ�λ����
#define  CLIP_CODE_CENTER	0x0000
#define  CLIP_CODE_LEFT		0x0001
#define  CLIP_CODE_RIGHT	0x0002
#define  CLIP_CODE_TOP		0x0004
#define  CLIP_CODE_BOTTOM	0x0008
 
int MemRenderBuffer::__EnCode(float x, float y)
{
	int c = CLIP_CODE_CENTER;
	if (x < 0) c |= CLIP_CODE_LEFT;
	if (y < 0) c |= CLIP_CODE_TOP;
	if (x > mCol - 1) c |= CLIP_CODE_RIGHT;
	if (y > mRow -1) c |= CLIP_CODE_BOTTOM;
	return c;
}

int MemRenderBuffer::__BorderCode(float x, float y)
{
	int c = CLIP_CODE_CENTER;
	if (abs(x) < 1) c |= CLIP_CODE_LEFT;
	if (abs(y) < 1) c |= CLIP_CODE_TOP;
	if (abs(mCol - 1 - x) < 1) c |= CLIP_CODE_RIGHT;
	if (abs(mRow - 1 - y) < 1) c |= CLIP_CODE_BOTTOM;
	return c;
}

bool MemRenderBuffer::__NotFinish(float x, float y)
{
	if ((((abs(x) < 1 || abs(mCol - 1 - x) < 1) && (y >= 0 && y <= mRow - 1) ||
		(abs(y) < 1 || abs(mRow - 1 - y) < 1) && (x >= 0 && x <= mCol - 1))))
	{
		return false;
	}
	return true;
}

bool MemRenderBuffer::__ClipLineMP(float& px1, float& py1, float px2, float py2)
{
	float mx, my;  // �м��
	int code1, code2, codem;  // ��� �յ� �е�ķ�λ
	code1 = __EnCode(px1, py1);
	code2 = __EnCode(px2, py2);

	mx = (px1 + px2) / 2;
	my = (py1 + py2) / 2;
	codem = __EnCode(mx, my);

	if (code1 == CLIP_CODE_CENTER) // ����ڴ����� ���òü�
	{
		return true;
	}
	else if (code1 & code2)  // ������յ㶼�� ������ ����ͬһ����λ
	{
		return false; // ������
	}
	else if (code1 & __BorderCode(mx, my)) // �м���ڴ��ڵı߽�  (�Ѿ��жϹ��� ��㲻�ڴ����� ������յ�Ҳ���ٴ������ͬһ��) �����е�ķ�λ�����һ��
	{
		// ����м��������Ҫ�ü����ĵ�
		px1 = mx;
		py1 = my;
		return true;
	}
	else  // ����յ㶼��ͬ�ķ�λ  ���������
	{
		do
		{
			// ����������м�㲻��Ҫ�ҵ����е�
			mx = (px1 + px2) / 2;
			my = (py1 + py2) / 2;
			codem = __EnCode(mx, my);

			// �ж��м���λ�� �������յ�
			if (codem == CLIP_CODE_CENTER)  // �е����ڲ� �����յ�
			{
				px2 = mx;
				py2 = my;
				code2 = codem;
			}
			else  // �е��ڴ�����
			{
				if (codem & code1)  // �е�����ʼ����ͬһ����λ
				{
					px1 = mx;
					py1 = my;
					code1 = codem;
				}
				else  // �е��� ���յ���ͬһ����λ
				{
					px2 = mx;
					py2 = my;
					code2 = codem; 
				}
			}

			if (code1 & code2)  // ���������յ�֮�� ������յ���ͬһ����λ ���� ���յ� �������� ���Ǹտ�ʼ����ͬһ����λ
			{
				return false;  // ������
			}

		} while (__NotFinish(px1,py1));  // �ж����Ĳü��Ƿ��Ѿ���� û����ɻ���������е�

	}
	return true;
}

#define CLIP_CODE_C		0x0000
#define CLIP_CODE_N		0x0008
#define CLIP_CODE_S		0x0004
#define CLIP_CODE_W		0x0002
#define CLIP_CODE_E		0x0001
#define CLIP_CODE_WN	0x000a
#define CLIP_CODE_WS	0x0006
#define CLIP_CODE_EN	0x0009
#define CLIP_CODE_ES	0x0005
bool MemRenderBuffer::_ClipLine_csl(int& x1, int& y1, int& x2, int& y2)
{
	int xc1 = x1, yc1 = y1, xc2 = x2, yc2 = y2;
	// ���յ��ʼ��Ϊ�м�
	int p1_code = CLIP_CODE_C, p2_code = CLIP_CODE_C;

	if (y1 < 0) { p1_code |= CLIP_CODE_N; }
	else if (y1 >= mRow) { p1_code |= CLIP_CODE_S; }
	if (x1 < 0) { p1_code |= CLIP_CODE_W; }
	else if (x1 >= mCol) { p1_code |= CLIP_CODE_E; }

	if (y2 < 0) { p2_code |= CLIP_CODE_N; }
	else if (y2 >= mRow) { p2_code |= CLIP_CODE_S; }
	if (x2 < 0) { p2_code |= CLIP_CODE_W; }
	else if (x2 >= mCol) { p2_code |= CLIP_CODE_E; }

	// �������
	if (p1_code & p2_code) // ����ͬ��
	{
		return false;
	}

	if (p1_code == p2_code && p1_code == CLIP_CODE_C) // ����
	{
		return true;
	}

	// �ü�p1
	switch (p1_code)
	{
	case CLIP_CODE_C:break;  // ����
	case CLIP_CODE_N:
	{
		yc1 = 0;
		xc1 = x1 + (0 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
	}break;
	case CLIP_CODE_S:
	{
		yc1 = mRow - 1;
		xc1 = x1 + (mRow - 1 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;

	}break;
	case CLIP_CODE_W:
	{
		xc1 = 0;
		yc1 = y1 + (0 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
	}break;
	case CLIP_CODE_E:
	{
		xc1 = mCol - 1;
		yc1 = y1 + (mCol - 1 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
	}break;
	case CLIP_CODE_EN:
	{
		xc1 = mCol - 1;
		yc1 = y1 + (mCol - 1 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
		if (yc1 < 0 || yc1 >= mRow)
		{
			yc1 = 0;
			xc1 = x1 + (0 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
		}
	}break;
	case CLIP_CODE_WN:
	{
		xc1 = 0;
		yc1 = y1 + (0 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
		if (yc1 < 0 || yc1 >= mRow)
		{
			yc1 = 0;
			xc1 = x1 + (0 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
		}
	}break;
	case CLIP_CODE_ES:
	{
		xc1 = mCol - 1;
		yc1 = y1 + (mCol - 1 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
		if (yc1 < 0 || yc1 >= mRow)
		{
			yc1 = mRow - 1;
			xc1 = x1 + (mRow - 1 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
		}
	}break;
	case CLIP_CODE_WS:
	{
		xc1 = 0;
		yc1 = y1 + (0 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
		if (yc1 < 0 || yc1 >= mRow)
		{
			yc1 = mRow - 1;
			xc1 = x1 + (mRow - 1 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
		}
	}break;
	}

	// �ü�p2
	switch (p2_code)
	{
	case CLIP_CODE_C:break;
	case CLIP_CODE_N:
	{
		yc2 = 0;
		xc2 = x1 + (0 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
	}break;
	case CLIP_CODE_S:
	{
		yc2 = mRow - 1;
		xc2 = x1 + (mRow - 1 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
	}break;
	case CLIP_CODE_W:
	{
		xc2 = 0;
		yc2 = y1 + (0 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
	}break;
	case CLIP_CODE_E:
	{
		xc2 = mCol - 1;
		yc2 = y1 + (mCol - 1 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
	}break;
	case CLIP_CODE_EN:
	{
		xc2 = mCol - 1;
		yc2 = y1 + (mCol - 1 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
		if (yc2 < 0 || yc2 >= mRow)
		{
			yc2 = 0;
			xc2 = x1 + (0 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
		}
	}break;
	case CLIP_CODE_WN:
	{
		xc2 = 0;
		yc2 = y1 + (0 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
		if (yc2 < 0 || yc2 >= mRow)
		{
			yc2 = 0;
			xc2 = x1 + (0 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
		}
	}break;
	case CLIP_CODE_ES:
	{
		xc2 = mCol - 1;
		yc2 = y1 + (mCol - 1 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
		if (yc2 < 0 || yc2 >= mRow)
		{
			yc2 = mRow - 1;
			xc2 = x1 + (mRow - 1 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
		}
	}break;
	case CLIP_CODE_WS:
	{
		xc2 = 0;
		yc2 = y1 + (0 - x1) * (y2 - y1) / float(x2 - x1) + 0.5f;
		if (yc2 < 0 || yc2 >= mRow)
		{
			yc2 = mRow - 1;
			xc2 = x1 + (mRow - 1 - y1) * (x2 - x1) / float(y2 - y1) + 0.5f;
		}
	}break;
	}

	// �����߶ζ����ڴ�����
	if (xc1 < 0 || xc1 >= mCol || yc1 < 0 || yc1 >= mRow ||
		xc2 < 0 || xc2 >= mCol || yc2 < 0 || yc2 >= mRow)
	{
		return false;
	}

	x1 = xc1;
	x2 = xc2;
	y1 = yc1;
	y2 = yc2;

	return true;
}

void MemRenderBuffer::__BezierCurve2oft(vec2f& pt, const vec2f* pc, float t)
{
	if (pc == nullptr) { return;}
	float tm1 = 1 - t;
	vec2f p01 = pc[0] * tm1 + pc[1] * t;
	vec2f p12 = pc[1] * tm1 + pc[2] * t;
	pt = p01 * tm1 + p12 * t;
}

void MemRenderBuffer::__BezierCurve3oft(vec2f& pt, const vec2f* pc, float t)
{
	if (pc == nullptr) { return; }
	float tm1 = 1 - t;
	vec2f p01 = pc[0] * tm1 + pc[1] * t;
	vec2f p12 = pc[1] * tm1 + pc[2] * t;
	vec2f p23 = pc[2] * tm1 + pc[3] * t;
	vec2f p012 = p01 * tm1 + p12 * t;
	vec2f p123 = p12 * tm1 + p23 * t;
	pt = p012 * tm1 + p123 * t;
}

void MemRenderBuffer::__BezierCurveoft(vec2f& pt, std::vector<vec2f>& ctrlpoint, float t)
{
	size_t n = ctrlpoint.size();
	if (n < 1) { return; }
	if (n == 1) { pt = ctrlpoint[0]; }

	vec2f* buffer = new vec2f[n];
	vec2f* curp = buffer;
	vec2f* nextp = curp;
	float onemt = 1 - t;
	memcpy( curp,&ctrlpoint[0],n * sizeof(vec2f) );

	while (--n > 0) // ǰ�ü��� ���ױ����� ��ֵ�Ĳ���
	{
		for (size_t i = 0; i < n; ++i)  // ÿһ�ײ�ֵ�Ĵ��� �Ӷൽ��
		{
			nextp[i] = curp[i] * onemt + curp[i + 1] * t;
		}
	}
	pt = nextp[0];
	delete[] buffer;
	buffer = nullptr;
}

void MemRenderBuffer::_TriangleWeight(const int& x1, const int& y1, const int& x2, const int& y2, const int& x3, const int& y3,
	const int& px, const int& py, float& w1, float& w2, float& w3)
{
	// ���ݹ�ʽ����Ȩ��
	w1 = float((y2 - y3) * (px - x3) + (x3 - x2) * (py - y3)) /
		((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));

	w2 = float((y3 - y1) * (px - x3) + (x1 - x3) * (py - y3)) /
		((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));

	w3 = 1 - w1 - w2;
	//�������Ȩ���м���һ���Ǹ��ģ���ô������������ε��ⲿ
	//�жϹ�����,��Ϊ���ǵ��������ǲü��Ժ���ĵ���еļ��㣬һ������������
}

void MemRenderBuffer::DrawWidthLine1(int x1, int y1, int x2, int y2, int width, const COLOR32& color)
{
	if ((width = abs(width)) < 1) { return; }
	if (width == 1) { DrawLine(x1, y1, x2, y2, color);  return; }
	//ֱ�ߵĻ���
	//�ȴ���һЩ��������
	if (x1 == x2)//����
	{
		int ymin, ymax;
		ymin = min(y1, y2);
		ymax = max(y1, y2);
		int x = x1 - width / 2.0f;
		for (int y = ymin; y <= ymax; ++y)
		{
			for (int w = 0; w < width; ++w)
			{
				//memcpy(&mBufferArray[y][x + w], &color, sizeof(COLOR32));
				DrawPixel(x + w, y, color);
			}
		}
		return;
	}
	else if (y1 == y2)//����
	{
		int xmin, xmax;
		xmin = min(x1, x2);
		xmax = max(x1, x2);
		int y = y1 - width / 2.0f;
		for (int x = xmin; x <= xmax; ++x)
		{
			for (int w = 0; w < width; ++w)
			{
				//memcpy(&mBufferArray[y1][x], &color, sizeof(COLOR32));
				DrawPixel(x, y + w, color);
			}
		}
		return;
	}

	vec2f linedir = vec2f(x1, y1) - vec2f(x2, y2);
	vec2f crosslinedir =
		vec2f(linedir.y, -linedir.x).normalized() * width * 0.5f;
	vec2f curpoint;
	vec2f p1, p2;


	//ʣ����������ʹ��б�ʽ��м���
	//����y�Ĳ�ֵ�ľ���ֵ��x�Ĳ�ֵ�ľ���ֵ����ȷ��������һ�߽�����������
	int delta_y = y1 - y2;
	int delta_x = x1 - x2;

	//�������ֵ
	int delta_ax = abs(delta_x);
	int delta_ay = abs(delta_y);

	if (delta_ay < delta_ax)//����x������б�������
	{
		//��ѧ�ϵ�б�ʣ����ǿ��Կ��ɶ�Ӧ�������ر仯����һ�������ϵı仯��
		float k = float(delta_y) / delta_x;
		int xmin, xmax;
		float y;
		if (x1 < x2)
		{
			xmin = x1;
			xmax = x2;
			y = float(y1);
		}
		else
		{
			xmin = x2;
			xmax = x1;
			y = float(y2);
		}

		for (int x = xmin; x <= xmax; ++x)
		{
			//memcpy(&mBufferArray[int(y)][x], &color, sizeof(COLOR32));
			curpoint.x = x;
			curpoint.y = y;
			p1 = curpoint + crosslinedir;
			p2 = curpoint - crosslinedir;
			DrawLine(p1.x, p1.y, p2.x, p2.y, color);
			y += k;
		}
	}
	else//����y������б�������
	{
		//��ѧ�ϵ�б�ʣ����ǿ��Կ��ɶ�Ӧ�������ر仯����һ�������ϵı仯��
		float k = float(delta_x) / delta_y;
		int ymin, ymax;
		float x;
		if (y1 < y2)
		{
			ymin = y1;
			ymax = y2;
			x = float(x1);
		}
		else
		{
			ymin = y2;
			ymax = y1;
			x = float(x2);
		}
		for (int y = ymin; y <= ymax; ++y)
		{
			//memcpy(&mBufferArray[y][int(x)], &color, sizeof(COLOR32));
			p1 = curpoint + crosslinedir;
			p2 = curpoint - crosslinedir;
			DrawLine(p1.x, p1.y, p2.x, p2.y, color);
			x += k;
		}
	}
}

void MemRenderBuffer::DrawWidthLine2(int x1, int y1, int x2, int y2, int width, const COLOR32& color)
{
	vec2f startpoint = vec2f(x1, y1);
	vec2f endpoint = vec2f(x2, y2);
	vec2f linedir = endpoint - startpoint;
	vec2f crosslinedir =
		vec2f(linedir.y, -linedir.x).normalized() * width * 0.5f;

	vec2f p11, p12, p21, p22;
	p11 = startpoint + crosslinedir;
	p12 = startpoint - crosslinedir;
	p21 = endpoint + crosslinedir;
	p22 = endpoint - crosslinedir;

	std::vector<vec2f> line1, line2;
	GetLinePoints(p11.x, p11.y, p12.x, p12.y, line1);
	GetLinePoints(p21.x, p21.y, p22.x, p22.y, line2);
	size_t pointsnum = min(line1.size(), line2.size());
	for (int i = 0; i < pointsnum; ++i)
	{
		DrawLine(line1[i].x, line1[i].y, line2[i].x, line2[i].y, color);
	}
}

void MemRenderBuffer::DrawWidthLine3(int x1, int y1, int x2, int y2, float width, const COLOR32& color)
{
	std::vector<vec2f> lineMain;
	GetLinePoints(x1, y1, x2, y2, lineMain);
	for (vec2f& p : lineMain)
	{
		FillCircleColor(p.x, p.y, width / 2.0f, color);
	}
}

void MemRenderBuffer::DrawWidthLine4(int x1, int y1, int x2, int y2, float width, const COLOR32& color)
{
	vec2f startpoint(x1, y1);
	vec2f endpoint(x2, y2);
	std::vector<vec2f> centers;
	centers.push_back(startpoint);
	vec2f dir = (endpoint - startpoint).normalized() * width;
	int index = 0;
	float rr = width * width;
	while ((centers[index] - endpoint).sqrMagnitude() > rr)
	{
		centers.push_back(centers[index++] + dir);
	}
	centers.push_back(endpoint);

	Rect rc;
	float r = width / 2.0f;
	for (vec2f& p : centers)
	{
		rc.Set(p.x - r, p.y - r, p.x + r, p.y + r);
		DrawRectangleSolid(rc.left, rc.top, rc.right, rc.bottom, color);
	}
}

void MemRenderBuffer::GetLinePoints(int x1, int y1, int x2, int y2, std::vector<vec2f>& points)
{
	points.clear();
	//ֱ�ߵĻ���
	//�ȴ���һЩ��������
	if (x1 == x2)//����
	{
		int ymin, ymax;
		ymin = min(y1, y2);
		ymax = max(y1, y2);
		for (int y = ymin; y <= ymax; ++y)
		{
			//memcpy(&mBufferArray[y][x1], &color, sizeof(COLOR32));
			points.push_back(vec2f(x1, y));
		}
	}
	else if (y1 == y2)//����
	{
		int xmin, xmax;
		xmin = min(x1, x2);
		xmax = max(x1, x2);
		for (int x = xmin; x <= xmax; ++x)
		{
			//memcpy(&mBufferArray[y1][x], &color, sizeof(COLOR32));
			points.push_back(vec2f(x, y1));
		}
	}

	//ʣ����������ʹ��б�ʽ��м���
	//����y�Ĳ�ֵ�ľ���ֵ��x�Ĳ�ֵ�ľ���ֵ����ȷ��������һ�߽�����������
	int delta_y = y1 - y2;
	int delta_x = x1 - x2;

	//�������ֵ
	int delta_ax = abs(delta_x);
	int delta_ay = abs(delta_y);

	if (delta_ay < delta_ax)//����x������б�������
	{
		//��ѧ�ϵ�б�ʣ����ǿ��Կ��ɶ�Ӧ�������ر仯����һ�������ϵı仯��
		float k = float(delta_y) / delta_x;
		int xmin, xmax;
		float y;
		if (x1 < x2)
		{
			xmin = x1;
			xmax = x2;
			y = float(y1);
		}
		else
		{
			xmin = x2;
			xmax = x1;
			y = float(y2);
		}
		for (int x = xmin; x <= xmax; ++x)
		{
			//memcpy(&mBufferArray[int(y)][x],&color,sizeof(COLOR32));
			//DrawPixel(x, int(y), color);
			points.push_back(vec2f(x, int(y)));
			y += k;
		}
	}
	else//����y������б�������
	{
		//��ѧ�ϵ�б�ʣ����ǿ��Կ��ɶ�Ӧ�������ر仯����һ�������ϵı仯��
		float k = float(delta_x) / delta_y;
		int ymin, ymax;
		float x;
		if (y1 < y2)
		{
			ymin = y1;
			ymax = y2;
			x = float(x1);
		}
		else
		{
			ymin = y2;
			ymax = y1;
			x = float(x2);
		}
		for (int y = ymin; y <= ymax; ++y)
		{
			//memcpy(&mBufferArray[y][int(x)], &color, sizeof(COLOR32));
			//DrawPixel(int(x), y, color);
			points.push_back(vec2f(int(x), y));
			x += k;
		}
	}
}

void MemRenderBuffer::FillCircleColor(int px, int py, int radius, const COLOR32& color)
{
	float radiussqur = radius * radius;
	vec2f center(px, py);
	DrawPixel(px, py, color);

	std::deque<vec2f> points;
	points.push_back(center);
	static int dir[2][4] = {
		0,+1,0,-1,
		-1,0,+1,0
	};
	while (!points.empty())
	{
		//std::cout<<points.size()<<std::endl;
		const vec2f& curpoint = points.front();

		int nx, ny;
		for (int i = 0; i < 4; ++i)
		{
			nx = dir[0][i] + curpoint.x;
			ny = dir[1][i] + curpoint.y;
			if (mRect.PtInRect(nx, ny)//�ڴ����� 
				&& mBufferArray[ny][nx].color32 != color.color32//��ɫ����Ŀ����ɫ
				&& (vec2f(nx, ny) - center).sqrMagnitude() < radiussqur)//����Բ��
			{
				memcpy(&mBufferArray[ny][nx], &color, sizeof(COLOR32));
				points.push_back(vec2f(nx, ny));
			}
		}

		points.pop_front();
	}
}

void MemRenderBuffer::DrawElliptic_pn(int cx, int cy, int a, int b, const COLOR32& color)
{
	/*
	x^2   y^2
	--- + --- - 1 = 0
	a^2   b^2
	*/
	int a2 = a * a;
	int b2 = b * b;
	float da = 1.0f / a2;
	float db = 1.0f / b2;

	int x = 0, y = b;
	float f = 0;
	while (y > 0)
	{
		DrawPixel(cx + x, cy + y, color);
		DrawPixel(cx + x, cy - y, color);
		DrawPixel(cx - x, cy + y, color);
		DrawPixel(cx - x, cy - y, color);
		if (f <= 0)//������
		{
			f += (2 * x + 1) * da;
			++x;
		}
		else
		{
			f += (1 - 2 * y) * db;
			--y;
		}
	}
}

void MemRenderBuffer::DrawElliptic_mp(int cx, int cy, int a, int b, const COLOR32& color)
{

	int b2 = b * b;
	int a2 = a * a;

	float d = b2 + a2 / 4.0f - b * a2;
	int x = 0;
	int y = b;


	while (2 * b2 * x < 2 * a2 * y)
	{
		DrawPixel(cx + x, cy + y, color);
		DrawPixel(cx + x, cy - y, color);
		DrawPixel(cx - x, cy + y, color);
		DrawPixel(cx - x, cy - y, color);
		if (d < 0)//������
		{
			d += b2 * (2 * x + 3);
		}
		else//������
		{
			d += b2 * (2 * x + 3) + a2 * (2 - 2 * y);
			--y;
		}
		++x;
	}

	d = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;
	while (y >= 0)
	{
		DrawPixel(cx + x, cy + y, color);
		DrawPixel(cx + x, cy - y, color);
		DrawPixel(cx - x, cy + y, color);
		DrawPixel(cx - x, cy - y, color);
		if (d >= 0)
		{
			d += -2 * a2 * y + 3 * b2;
		}
		else
		{
			d += 2 * b2 * x - 2 * a2 * y + 3 * a2 + 2 * b2;
			++x;
		}
		--y;
	}
}

void MemRenderBuffer::DrawElliptic_Bresenham(int cx, int cy, int a, int b, const COLOR32& color)
{
	int a2 = a * a, b2 = b * b;
	int x = 0, y = b, d = 4 * b2 + a2 - 4 * a2 * b;

	while (b2 * x < a2 * y)
	{
		DrawPixel(cx + x, cy + y, color);
		DrawPixel(cx + x, cy - y, color);
		DrawPixel(cx - x, cy + y, color);
		DrawPixel(cx - x, cy - y, color);
		if (d < 0)
		{
			d += 4 * (2 * b2 * x + 3 * b2);
		}
		else
		{
			d = d + 4 * (2 * b2 * x - 2 * a2 * y + 2 * a2 + 3 * b2);
			--y;
		}
		++x;
	}

	d = 4 * b2 * x * x + 4 * a2 * y * y + 4 * 2 * b2 * x - a2 * y + a2 + 4 * b2 - 4 * a2 * b2;
	while (y >= 0)
	{
		DrawPixel(cx + x, cy + y, color);
		DrawPixel(cx + x, cy - y, color);
		DrawPixel(cx - x, cy + y, color);
		DrawPixel(cx - x, cy - y, color);

		if (d >= 0)
		{
			d += -4 * (2 * a2 * y + 3 * b2);
		}
		else
		{
			d += 4 * (2 * b2 * x - 2 * a2 * y + 3 * a2 + 2 * b2);
			++x;
		}
		--y;
	}
}

void MemRenderBuffer::DrawElliptic_Bresenham(int cx, int cy, int a, int b, const COLOR32& color, float angle)
{
	int a2 = a * a, b2 = b * b;
	int x = 0, y = b, d = 4 * b2 + a2 - 4 * a2 * b;
	Mat3x3f matro;
	matro.SetRotateDegree(angle).Translate(cx, cy);
	vec2f pt[4];
	while (b2 * x < a2 * y)
	{
		//DrawPixel(cx + x, cy + y, color);
		//DrawPixel(cx + x, cy - y, color);
		//DrawPixel(cx - x, cy + y, color);
		//DrawPixel(cx - x, cy - y, color);
		pt[0].x = x; pt[0].y = y;
		pt[1].x = x; pt[1].y = -y;
		pt[2].x = -x; pt[2].y = y;
		pt[3].x = -x; pt[3].y = -y;
		for (vec2f& p : pt)
		{
			p *= matro;
			DrawPixel(p.x, p.y, color);
		}

		if (d < 0)
		{
			d += 4 * (2 * b2 * x + 3 * b2);
		}
		else
		{
			d = d + 4 * (2 * b2 * x - 2 * a2 * y + 2 * a2 + 3 * b2);
			--y;
		}
		++x;
	}

	d = 4 * b2 * x * x + 4 * a2 * y * y + 4 * 2 * b2 * x - a2 * y + a2 + 4 * b2 - 4 * a2 * b2;
	while (y >= 0)
	{
		//DrawPixel(cx + x, cy + y, color);
		//DrawPixel(cx + x, cy - y, color);
		//DrawPixel(cx - x, cy + y, color);
		//DrawPixel(cx - x, cy - y, color);
		pt[0].x = x; pt[0].y = y;
		pt[1].x = x; pt[1].y = -y;
		pt[2].x = -x; pt[2].y = y;
		pt[3].x = -x; pt[3].y = -y;
		for (vec2f& p : pt)
		{
			p *= matro;
			DrawPixel(p.x, p.y, color);
		}

		if (d >= 0)
		{
			d += -4 * (2 * a2 * y + 3 * b2);
		}
		else
		{
			d += 4 * (2 * b2 * x - 2 * a2 * y + 3 * a2 + 2 * b2);
			++x;
		}
		--y;
	}
}

void MemRenderBuffer::_Copy(const MemRenderBuffer& that)
{
	if (that.mBuffer)
	{
		mRow = that.mRow;
		mCol = that.mCol;
		mRect = that.mRect;
		mPixleSize = mRow * mCol;
		mBuffer = new COLOR32[mPixleSize];
		mBufferArray = new LPCOLOR32[mRow];
		mLineCount = that.mLineCount;
		mZBuffer = new float[mPixleSize];

		for (int i = 0; i < mRow; ++i)
		{
			//�õ�ÿ�е��׵�ַ
			mBufferArray[i] = mBuffer + i * mCol;
		}
		memcpy(mBuffer, that.mBuffer, sizeof(COLOR32) * mPixleSize);
		memcpy(mZBuffer, that.mZBuffer, sizeof(float) * mPixleSize);
	}
}

void MemRenderBuffer::_Clear()
{
	SAFE_DELETE_ARRAY_PTR(mBuffer);
	SAFE_DELETE_ARRAY_PTR(mBufferArray);
	SAFE_DELETE_ARRAY_PTR(mZBuffer);
	mLineCount = mPixleSize = mCol = mRow = 0;
}
