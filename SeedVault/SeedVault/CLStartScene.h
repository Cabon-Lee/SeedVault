#pragma once
#include "Scene.h"

class CLStartScene final : public Scene
{
public:
	CLStartScene();
	virtual ~CLStartScene();
	CLStartScene(const CLStartScene& other) = default;
	
	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

private:

	GameObject* m_pBackGround;

	GameObject* m_StartImage;


	GameObject* m_StartButton;
	GameObject* m_ChapterChoiceButton;
	GameObject* m_OptionButton;
	GameObject* m_ExitButton;
	
	GameObject* m_pMainCamera;

};

