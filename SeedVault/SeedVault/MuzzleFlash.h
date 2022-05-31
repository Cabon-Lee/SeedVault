#pragma once

#include "ComponentBase.h"

class MuzzleFlash : public ComponentBase
{
public:
	MuzzleFlash();
	virtual	~MuzzleFlash();
	MuzzleFlash(const MuzzleFlash&) = default;
	MuzzleFlash& operator= (const MuzzleFlash&) = default;
	MuzzleFlash(MuzzleFlash&&) = default;
	MuzzleFlash& operator= (MuzzleFlash&&) = default;

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void Fire();

private:

	class ParticleSpawner* m_MyParicleSpawer;
	class Light* m_MyLight;

	const float m_LightContinueTime = 0.05f;
	float m_LightTime;

	float m_NowElpsedTime;
};

