//#pragma once
//#include <memory>
//#include "DLLDefine.h"
//#include "ComponentBase.h"
//#include "ResourceObserver.h"
//#include "EffectUIEnum.h"
//
//__interface IRenderer;
//__interface IResourceManager;
//
//class Engine;
//class Camera;
//class PlaneAnimation;
//class Transform;
//struct SpriteRender_Save;
//
//struct SingleSpriteInfo 
//{
//	std::string SpriteName;
//
//	float ProportionX = 1;
//	float ProportionY = 1;
//
//
//};
//
//
///// <summary>
///// 스프라이트번들 컴포넌트 
///// : 게임오브젝트에 여러개의 컴포넌트를 붙일 수 없기때문에 
/////   여러개의 이미지를 띄우고 싶을 경우 사용할려고 만듦
/////  
///// 2022.05.23 B.Bunny
///// </summary>
//class Sprite2DBundle : public ComponentBase, public ResourceObserver
//{
//public:
//	_DLL Sprite2DBundle();
//	_DLL virtual ~Sprite2DBundle();
//
//	virtual void Start() override;
//	virtual void Update(float dTime) override {};
//	virtual void OnRender() override;
//	virtual void OnUIRender() override;
//
//	_DLL void SetType(eResourceType);
//
//	_DLL void SetProportion(float x, float y);
//	_DLL void SetPivot(float pivotX, float pivotY);
//	_DLL void SetUIAxis(UIAxis uiAxis);
//
//private:
//	std::shared_ptr<IRenderer> m_pRenderer;
//	Engine* m_pEngine;
//	Camera* m_pCamera;
//	PlaneAnimation* m_pPlaneAni;
//
//public:
//	std::vector<SingleSpriteInfo> m_SingleSprite_V;	// 스프라이트들의 정보들 이름
//
//private:
//	std::vector<unsigned int> m_SpriteIndex_V;	// 스프라이트의 인덱스
//	eResourceType m_resourceType;				// 스프라이트의 이미지 타입(단일이미지, 애니메이션 낱장, 애니메이션 여러장 )
//	UIAxis m_UIAxis;
//
//	float m_width;
//	float m_height;
//
//	// 그림 비율 (1은 원본 사이즈)
//	float m_ProportionX = 1;
//	float m_ProportionY = 1;
//
//	// 피벗(중심점) 0~1까지
//	float m_PivotX = 0.5f;
//	float m_PivotY = 0.5f;
//
//	DirectX::SimpleMath::Matrix m_worldTM;
//
//public:
//	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager) override;
//
//public:
//	virtual void SaveData();
//	virtual void LoadData();
//
//protected:
//	SpriteRender_Save* m_SaveData;
//};
//
//struct SpriteRender_Save
//{
//	bool				m_bEnable;
//	unsigned int		m_ComponentId;
//
//	float				m_ProportionX;
//	float				m_ProportionY;
//
//	float				m_PivotX;
//	float				m_PivotY;
//};
//
//BOOST_DESCRIBE_STRUCT(SpriteRender_Save, (), (
//	m_bEnable,
//	m_ComponentId,
//
//	m_ProportionX,
//	m_ProportionY,
//
//	m_PivotX,
//	m_PivotY
//	))
