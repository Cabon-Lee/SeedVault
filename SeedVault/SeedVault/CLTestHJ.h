#pragma once
#include "SceneBase.h"
class CLTestHJ : public SceneBase
{
public:
	CLTestHJ();
	virtual ~CLTestHJ();

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;
};

