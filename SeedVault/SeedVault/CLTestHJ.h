#pragma once
#include "SceneBase.h"

class Audio;
class SoundEvent;
class CLTestHJ : public SceneBase
{
public:
	CLTestHJ();
	virtual ~CLTestHJ();

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;


	GameObject* m_Audio;
	GameObject* m_Cam;
	SoundEvent* _Event;
};

