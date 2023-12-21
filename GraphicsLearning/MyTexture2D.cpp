#include "MyTexture2D.h"
#include <windows.h>

MyTexture2D::MyTexture2D()
{
	mData = nullptr;
	mWidth = mHeight = mLineCount = 0;
}

MyTexture2D::~MyTexture2D()
{
	SAFE_DELETE_ARRAY_PTR(mData);
	mWidth = mHeight = mLineCount = 0;
}

bool MyTexture2D::LoadTexture(const char* texturefile)
{
	if (texturefile == nullptr) { return false; }

	FILE* bmp = nullptr;
	fopen_s(&bmp, texturefile, "rb");
	if (bmp == nullptr) { return false; }
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	fread(&bf, 1, sizeof(bf), bmp);
	fread(&bi, 1, sizeof(bi), bmp);

	if (bf.bfType != 0x4d42 || bi.biBitCount != 24)
	{
		fclose(bmp);
		bmp = nullptr;
		return false;
	}

	mData = new unsigned char[bi.biSizeImage];
	if (mData == nullptr)
	{
		fclose(bmp);
		bmp = nullptr;
		return false;
	}

	fread(mData, 1, bi.biSizeImage, bmp);
	fclose(bmp);
	bmp = nullptr;
	mWidth = bi.biWidth;
	mHeight = bi.biHeight;
	mLineCount = bi.biSizeImage / bi.biHeight;
	return true;
}

COLOR32 MyTexture2D::GetUVColor(const vec2f& uv)
{
	return GetUVColor(uv.u, uv.v);
}

COLOR32 MyTexture2D::GetUVColor(float u, float v)
{
	if (mData == nullptr) { return COLOR32::White(); }
	//通过UV坐标计算出图片上某一个像素的坐标
	int x = (int)(UVREPEAT(u) * (mWidth - 1));
	int y = (int)(UVREPEAT(v) * (mHeight - 1));
	//取对应像素的颜色
	unsigned char* pColor24 = mData + (y * mLineCount + x * 3);
	return COLOR32(pColor24[2], pColor24[1], pColor24[0]);
}

TextureManager::~TextureManager()
{
	if (!mTexMap.empty())
	{
		for (auto& item:mTexMap)
		{
			delete item.second;
		}
		mTexMap.clear();
	}
}

MyTexture2D* TextureManager::LoadTexture(const char* textpath)
{
	auto it = mTexMap.find(textpath);
	if (it != mTexMap.end())
	{
		return it->second;
	}
	MyTexture2D* temp = new MyTexture2D;
	if (temp != nullptr)
	{
		if (temp->LoadTexture(textpath))
		{
			mTexMap[textpath] = temp;
			return temp;
		}
		delete temp;
		temp = nullptr;
	}
	return temp;
}
