#include "pch.h"
#include "ParticleSystem.h"
#include "EngineDLL.h"
#include "UIData.h"

#include "IRenderer.h"
#include "IResourceManager.h"

ParticleSpawner::ParticleSpawner()
	: ComponentBase(ComponentType::Rendering)
{
	m_pParticleProperty = std::make_unique<ParticleProperty>();
	m_Direction.y = 0.1f;
	m_pParticleProperty->emitDir = new DirectX::XMFLOAT3();
	m_pParticleProperty->emitterPos = new DirectX::XMFLOAT3();
	m_pParticleProperty->Particlesize = new DirectX::XMFLOAT2(1.0f, 1.0f);
	m_pParticleProperty->deadTime = 0.1f;
	m_Velocity = 1.0f;
	m_CurrSpriteIndex = 0;

	m_pParticleProperty->particeCount = 1;
	m_pParticleProperty->stopTime = 0.1f;
}

ParticleSpawner::~ParticleSpawner()
{
	delete(m_pParticleProperty->emitDir);
	delete(m_pParticleProperty->emitterPos);
	delete(m_pParticleProperty->Particlesize);

	m_pParticleProperty.reset();
}

void ParticleSpawner::Start()
{
	m_pRenderer = DLLEngine::GetEngine()->GetIRenderer();

}

void ParticleSpawner::OnRender()
{
	if (m_IsActive == true)
	{

		*m_pParticleProperty->emitDir = m_Direction * m_Velocity;
		m_pParticleProperty->textureIndex = m_ParticleIndex_V[m_CurrSpriteIndex];
		m_pRenderer->SpawnParticle(m_pParticleProperty.get());

		m_IsActive = false;
	}
}

void ParticleSpawner::SetSpriteIndex(const unsigned int idx)
{
	m_CurrSpriteIndex = idx;
}

void ParticleSpawner::AddSpriteName(const std::string&& name)
{
	m_SpriteName_V.push_back(name);
}

const DirectX::XMFLOAT3& ParticleSpawner::GetDirection()
{	return m_Direction;	}

const float ParticleSpawner::GetVelocity()
{
	return m_Velocity;
}

const DirectX::XMFLOAT2& ParticleSpawner::GetSize()
{
	return *m_pParticleProperty->Particlesize;
}

const float ParticleSpawner::GetRandomDirWeight()
{
	return m_pParticleProperty->dirRandomWeight;
}

const float ParticleSpawner::GetBirthTime()
{
	return m_pParticleProperty->birthTime;
}

const float ParticleSpawner::GetDeadTime()
{
	return m_pParticleProperty->deadTime;
}

void ParticleSpawner::SetDirection(const DirectX::XMFLOAT3& dir)
{
	m_Direction = dir;
}

_DLL void ParticleSpawner::SetDirection(float* pDir)
{
	m_Direction.x = pDir[0]; m_Direction.y = pDir[1]; m_Direction.z = pDir[2];
}

_DLL void ParticleSpawner::SetPosition(float* pPos)
{
	m_pParticleProperty->emitterPos->x = pPos[0];
	m_pParticleProperty->emitterPos->y = pPos[1];
	m_pParticleProperty->emitterPos->z = pPos[2];
}

void ParticleSpawner::SetPosition(const DirectX::XMFLOAT3& pos)
{
	*m_pParticleProperty->emitterPos = pos;
}

void ParticleSpawner::SetVelocity(float val)
{
	m_Velocity = val;
}

void ParticleSpawner::SetSize(const DirectX::XMFLOAT2& size)
{
	*m_pParticleProperty->Particlesize = size;
}

_DLL void ParticleSpawner::SetSize(float* pSize)
{
	m_pParticleProperty->Particlesize->x = pSize[0];
	m_pParticleProperty->Particlesize->y = pSize[1];
}

void ParticleSpawner::SetRandomDirWeight(const float val)
{
	m_pParticleProperty->dirRandomWeight = val;
}

void ParticleSpawner::SetBirthTime(const float val)
{
	m_pParticleProperty->birthTime = val;
}

void ParticleSpawner::SetDeadTime(const float val)
{
	m_pParticleProperty->deadTime = val;
}

void ParticleSpawner::SetActive(bool val)
{
	m_IsActive = val;
}

_DLL void ParticleSpawner::SetRotation(float val)
{
	m_pParticleProperty->constZrot = val;
}

_DLL void ParticleSpawner::SetForce(float val)
{
	m_pParticleProperty->constForce = val;
}

_DLL void ParticleSpawner::SetDeltaAlpha(float val)
{
	m_pParticleProperty->deltaAlpha = val;
}

_DLL void ParticleSpawner::SetDeltaVelocity(float val)
{
	m_pParticleProperty->deltaVelo = val;
}

_DLL void ParticleSpawner::SetDeltaSize(float val)
{
	m_pParticleProperty->deltaSize = val;
}

_DLL void ParticleSpawner::SetRandomSizeWeight(float val)
{
	m_pParticleProperty->deltaSizeWeight = val;
}

_DLL void ParticleSpawner::SetDeltaRotation(float val)
{
	m_pParticleProperty->deltaRot = val;
}

_DLL void ParticleSpawner::SetRandowmRotWeight(float val)
{
	m_pParticleProperty->deltaRotWeight = val;
}

_DLL void ParticleSpawner::SetStopTime(float val)
{
	m_pParticleProperty->stopTime = val;
}

_DLL void ParticleSpawner::SetParticleCount(UINT val)
{
	m_pParticleProperty->particeCount = val;
}

_DLL float ParticleSpawner::GetRotation()
{
	return m_pParticleProperty->constZrot;
}

_DLL float ParticleSpawner::GetForce()
{
	return m_pParticleProperty->constForce;
}

_DLL float ParticleSpawner::GetDeltaAlpha()
{
	return m_pParticleProperty->deltaAlpha;
}

_DLL float ParticleSpawner::GetDeltaVelocity()
{
	return m_pParticleProperty->deltaVelo;
}

_DLL float ParticleSpawner::GetDeltaSize()
{
	return m_pParticleProperty->deltaSize;
}

_DLL float ParticleSpawner::GetRandomSizeWeight()
{
	return m_pParticleProperty->deltaSizeWeight;
}

_DLL float ParticleSpawner::GetDeltaRotation()
{
	return m_pParticleProperty->deltaRot;
}

_DLL float ParticleSpawner::GetRandowmRotWeight()
{
	return m_pParticleProperty->deltaRotWeight;
}

_DLL float ParticleSpawner::GetStopTime()
{
	return m_pParticleProperty->stopTime;
}

_DLL UINT ParticleSpawner::GetParticleCount()
{
	return m_pParticleProperty->particeCount;
}

void ParticleSpawner::ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_ParticleIndex_V.resize(std::size(m_SpriteName_V));

	for (int i = 0; i < std::size(m_SpriteName_V); i++)
	{
		m_ParticleIndex_V[i] = pResourceManager->GetSpriteDataIndex(m_SpriteName_V[i]);
	}
}

void ParticleSpawner::SaveData()
{
}

void ParticleSpawner::LoadData()
{
}

