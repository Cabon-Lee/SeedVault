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

	// Ŀ���� ��Ʈ ����.
	IDWriteFontCollection1* m_FontCollection = nullptr;
	// �ؽ�Ʈ ����.
	IDWriteTextFormat* m_TextFormat = nullptr;

	/// ���ʿ��� Release ����..
	// �ؽ�Ʈ ���̾ƿ�
	IDWriteTextLayout* m_TextLayOut = nullptr;
	// �ؽ�Ʈ ũ��
	//float m_Font_Size = 24.f;

	float m_LineWidth;		// �ؽ�Ʈ �ڽ� ũ��(�ʺ�)
	float m_LineSpace;		// �ؽ�Ʈ �ణ
	float m_FontSize;			// �ؽ�Ʈ ��Ʈ ũ��

	float m_R;		// Color
	float m_G;
	float m_B;
	float m_A;

	float m_PosX;		// �ؽ�Ʈ �׸��� ������ X
	float m_PosY;		// �ؽ�Ʈ �׸��� ������ Y

	eUIAxis m_textPivotType;	//
	eTextPoint m_textPoint;
};

