#pragma once

#include "ComponentBase.h"

class ModelController :
    public ComponentBase
{
public:
    ModelController();
    virtual ~ModelController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;


private:
	void UserInputUpdate();
};

