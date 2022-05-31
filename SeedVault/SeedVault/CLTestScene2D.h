#pragma once

#include "SceneBase.h"

class CLTestScene2D : public SceneBase
{
public:
	CLTestScene2D();
	virtual ~CLTestScene2D();
	CLTestScene2D(const CLTestScene2D& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

private:
	GameObject* m_pDirectionalLight;

	GameObject* m_Billboard;
	GameObject* m_Billboard2;
		
	std::vector<GameObject*> m_Billboard_V;
	std::vector<GameObject*> m_Billboard_V2;
	
	GameObject* m_Particle;
	GameObject* m_TestObject;
	
	GameObject* m_Sprite;
	GameObject* m_SpriteAni;

	GameObject* m_TransformAnimation;
	GameObject* m_DoorFrame;
	GameObject* m_Door1;
	GameObject* m_Door2;
	
	GameObject* m_Button;
	
	GameObject* m_D2DText;
	GameObject* m_D2DText2;

	GameObject* m_OIT;
	GameObject* m_OIT2;
	
	GameObject* m_pMainCamera;
};
