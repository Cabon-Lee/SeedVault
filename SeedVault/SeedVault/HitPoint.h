#pragma once

#include "ComponentBase.h"

class HitPoint : public ComponentBase
{
public:
	HitPoint();
	virtual ~HitPoint();
	HitPoint(const HitPoint&) = default;
	HitPoint& operator= (const HitPoint&) = default;
	HitPoint(HitPoint&&) = default;
	HitPoint& operator= (HitPoint&&) = default;

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	GameObject* GetHitPoint();


private:
	ParticleSpawner* m_MyParticleSpawner;
	DirectX::XMFLOAT3 m_CursorPostion;
	float m_DepthFromCamera;
	DirectX::XMFLOAT3 m_CursorNormal;
	unsigned int m_ObjectID;

	DirectX::XMFLOAT2 m_Size;

	unsigned int m_SparkParticle;
	unsigned int m_BloodParticle;

};

