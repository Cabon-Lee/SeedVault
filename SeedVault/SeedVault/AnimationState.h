#pragma once

#include "AnimationSystem.h"
#include "CATrace.h"

class Idle : public State 
{
	virtual void Update() override
	{
		CA_TRACE("Idle State");
		State::Update();
	}
};
