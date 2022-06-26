#pragma once
#include <memory>
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "ResourceObserver.h"
#include "EffectUIEnum.h"

__interface IRenderer;
__interface IResourceManager;

class Camera;
class Transform;
class PlaneAnimation;
struct Billboard_Save;

/// <summary>
/// 빌보드 컴포넌트
/// 
/// 2021.12.29 B.Bunny
/// </summary>
class Billboard : public ComponentBase, public ResourceObserver
{
public:
	_DLL Billboard();
	_DLL virtual ~Billboard();

	virtual void Start() override;
	virtual void Update(float dTime) override {};
	virtual void OnRender() override;
	virtual void OnUIRender() override;

	_DLL void SeteResourceType(eResourceType resourceType);
	_DLL void SetRotationType(eRotationType rotationType);
	
	_DLL void SetProportion(float x, float y);
	_DLL void SetPivot(float pivotX, float pivotY);

private:
	std::shared_ptr<IRenderer> m_pRenderer;
	Camera* m_pCamera;
	PlaneAnimation* m_pPlaneAni;

public:
	std::vector<std::string> m_SpriteName_V;
	
private:
	std::vector<unsigned int> m_SpriteIndex_V;
	
	eResourceType m_resourceType;
	eRotationType m_RotationType;
	// 중심점 자리 설정값
	
	// 그림 비율 (1은 원본 사이즈)
	float m_ProportionX = 1;
	float m_ProportionY = 1;

	// 피벗(중심점) 0~1까지
	float m_PivotX = 0.5f;
	float m_PivotY = 0.5f;

	bool m_IsGizmoOn;

	DirectX::SimpleMath::Matrix m_worldTM;	// MyTransform으로부터 유도되므로 저장될 필요 없음

private:
	void Y_FixRender();
	void LookAtdRender();

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	Billboard_Save* m_SaveData;
};

struct Billboard_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;

	//std::string m_SpriteName;


	//float m_ProportionX;
	//float m_ProportionY;
	//
	//float m_PivotX;
	//float m_PivotY;
};


BOOST_DESCRIBE_STRUCT(Billboard_Save, (), (
	m_bEnable,
	m_ComponentId,
	
	//m_SpriteName,

	//m_ProportionX,
	//m_ProportionY,
	//
	//m_PivotX,
	//m_PivotY

	))