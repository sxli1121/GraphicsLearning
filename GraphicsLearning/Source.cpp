#include <iostream>
#include "Application.h"
#ifndef _DEBUG
//�������ο���̨���ڣ����Ե�ʱ����Ч��ֱ�����г����ʱ����Ч
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif



// ����Լ�� _stdcall  _cdcel  _fastcall

int main()
{
    if (!App.Run("LISIXUEͼ��ѧѧϰ���-����"))
    {
        printf("����Ӧ�ô���ʧ��\n");
    }

    system("pause");
    return 0;
}
