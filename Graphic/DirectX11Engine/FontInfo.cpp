#include "FontInfo.h"
#include "UIData.h"

#include "ComException.h"
#include "ErrorLogger.h"

FontInfo::FontInfo()
{
}

FontInfo::~FontInfo()
{
}

HRESULT FontInfo::Initialize(IDWriteFactory5* pWriteFactory, std::shared_ptr<Text_Queue_Info> pFontDesc)
{
	HRESULT _hr;
	try
	{
		// FontFileReference를 얻기 위해 필요한 객체. 
		IDWriteFontFile* pFontFileReference = nullptr;
		// FontFile을 추가하여 FontSet을 만드는 Builder 객체.
		IDWriteFontSetBuilder1* pFontSetBuilder1;
		// FontCollection과 바꿔먹을 것.
		IDWriteFontSet* pCustomFontSet;

		_hr = pWriteFactory->CreateFontFileReference(pFontDesc->fontName, nullptr, &pFontFileReference);
		COM_ERROR_IF_FAILED(_hr, "CreateFontFileReference Fail");

		_hr = pWriteFactory->CreateFontSetBuilder(&pFontSetBuilder1);
		COM_ERROR_IF_FAILED(_hr, "CreateFontSetBuilder Fail");

		_hr = pFontSetBuilder1->AddFontFile(pFontFileReference);
		COM_ERROR_IF_FAILED(_hr, "AddFontFile Fail");

		_hr = pFontSetBuilder1->CreateFontSet(&pCustomFontSet);
		COM_ERROR_IF_FAILED(_hr, "CreateFontSet Fail");

		_hr = pWriteFactory->CreateFontCollectionFromFontSet(pCustomFontSet, &m_FontCollection);
		COM_ERROR_IF_FAILED(_hr, "CreateFontCollectionFromFontSet Fail");

		pFontFileReference->Release();
		pFontSetBuilder1->Release();
		pCustomFontSet->Release();

		IDWriteFontFamily* fontFamily;
		IDWriteLocalizedStrings* localizedFontName;
		TCHAR c_styleFontName[65];

		_hr = m_FontCollection->GetFontFamily(0, &fontFamily);
		COM_ERROR_IF_FAILED(_hr, "GetFontFamily Fail");

		_hr = fontFamily->GetFamilyNames(&localizedFontName);
		COM_ERROR_IF_FAILED(_hr, "GetFamilyNames Fail");

		_hr = localizedFontName->GetString(0, c_styleFontName, 65);
		COM_ERROR_IF_FAILED(_hr, "GetString Fail");

		fontFamily->Release();
		localizedFontName->Release();

		// pTextFormat 
		//pWriteFactory5->CreateTextLayout()
		_hr = pWriteFactory->CreateTextFormat(
			c_styleFontName,
			m_FontCollection,
			DWRITE_FONT_WEIGHT_SEMI_BOLD,	// 스타일 설정 1
			DWRITE_FONT_STYLE_NORMAL,		// 스타일 설정 2
			DWRITE_FONT_STRETCH_NORMAL,		// 스타일 설정 3
			pFontDesc->fontSize,
			L"ko-kr", //locale
			&m_TextFormat);
		COM_ERROR_IF_FAILED(_hr, "CreateTextFormat Fail");

		// 좌상단 기준으로 텍스트 정렬.
		_hr = m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		COM_ERROR_IF_FAILED(_hr, "SetTextAlignment Fail");

		_hr = m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		COM_ERROR_IF_FAILED(_hr, "SetParagraphAlignment Fail");

		m_LineWidth = pFontDesc->lineWidth;
		m_LineSpace = pFontDesc->lineSpace;
		m_FontSize = pFontDesc->fontSize;

		m_R =  pFontDesc->r;		
		m_G =  pFontDesc->g;
		m_B =  pFontDesc->b;
		m_A =  pFontDesc->a;

		m_PosX =  pFontDesc->posX;		
		m_PosY =  pFontDesc->posY;	

		m_textPivotType = (eUIAxis)pFontDesc->pivotType;
		m_textPoint = (eTextPoint)pFontDesc->textpoint;
	}
	catch(COMException& e)
	{
		ErrorLogger::Log(e);
	}
	return S_OK;
}
