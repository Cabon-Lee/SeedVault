#pragma once
#include <dwrite.h>
#include <dwrite_3.h>
#include <EffectUIEnum.h>

#include <memory>

struct Text_Queue_Info;

class FontInfo
{
public:
	FontInfo();
	~FontInfo();

	HRESULT Initialize(IDWriteFactory5* pWriteFactory, std::shared_ptr<Text_Queue_Info> pFontDesc);


public:

	// 커스텀 폰트 정보.
	IDWriteFontCollection1* m_FontCollection = nullptr;
	// 텍스트 포멧.
	IDWriteTextFormat* m_TextFormat = nullptr;

	/// 불필요한 Release 방지..
	// 텍스트 레이아웃
	IDWriteTextLayout* m_TextLayOut = nullptr;
	// 텍스트 크기
	//float m_Font_Size = 24.f;

	float m_LineWidth;		// 텍스트 박스 크기(너비)
	float m_LineSpace;		// 텍스트 행간
	float m_FontSize;			// 텍스트 폰트 크기

	float m_R;		// Color
	float m_G;
	float m_B;
	float m_A;

	float m_PosX;		// 텍스트 그리기 시작점 X
	float m_PosY;		// 텍스트 그리기 시작점 Y

	eUIAxis m_textPivotType;	//
	eTextPoint m_textPoint;
};

