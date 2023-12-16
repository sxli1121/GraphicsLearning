#pragma once
#ifndef __MY_ENCODING_H__
#define __MY_ENCODING_H__
#include <string>
#include <windows.h>

namespace EnCodeing
{
	//将Unicode宽字符转换到ascii多字节字符
	inline std::string W2C(const wchar_t* wp)
	{
		if (wp == nullptr) { return std::string(); }
		//获取宽字符的长度
		int wlen = (int)wcslen(wp);
		//得到转换以后的长度
		int slen = WideCharToMultiByte(CP_ACP, 0, wp, wlen, nullptr, 0, nullptr, nullptr);
		char* _char = new char[slen + 1];//存储转换结果的堆内存
		//真正的执行转换
		WideCharToMultiByte(CP_ACP, 0, wp, wlen, _char, slen, nullptr, nullptr);
		_char[slen] = '\0';
		std::string retstr = _char;
		delete[] _char;
		return retstr;
	}
	//将ascii多字节字符转换到Unicode宽字符
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