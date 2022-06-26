#pragma once
#include <memory>
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "ResourceObserver.h"
#include "EffectUIEnum.h"

__interface IRenderer;
__interface IResourceManager;

class Engine;
class Camera;
class PlaneAnimation;
class Transform;
struct SpriteRender_Save;

/// <summary>
/// 스프라이트 컴포넌트 
/// 
/// 2022.01.26 B.Bunny
/// </summary>
class Sprite2D : public ComponentBase, public ResourceObserver
{
public:
	_DLL Sprite2D();
	_DLL virtual ~Sprite2D();

	virtual void Start() override;
	virtual void Update(float dTime) override {};
	virtual void OnRender() override;
	virtual void OnUIRender() override;

	_DLL void SetType(eResourceType);

	_DLL void SetProportion(float x, float y);
	_DLL void SetPivot(float pivotX, float pivotY);
	_DLL void SetUIAxis(eUIAxis uiAxis);

	_DLL void SetspriteIndex(unsigned int index);


private:
	std::shared_ptr<IRenderer> m_pRenderer;
	Engine* m_pEngine;
	Camera* m_pCamera;
	PlaneAnimation* m_pPlaneAni;

public:
	std::vector<std::string> m_SpriteName_V;	// 스프라이트 이름
	
private:
	std::vector<unsigned int> m_SpriteIndex_V;	// 스프라이트의 인덱스
	unsigned int m_NowSpriteIndex = 0;

	eResourceType m_resourceType;				// 스프라이트의 이미지 타입(단일이미지, 애니메이션 낱장, 애니메이션 여러장 )
	eUIAxis m_UIAxis;

	float m_width;
	float m_height;

	// 그림 비율 (1은 원본 사이즈)
	float m_ProportionX = 1;
	float m_ProportionY = 1;

	// 피벗(중심점) 0~1까지
	float m_PivotX = 0.5f;
	float m_PivotY = 0.5f;
	
	DirectX::SimpleMath::Matrix m_worldTM;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager) override;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	SpriteRender_Save* m_SaveData;
};

struct SpriteRender_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;

	float				m_ProportionX;
	float				m_ProportionY;

	float				m_PivotX;
	float				m_PivotY;
};

BOOST_DESCRIBE_STRUCT(SpriteRender_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_ProportionX,
	m_ProportionY,

	m_PivotX,
	m_PivotY
	))
