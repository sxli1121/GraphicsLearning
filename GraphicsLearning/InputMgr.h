#pragma once
#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__
#include "SingletonTemplate.h"
#include <windows.h>

//定义按键的状态的枚举
enum class emKeyState
{
	KS_UNKNOWN = 0,//未定义的状态
	KS_KEEP_UP = 1,//0001
	KS_JUST_UP = 2,//0010
	KS_KEEP_DOWN = 4,//0100
	KS_JUST_DOWN = 8//1000
};


class InputMgr:public Singleton<InputMgr>
{
	//只屏蔽拷贝，保留构造
	PRIVATE_COPY(InputMgr);
private:
	InputMgr();
public:
	void Init(HWND hwnd);
	void UpdateKeyState();

	emKeyState KeyState(unsigned char vk);
	emKeyState operator [](unsigned char vk);

	const POINT& GetMousePos();

	bool KeyDown(unsigned char vk);
	bool KeyJustDown(unsigned char vk);
	bool KeyUp(unsigned char vk);
	bool KeyJustUp(unsigned char vk);

private:
	emKeyState mKeyStates[256];
	HWND mHwnd;
};
#define Input InputMgr::Instance()
#endif