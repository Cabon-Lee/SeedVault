#pragma once
#include <windows.h>
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

struct TextString
{
	TextString()
		: text(L"") {};

	TCHAR text[1024] = L"";
};

//struct Text_Queue_Data
//{
//	Text_Queue_Data()
//	{
//		Position_X = 0.f;
//		Position_Y = 0.f;
//		Text_Width = 0.f;
//		Color_R = 0.f;
//		Color_G = 0.f;
//		Color_B = 0.f;
//		Color_Alpha = 0.f;
//		Text_Size = 0.f;
//		Text_String = nullptr;
//	}
//	~Text_Queue_Data()
//	{
//		delete[] Text_String;
//	}
//
//	float Position_X;		// 텍스트 그리기 시작점 X
//	float Position_Y;		// 텍스트 그리기 시작점 Y
//
//	float Text_Width;		// 텍스트 박스 크기
//
//	float Color_R;			// Color
//	float Color_G;
//	float Color_B;
//	float Color_Alpha;
//
//	float Text_Size;		// 텍스트 폰트 크기
//
//	wchar_t* Text_String = nullptr;	// 텍스트 내용
//};

struct Text_Queue_Info
{
	Text_Queue_Info()
	{
		fontName = nullptr;

		posX = 0.f;
		posY = 0.f;

		r = 0.f;
		g = 0.f;
		b = 0.f;
		a = 0.f;

		lineWidth = 0.f;
		lineSpace = 0.f;
		fontSize = 0.f;
	}
	~Text_Queue_Info()
	{
		//delete[] fontName;
	}
	wchar_t const* fontName;

	float posX;		// 텍스트 그리기 시작점 X
	float posY;		// 텍스트 그리기 시작점 Y

	float r;		// Color
	float g;
	float b;
	float a;

	float lineWidth;	// 텍스트 박스 크기(너비)
	float lineSpace;	// 텍스트 행간
	float fontSize;		// 텍스트 폰트 크기

	unsigned int pivotType;
	unsigned int textpoint;
};

struct TextBlock
{
	unsigned int fontInfoIndex;
	std::vector<std::shared_ptr<TextString>> textString_V;

	void ResetTextBlock()
	{
		for (int i = 0; i < textString_V.size(); i++)
		{
			textString_V[i].reset();
		}
		textString_V.clear();
	}
};

struct ParticleInfo
{
	float positionX, positionY, positionZ;
	float red, green, blue;
	float velocity;
	bool active;

	float currentParticleCount;
	float particleSize;
	int maxParticles;
};

struct ParticleSetting
{
	float particleSize;
	int maxParticles;
};