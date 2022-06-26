#pragma once

/// <summary>
/// �׽�Ʈ ��
/// ������ IScene �� ��� �޾� ������
/// 
/// �ϴ� ������Ʈ �ϳ� �����ϰ� �������ϰ� �����̴� ������ �����ؾ� �Ѵ�.
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

#include "Scene.h"

class CLTestSceneYH final : public Scene
{
public:
	CLTestSceneYH();
	virtual ~CLTestSceneYH();
	CLTestSceneYH(const CLTestSceneYH& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;
	//virtual void Render() override;


private:

	GameObject* m_pGameManager;

	GameObject* m_pAxis;
	
	GameObject* m_pNavMeshObject_1;
	GameObject* m_pNavMeshObject_2;

	GameObject* m_pGround;

	GameObject* m_pPlayer;
	GameObject* m_RightHand;

	GameObject* m_FreeCamera;

	GameObject* m_pPartner;

	GameObject* m_pTestCube_1;
	GameObject* m_pTestCube_2;

	GameObject* m_pZombie_Runner_1;
	GameObject* m_pZombie_Runner_2;
	GameObject* m_pMonster_3;

	/// UI Object
	GameObject* m_InGameUI;
	GameObject* m_PauseUI;

	// Test
	GameObject* m_TestCube;

	bool m_BGM_Playing;
};

