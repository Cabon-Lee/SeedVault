#include "pch.h"
#include "MuzzleFlash.h"

MuzzleFlash::MuzzleFlash()
	: ComponentBase(ComponentType::GameLogic)
{

}

MuzzleFlash::~MuzzleFlash()
{
	
}

void MuzzleFlash::Start()
{
	m_MyParicleSpawer = m_pMyObject->GetComponent<ParticleSpawner>();
	m_MyLight = m_pMyObject->GetComponent<Light>();

	m_MyLight->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.5f, 0.0f));
	m_MyLight->SetIntensity(1.f);
	m_MyLight->SetFalloffStart(1.0f);
	m_MyLight->SetFalloffEnd(2.0f);
	m_MyLight->m_eLightType = eLIGHT_TYPE::POINT;
	m_MyLight->m_CastShadow = false;


	m_MyParicleSpawer->SetRandomDirWeight(0.0f);
	m_MyParicleSpawer->SetVelocity(1.0f);
	
}

void MuzzleFlash::Update(float dTime)
{
	if (m_LightTime <= 0.0f)
	{
		m_MyLight->SetEnable(false);
	}
	else
	{
		m_MyLight->SetEnable(true);
	}

	m_LightTime -= dTime;
	if (m_LightTime < 0) m_LightTime = 0;
}

void MuzzleFlash::OnRender()
{

}



void MuzzleFlash::Fire()
{
	m_LightTime = m_LightContinueTime;

	// 랜덤으로 제공
	m_MyParicleSpawer->SetActive(true);
	m_MyParicleSpawer->SetPosition(m_Transform->GetWorldPosition());
	m_MyParicleSpawer->SetBirthTime(0.00f);
	m_MyParicleSpawer->SetStopTime(0.05f);
	m_MyParicleSpawer->SetDeadTime(0.1f);
	
	m_MyParicleSpawer->SetRotation(static_cast<float>(rand() % 10));
	float _size = 0.05f * (1 - static_cast<float>(rand() % 3));
	float _random[2] = { _size, _size };
	m_MyParicleSpawer->SetSize(_random);
	m_MyParicleSpawer->SetDeltaSize(0.8f);

}

