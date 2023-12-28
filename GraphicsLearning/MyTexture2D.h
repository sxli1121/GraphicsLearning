#pragma once
#ifndef __MY_TEXTURE_2D_H__
#define __MY_TEXTURE_2D_H__
#include <map>
#include <string>
#include "SingletonTemplate.h"
#include "UtilityHelper.h"
#include "math3d/Vector2.h"
using namespace Math;
using namespace Utility;

class MyTexture2D
{
public:
	MyTexture2D(const MyTexture2D&) = delete;
	MyTexture2D& operator =(const MyTexture2D&) = delete;
	MyTexture2D();
	~MyTexture2D();

	bool LoadTexture(const char* texturefile);

	COLOR32 GetUVColor(const vec2f& uv);
	COLOR32 GetUVColor(float u, float v);
	int GetPixelGray(int x, int y);

	const int& Width() const { return mWidth; }
	const int& Height() const { return mHeight; }

protected:
	int mWidth;
	int mHeight;
	int mLineCount;
	unsigned char* mData;
};


// 图片管理类
class TextureManager:public Singleton<TextureManager>
{
	 PRIVATE_CONSTRUCTOR_AND_COPY(TextureManager)
public:
	~TextureManager();
	MyTexture2D* LoadTexture(const char* textpath);
protected:
	std::map<std::string, MyTexture2D*> mTexMap;
};
#define TextureMgr TextureManager::Instance()
#endif