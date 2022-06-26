#pragma once

#include "SceneBase.h"

class CLAnimation final : public SceneBase
{
public:
	CLAnimation();
	virtual ~CLAnimation();
	CLAnimation(const CLAnimation& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;
	//virtual void Render() override;


private:
	GameObject* m_pGameManager;

	GameObject* m_pAxis;
	GameObject* m_pDirectionalLight;

	GameObject* m_pGround;

	GameObject* m_pPlayer;
	GameObject* m_pPlayerMesh;
	GameObject* m_pCameraParent;
	GameObject* m_pMainCamera;
	GameObject* m_pZoomInPosition;

	GameObject* m_FreeCamera;

	GameObject* m_pTestCube_1;
	GameObject* m_pTestCube_2;

	GameObject* m_pWayPoint_1;
	GameObject* m_pWayPoint_2;
	GameObject* m_pWayPoint_3;
	GameObject* m_pWayPoint_4;

	GameObject* m_pZombie_Runner_1;
	GameObject* m_pMonster_1_Mesh;
	GameObject* m_pZombie_Runner_2;
	GameObject* m_pMonster_2_Mesh;

	/// UI Object
	// Text
	GameObject* m_txtMonster_1_Name;
	GameObject* m_txtMonster_1_State;

	// Sprite
	GameObject* m_sprDot;

	// Button
	GameObject* m_Btn_1;

};

