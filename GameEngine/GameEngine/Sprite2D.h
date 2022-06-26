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
/// ��������Ʈ ������Ʈ 
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
	std::vector<std::string> m_SpriteName_V;	// ��������Ʈ �̸�
	
private:
	std::vector<unsigned int> m_SpriteIndex_V;	// ��������Ʈ�� �ε���
	unsigned int m_NowSpriteIndex = 0;

	eResourceType m_resourceType;				// ��������Ʈ�� �̹��� Ÿ��(�����̹���, �ִϸ��̼� ����, �ִϸ��̼� ������ )
	eUIAxis m_UIAxis;

	float m_width;
	float m_height;

	// �׸� ���� (1�� ���� ������)
	float m_ProportionX = 1;
	float m_ProportionY = 1;

	// �ǹ�(�߽���) 0~1����
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
