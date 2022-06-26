#pragma once

/// <summary>
/// NavMesh 테스트 씬
/// 
/// NavMesh 를 설정하고 Agent를 만들어서 충돌 판정, 목적지 설정, 경로탐색 등을 테스트해보자
/// 
/// 작성자 : 최 요 환
/// </summary>

#include "Scene.h"

class CLTestNavMeshScene final : public Scene
{
public:
	CLTestNavMeshScene();
	virtual ~CLTestNavMeshScene();
	CLTestNavMeshScene(const CLTestNavMeshScene& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;
	//virtual void Render() override;


private:
	GameObject* m_pAxis;
	GameObject* m_pDirectionalLight;

	GameObject* m_pGameManager;

	GameObject* m_pGround;
	
	GameObject* m_pPlayer;
	GameObject* m_RightHand;

	GameObject* m_FreeCamera;

	GameObject* m_pTargetObject_1;
	GameObject* m_pTargetObject_2;

	GameObject* m_InGameUI;
};

