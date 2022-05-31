#include "pch.h"
#include "Text.h"
#include "UIData.h"
#include "EngineDLL.h"

#include "IRenderer.h"
#include "IResourceManager.h"

Text::Text()
	: ComponentBase(ComponentType::Rendering)//, m_SaveData(new Text_Save())
{
	m_TextBlock = std::make_shared<TextBlock>();
}

Text::~Text()
{
}

void Text::Start()
{
	m_pRenderer = DLLEngine::GetEngine()->GetIRenderer();

	m_width = static_cast<float>(DLLWindow::GetScreenWidth());
	m_Height = static_cast<float>(DLLWindow::GetScreenHeight());
	
	m_TextBlock->fontInfoIndex = m_pRenderer->FontInfoInitialize(m_FontDesc);
}

void Text::Update(float dTime)
{
}

void Text::OnRender()
{
	m_pRenderer->DrawD2DText(m_TextBlock);
}

void Text::SetSpriteTextInfo(
	wchar_t const* fontName,
	float posX, float posY,
	float r, float g, float b, float a,
	float lineWidth,
	float lineSpace,
	float fontSize,
	UIAxis PivotType)
{
	std::shared_ptr<Text_Queue_Info> _textInfo = std::make_shared<Text_Queue_Info>();

	_textInfo->fontName = fontName;

	_textInfo->posX = posX;
	_textInfo->posY = posY;
	
	_textInfo->r = r;
	_textInfo->g = g;
	_textInfo->b = b;
	_textInfo->a = a;

	_textInfo->lineWidth = lineWidth;
	_textInfo->lineSpace = lineSpace;
	_textInfo->fontSize = fontSize;

	_textInfo->pivotType = (unsigned int)PivotType;

	m_FontDesc.swap(_textInfo);
}

void Text::PrintSpriteText(TCHAR* text, ...)
{
	std::shared_ptr<TextString> _textString = std::make_shared<TextString>();

	va_list vl;
	va_start(vl, text);
	_vstprintf(_textString->text, 1024, text, vl);
	va_end(vl);

	m_TextBlock->textString_V.push_back(_textString);
}

void Text::ResetBuffer()
{
	m_TextBlock->ResetTextBlock();
}

void Text::SaveData()
{
	//m_SaveData->m_bEnable = GetIsEnabled();
	//m_SaveData->m_ComponentId = m_ComponentId;
	//
	//m_SaveData->m_TextInfo_V = m_TextInfo_V;
	//m_SaveData->m_textBuffer_V = m_textBuffer_V;
	//m_SaveData->m_setupConverPoint_V = m_setupConverPoint_V;
	//
	//save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Text::LoadData()
{
	//save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	//
	//SetEnable(m_SaveData->m_bEnable);
	//
	//m_TextInfo_V = m_SaveData->m_TextInfo_V;
	//m_textBuffer_V = m_SaveData->m_textBuffer_V;
	//m_setupConverPoint_V = m_SaveData->m_setupConverPoint_V;
}