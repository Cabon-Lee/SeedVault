#include "pch.h"
#include "Light.h"
#include "Transform.h"

#include "IRenderer.h"
#include "IResourceManager.h"
#include "EngineDLL.h"
#include "Managers.h"

Light::Light()
	: ComponentBase(ComponentType::Light)
	, m_eLightType(eLIGHT_TYPE::DIRECTIONAL)
	, m_Intensity(1.0f)
	, m_FalloffStart(0.0f)
	, m_FalloffEnd(0.0f)
	, m_SpotPower(0.0f)
	, m_CastShadow(false)
	, m_IsTextured(false)
	, m_LightTextureIndex(0)
	, m_SaveData(new Light_Save())
{
	m_Color = { 1.0f, 1.0f, 1.0f };
}

Light::~Light()
{

}

void Light::Start()
{
	m_pMyTransform = m_pMyObject->GetComponent<Transform>();

	Engine* engine = DLLEngine::GetEngine();
	m_pRenderer = engine->GetIRenderer();

}

void Light::Update(float dTime)
{


}

void Light::OnPreRender()
{

	if (this->GetIsEnabled() != true) return;

	DirectX::XMFLOAT3 _nowAnlge;

	_nowAnlge.x = m_pMyTransform->m_EulerAngles.x * MathHelper::Pi / 180;
	_nowAnlge.y = m_pMyTransform->m_EulerAngles.y * MathHelper::Pi / 180;
	_nowAnlge.z = m_pMyTransform->m_EulerAngles.z * MathHelper::Pi / 180;

	// 광원의 위치 정보를 통해 연산을 할지 말지 결정한다
	// 만약 너무 멀면 그림자, 텍스쳐 라이트 연산을 하지 않는다
	if (m_pRenderer->LightUpdate(
		static_cast<unsigned int>(m_eLightType),				// 광원의 형태
		m_pMyTransform->GetWorldPosition(), _nowAnlge,			// 광원의 Transform 정보
		m_Color, m_Intensity,									// 광원 색상, 밝기
		m_FalloffStart, m_FalloffEnd, m_SpotPower,				// 감쇄 관련
		m_CastShadow,											// 그림자 사용 여부
		m_IsTextured, m_LightTextureIndex))						// 텍스쳐 라이트 및 텍스쳐 인덱스
	{
		if (m_IsTextured == true)
		{
			m_pRenderer->LightTextureMap(m_LightTextureIndex);
			Managers::GetInstance()->GetComponentSystem()->LightPreRender();
		}

		if (m_CastShadow == true)
		{
			m_pRenderer->BeginShadow();
			Managers::GetInstance()->GetComponentSystem()->LightPreRender();
		}
	}

}

void Light::OnRender()
{


}


_DLL void Light::SetPostion(const DirectX::SimpleMath::Vector3& pos)
{
	m_pMyTransform->SetPosition(pos);
}

_DLL void Light::SetRotation(const DirectX::SimpleMath::Vector3& axis, float rotVal)
{
	m_pMyTransform->SetRotation(DirectX::SimpleMath::Quaternion(axis, rotVal));
}

_DLL void Light::SetColor(const DirectX::SimpleMath::Vector3& color)
{
	m_Color = color;
}

_DLL void Light::SetColor(const float& r, const float& g, const float& b)
{
	m_Color.x = r;
	m_Color.y = g;
	m_Color.z = b;
}

_DLL void Light::SetIntensity(float val)
{
	m_Intensity = val;
}

_DLL void Light::SetCastShadow(bool val)
{
	m_CastShadow = val;
}

const DirectX::SimpleMath::Matrix& Light::GetTransform()
{
	return m_pMyTransform->GetWorld();
}

const DirectX::SimpleMath::Vector3& Light::GetPostion()
{
	return m_pMyTransform->GetLocalPosition();
}

const DirectX::SimpleMath::Quaternion& Light::GetRotation()
{
	return m_pMyTransform->GetRotation();
}

const DirectX::SimpleMath::Vector3& Light::GetColor()
{
	return m_Color;
}

float Light::GetIntensity()
{
	return m_Intensity;
}

_DLL bool Light::GetCastShadow()
{
	return m_CastShadow;
}

const eLIGHT_TYPE& Light::GetLightType()
{
	return m_eLightType;
}

_DLL void Light::SetFalloffStart(float value)
{
	m_FalloffStart = value;
}

_DLL void Light::SetFalloffEnd(float value)
{
	m_FalloffEnd = value;
}

_DLL void Light::SetSpotPower(float value)
{
	m_SpotPower = value;
}

_DLL float Light::GetFalloffStart()
{
	return m_FalloffStart;
}

_DLL float Light::GetFalloffEnd()
{
	return m_FalloffEnd;
}

_DLL float Light::GetSpotPower()
{
	return m_SpotPower;
}

void Light::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	m_LightTextureIndex = pReosureManager->GetTextureIndex(m_LightTextureName);
}

void Light::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_eLightType = static_cast<int>(m_eLightType);
	m_SaveData->m_CastShadow = m_CastShadow;
	m_SaveData->m_IsTextured = m_IsTextured;

	m_SaveData->m_Intensity = m_Intensity;
	m_SaveData->m_Color = &m_Color;

	m_SaveData->m_FalloffStart = m_FalloffStart;
	m_SaveData->m_FalloffEnd = m_FalloffEnd;
	m_SaveData->m_SpotPower = m_SpotPower;
	m_SaveData->m_TextureName = m_LightTextureName;


	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Light::LoadData()
{
	m_SaveData->m_Color = &m_Color;
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);

	m_eLightType = static_cast<decltype(m_eLightType)>(m_SaveData->m_eLightType);
	m_CastShadow = m_SaveData->m_CastShadow;
	m_IsTextured = m_SaveData->m_IsTextured;

	m_Intensity = m_SaveData->m_Intensity;

	m_FalloffStart = m_SaveData->m_FalloffStart;
	m_FalloffEnd = m_SaveData->m_FalloffEnd;
	m_SpotPower = m_SaveData->m_SpotPower;

	m_LightTextureName = m_SaveData->m_TextureName;
}
