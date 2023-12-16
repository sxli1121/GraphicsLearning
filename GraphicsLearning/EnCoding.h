#pragma once
#ifndef __MY_ENCODING_H__
#define __MY_ENCODING_H__
#include <string>
#include <windows.h>

namespace EnCodeing
{
	//��Unicode���ַ�ת����ascii���ֽ��ַ�
	inline std::string W2C(const wchar_t* wp)
	{
		if (wp == nullptr) { return std::string(); }
		//��ȡ���ַ��ĳ���
		int wlen = (int)wcslen(wp);
		//�õ�ת���Ժ�ĳ���
		int slen = WideCharToMultiByte(CP_ACP, 0, wp, wlen, nullptr, 0, nullptr, nullptr);
		char* _char = new char[slen + 1];//�洢ת������Ķ��ڴ�
		//������ִ��ת��
		WideCharToMultiByte(CP_ACP, 0, wp, wlen, _char, slen, nullptr, nullptr);
		_char[slen] = '\0';
		std::string retstr = _char;
		delete[] _char;
		return retstr;
	}
	//��ascii���ֽ��ַ�ת����Unicode���ַ�
	inline  std::wstring C2W(const char* sc)
	{
		int slen = (int)strlen(sc);
		size_t wlen = (size_t)MultiByteToWideChar(CP_ACP, 0, sc, slen, nullptr, 0);
		wchar_t* sw = new wchar_t[wlen + 1];
		MultiByteToWideChar(CP_ACP, 0, sc, slen, sw, wlen);
		sw[wlen] = 0;
		std::wstring retstr = sw;
		delete[] sw;
		return retstr;
	}
}
#endif