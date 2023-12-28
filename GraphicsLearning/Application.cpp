#include "Application.h"
#include "EnCoding.h"
#include "InputMgr.h"
#include "Scene.h"
#include "RenderList.h"
#include "MemRenderBuffer.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	};
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool Application::Run(const char* title,
	int clientwidth,
	int clientheight)
{
	//得到实例句柄
	HINSTANCE hInstance = GetModuleHandleA(nullptr);
	_hinst = hInstance;

	//定义窗口类
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = nullptr;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = TEXT("GameWnd");
	wcex.hIconSm = nullptr;

	RegisterClassExW(&wcex);

#ifdef _UNICODE
	std::wstring wndtitle = EnCodeing::C2W(title);
#else
	std::string wndtitle = title;
#endif

	HWND hwnd = CreateWindow(wcex.lpszClassName, wndtitle.c_str(), WS_OVERLAPPEDWINDOW,
		0, 0, clientwidth, clientheight, nullptr, nullptr, hInstance, nullptr);

	if (!hwnd)
	{
		MessageBox(nullptr, TEXT("窗口创建失败"), TEXT("窗口创建提示"), MB_OK);
		return false;
	}
	_hwnd = hwnd;

	printf("成功创建窗口\n");
	//调整一下窗口的位置
	RECT rc = { 0,0,clientwidth,clientheight };//希望的客户区的大小
	AdjustWindowRect(&rc, GetWindowLong(hwnd, GWL_STYLE), FALSE);

	//存储客户区的窗口大小
	_clientWidth = clientwidth;
	_clientHeight = clientheight;

	//存储新的窗口的大小
	_wndWidth = rc.right - rc.left;
	_wndHeight = rc.bottom - rc.top;

	int screex = GetSystemMetrics(SM_CXSCREEN);
	int screey = GetSystemMetrics(SM_CYSCREEN);

	//移动窗口,MoveWindow不光可以移动窗口还可以改变窗口的大小
	MoveWindow(hwnd,
		(screex - (rc.right - rc.left)) / 2,//新的窗口的左上角x
		(screey - (rc.bottom - rc.top)) / 2,//新的窗口的左上角y
		rc.right - rc.left,//新的窗口宽度
		rc.bottom - rc.top,//新的窗口高度
		TRUE);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	//获取窗口的客户区DC
	mMainDC = GetDC(_hwnd);
	//设置文字渲染背景透明
	SetBkMode(mMainDC, TRANSPARENT);

	//将位图信息结构体清空
	memset(&mBitMapinfo, 0, sizeof(mBitMapinfo));

	mBitMapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	mBitMapinfo.bmiHeader.biWidth = _clientWidth;
	//高度为负是为了让我们的颜色数据不用标准位图的格式定义
	//可以让像素从上到下从左往右的符合我们经验的顺序定义
	mBitMapinfo.bmiHeader.biHeight = -_clientHeight;
	mBitMapinfo.bmiHeader.biPlanes = 1;
	mBitMapinfo.bmiHeader.biBitCount = 32;

	//创建内存渲染器
	if (!MRB.Create(_clientHeight, _clientWidth))
	{
		printf("内存渲染器创建失败\n");
		return false;
	}

	//初始化输入管理器
	Input.Init(hwnd);

	//初始化场景
	if (!MainScene.Init())
	{
		printf("场景初始化失败\n");
		system("pause");
		return 0;
	}

	//循环相关的时间变量
	ULONGLONG lasttime = GetTickCount64();
	ULONGLONG curtime = 0;
	ULONGLONG deltatime = 0;
	float deltatimef = 0;

	//消息循环
	MSG msg = {};
	do
	{
		//有消息处理消息并将处理过的消息移除
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//消息空闲的时候执行我们的逻辑
			curtime = GetTickCount64();
			deltatime = curtime - lasttime;
			if (deltatime >= 16)
			{
				//计算帧间隔时间float值
				deltatimef = deltatime / 1000.0f;
				//一次性讲内容结果绘制到窗口主DC上
				//将指定的位图数据直接绘制到DC上
				SetDIBitsToDevice(
					mMainDC,//目标DC
					0, 0,//目标的左上角
					_clientWidth,//目标的宽度
					_clientHeight,//目标的高度
					0, 0,//源区域的左上角
					0,//表示从第几行开始时拷贝数据
					_clientHeight,//拷贝行数
					MRB.BufferData(),//需要颜色数据的指针
					&mBitMapinfo,//用于描述颜色数据的存储方式
					DIB_RGB_COLORS);

				//处理输入信息
				Input.UpdateKeyState();

				//调用逻辑和绘制相关的内容
				MainScene.Update(deltatimef);

				//清空地形三角形
				TRL.ClearTriangle();
				//每次绘制之前清掉三角形
				RL.ClearTriangle();

				////用指定颜色清除背景
				MRB.FillColor(RGB(0, 0, 0));
				//清除深度缓冲区
				MRB.ClearZ();


				MainScene.Render(deltatimef);

				//绘制地形
				TRL.RenderAllTriangle();
				//最后批量绘制三角形
				RL.RenderAllTriangle();



				lasttime = curtime;
			}
		}
	} while (msg.message != WM_QUIT);
	return true;
}