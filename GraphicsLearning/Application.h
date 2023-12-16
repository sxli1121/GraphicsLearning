#pragma once
#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include "SingletonTemplate.h"
#include <windows.h>


class Application :public Singleton<Application>
{
	PRIVATE_CONSTRUCTOR_AND_COPY(Application)
public:

	bool Run(const char* title,
		int clientwidth = 800,
		int clientheight = 600);
	POINT WndSize()
	{
		return { _clientWidth ,_clientHeight };
	}
protected:
private:
	HWND _hwnd;//窗口句柄
	HINSTANCE _hinst;//实例句柄
	int _wndWidth;//窗口的大小
	int _wndHeight;
	int _clientWidth;//客户区的大小
	int _clientHeight;
	HDC mMainDC;

	//位图信息，主要记录的是我们自己实现的用于显示在屏幕上的颜色数据
	BITMAPINFO mBitMapinfo;
};
#define App Application::Instance()
#endif