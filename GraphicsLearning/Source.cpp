#include <iostream>
#include "Application.h"
#ifndef _DEBUG
//可以屏蔽控制台窗口，调试的时候无效，直接运行程序的时候有效
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif



// 调用约定 _stdcall  _cdcel  _fastcall

int main()
{
    if (!App.Run("LISIXUE图形学学习框架-复现"))
    {
        printf("创建应用窗口失败\n");
    }

    system("pause");
    return 0;
}
