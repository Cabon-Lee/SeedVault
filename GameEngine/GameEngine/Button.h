#pragma once
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "ResourceObserver.h"
#include "EffectUIEnum.h"

__interface IRenderer;

class Camera;
class Transform;
class Engine;
struct SpriteData;
struct Button_Save;

/// <summary>
/// 버튼 컴포넌트
/// 
/// 2022.01.09 B.Bunny
/// </summary>
class Button : public ComponentBase, public ResourceObserver
{
public:
	_DLL Button();
	_DLL virtual ~Button();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;
	virtual void OnUIRender() override;

	_DLL void SetFunc(std::function<void()> onClick);
	_DLL void SetProportion(float x, float y);
	_DLL void SetPivot(float pivotX, float pivotY);
	_DLL void SetAxis(eUIAxis uiAxis);
	_DLL void SetOverSprite(bool ableOver);

private:
	bool Intersect(POINT mousePos);
	RECT GetSpriteRect();

public:
	std::string m_OffSprite;
	std::string m_OnSprite;
	std::string m_OverSprite;

private:
	// 키입력을 받아오기 위한 엔진
	Engine* m_pEngine;
	Camera* m_pCamera;

	// 그림을 그리기 위한 렌더러
	std::shared_ptr<IRenderer> m_pRenderer;

	XMMATRIX m_worldTM;

	unsigned int m_OffSpriteIndex;
	unsigned int m_OnSpriteIndex;
	unsigned int m_OverSpriteIndex;

	eUIAxis m_UIAxis;

	float m_width;
	float m_height;

	// 마우스 입력을 받는 범위
	RECT m_Rect;

	float m_realPosX = 0;
	float m_realPosY = 0;

	eButtonState m_NowState = eButtonState::OFF;
	bool m_IsPushedBefore = false;
	bool m_IsPushed = false;

	bool m_AbleOver = false;

	// 그림 비율 (1은 원본 사이즈)
	float m_ProportionX = 1;
	float m_ProportionY = 1;

	// 피벗(중심점) 0~1까지
	float m_PivotX = 0.5f;
	float m_PivotY = 0.5f;

	std::function<void()> m_onClick;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	Button_Save* m_SaveData;
};

struct Button_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;

	std::string m_OnSprite;
	std::string m_OffSprite;

	float m_ProportionX;
	float m_ProportionY;

	float m_PivotX;
	float m_PivotY;
};


BOOST_DESCRIBE_STRUCT(Button_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_OnSprite,
	m_OffSprite,
	
	m_ProportionX,
	m_ProportionY,

	m_PivotX,
	m_PivotY
	
	))

