#pragma once

#include "DLLDefine.h"
#include "ComponentBase.h"
#include "ResourceObserver.h"

enum class eLIGHT_TYPE
{
	DIRECTIONAL = 0,
	POINT,
	SPOT,
};

__interface IRenderer;
class Transform;
struct Light_Save;

class Light : public ComponentBase, public ResourceObserver
{
public:

	_DLL Light();
	_DLL virtual ~Light();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;

	virtual void OnPreRender() override;
	virtual void OnRender() override;


	_DLL void SetPostion(const DirectX::SimpleMath::Vector3& pos);
	_DLL void SetRotation(const DirectX::SimpleMath::Vector3& axis, float rotVal);
	_DLL void SetColor(const DirectX::SimpleMath::Vector3& color);
	_DLL void SetColor(const float& r, const float& g, const float& b);
	_DLL void SetIntensity(float val);
	_DLL void SetCastShadow(bool val);

	_DLL const DirectX::SimpleMath::Matrix& GetTransform();
	_DLL const DirectX::SimpleMath::Vector3& GetPostion();
	_DLL const DirectX::SimpleMath::Quaternion& GetRotation();
	_DLL const DirectX::SimpleMath::Vector3& GetColor();
	_DLL float GetIntensity();
	_DLL bool  GetCastShadow();
	_DLL const eLIGHT_TYPE& GetLightType();


	// 일부러 퍼블릭으로 뻈음
	eLIGHT_TYPE m_eLightType;

	bool m_CastShadow;
	bool m_IsTextured;

	std::string m_LightTextureName;
	unsigned int m_LightTextureIndex;

private:
	std::shared_ptr<IRenderer> m_pRenderer;

	Transform* m_pMyTransform;

	DirectX::SimpleMath::Vector3 m_Color;
	float m_Intensity;

public:
	_DLL void SetFalloffStart(float value);
	_DLL void SetFalloffEnd(float value);
	_DLL void SetSpotPower(float value);

	_DLL float GetFalloffStart();
	_DLL float GetFalloffEnd();
	_DLL float GetSpotPower();

	float m_FalloffStart;
	float m_FalloffEnd;
	float m_SpotPower;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	Light_Save* m_SaveData;
};

struct Light_Save
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;
	int		m_eLightType;
	bool	m_CastShadow;
	bool	m_IsTextured;
	float	m_Intensity;
	Vector3*	m_Color;
	float	m_FalloffStart;
	float	m_FalloffEnd;
	float	m_SpotPower;
	std::string m_TextureName;
};

BOOST_DESCRIBE_STRUCT(Light_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_eLightType,
	m_CastShadow,
	m_IsTextured,
	m_Intensity,
	m_Color,
	m_FalloffStart,
	m_FalloffEnd,
	m_SpotPower,
	m_TextureName
	))