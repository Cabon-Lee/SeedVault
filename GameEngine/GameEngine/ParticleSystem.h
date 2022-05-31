#pragma once
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "MeshRenderer.h"
#include "ResourceObserver.h"
#include "Animation.h"

__interface IRenderer;
__interface IResourceManager;

struct ParticleSystem_Save;

enum class eParticleKind
{
	Blood = 0,
	Smoke, 
	MuzzleFlash, 
	Spark
};

/// <summary>
/// 파티클 컴포넌트
/// 
/// 2021.12.29 B.Bunny
/// </summary>
class ParticleSpawner : public ComponentBase, public ResourceObserver
{
public:
	_DLL ParticleSpawner();
	_DLL virtual ~ParticleSpawner();

	virtual void Start() override;
	virtual void Update(float dTime) override {} ;
	virtual void OnRender() override;

	eANIM_FRAMERATE m_AnimationFrameRate;


	_DLL void SetSpriteIndex(const unsigned int idx);
	_DLL void AddSpriteName(const std::string&& name);

	const DirectX::XMFLOAT3& GetDirection();
	const float GetVelocity();
	const DirectX::XMFLOAT2& GetSize();
	const float GetRandomDirWeight();
	const float GetBirthTime();
	const float GetDeadTime();

	_DLL void SetDirection(const DirectX::XMFLOAT3& dir);
	_DLL void SetPosition(const DirectX::XMFLOAT3& pos);
	_DLL void SetDirection(float* pDir);
	_DLL void SetPosition(float* pPos);
	_DLL void SetSize(const DirectX::XMFLOAT2& size);
	_DLL void SetSize(float* pSize);
	_DLL void SetVelocity(float val);
	_DLL void SetRandomDirWeight(const float val);
	_DLL void SetBirthTime(const float val);
	_DLL void SetDeadTime(const float val);
	_DLL void SetActive(bool val);

	_DLL void SetRotation(float val);
	_DLL void SetForce(float val);
	_DLL void SetDeltaAlpha(float val);
	_DLL void SetDeltaVelocity(float val);
	_DLL void SetDeltaSize(float val);
	_DLL void SetRandomSizeWeight(float val);
	_DLL void SetDeltaRotation(float val);
	_DLL void SetRandowmRotWeight(float val);
	_DLL void SetStopTime(float val);
	_DLL void SetParticleCount(UINT val);

	_DLL float GetRotation();
	_DLL float GetForce();
	_DLL float GetDeltaAlpha();
	_DLL float GetDeltaVelocity();
	_DLL float GetDeltaSize();
	_DLL float GetRandomSizeWeight();
	_DLL float GetDeltaRotation();
	_DLL float GetRandowmRotWeight();
	_DLL float GetStopTime();
	_DLL UINT GetParticleCount();

private:
	bool m_IsActive;

	DirectX::XMFLOAT3 m_Direction;
	DirectX::XMFLOAT3 m_Location;
	float m_Velocity;
	DirectX::XMFLOAT2 m_Size;
	float m_RandomWeight;
	float m_BirthTime;
	float m_DeadTime;

	std::shared_ptr<IRenderer> m_pRenderer;
	std::unique_ptr<struct ParticleProperty> m_pParticleProperty;

	std::vector<unsigned int> m_SpriteIndex_V;
	unsigned int m_SpriteIndex;

	eParticleKind m_eParticleKind;

	float m_TimeStack;
	unsigned int m_NowKeyFrame;
	unsigned int m_LastKeyFrame;

	unsigned int m_CurrSpriteIndex;
	std::vector<std::string> m_SpriteName_V;
	std::vector<unsigned int> m_ParticleIndex_V;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	ParticleSystem_Save* m_SaveData;

};
