#pragma once
#include <memory>
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "EffectUIEnum.h"

__interface IRenderer;
__interface IResourceManager;

class Camera;
class MeshFilter;
class MaterialComponent;
class Transform;
struct Text_Queue_Info;
struct TextString;
struct TextBlock;
struct Text_Save;


/// <summary>
/// D2D기반 Text 컴포넌트
/// 
/// 2022.01.09 B.Bunny
/// </summary>
class Text : public ComponentBase
{
public:
	_DLL Text();
	_DLL virtual ~Text();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL void SetSpriteTextInfo(
		wchar_t const* fontName,
		float posX, float posY,
		float r, float g, float b, float a,
		float lineWidth,
		float lineSpace,
		float fontSize,
		eUIAxis PivotType,
		eTextPoint textPoint
		);

	_DLL void PrintSpriteText(
		TCHAR* text, ...);
	_DLL void ResetBuffer();

private:
	unsigned int m_width;
	unsigned int m_Height;
	
	std::shared_ptr<IRenderer> m_pRenderer;

	std::shared_ptr<Text_Queue_Info> m_FontDesc;
	std::shared_ptr<TextBlock> m_TextBlock;

public:
	virtual void SaveData() override;
	virtual void LoadData() override;

private:
	// size, rgb, space, font
	// std::string FontName;

protected:
	Text_Save* m_SaveData;
};

struct Text_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;
};

BOOST_DESCRIBE_STRUCT(Text_Save, (), (
	m_bEnable,
	m_ComponentId
	))
