#pragma once
#include "SceneBase.h"

class QtScene00 : public SceneBase
{
public:
	QtScene00();
	virtual ~QtScene00();

public:
	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;


public:
	virtual void Load();
};

