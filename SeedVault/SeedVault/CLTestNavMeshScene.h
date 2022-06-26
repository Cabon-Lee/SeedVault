#pragma once

/// <summary>
/// NavMesh �׽�Ʈ ��
/// 
/// NavMesh �� �����ϰ� Agent�� ���� �浹 ����, ������ ����, ���Ž�� ���� �׽�Ʈ�غ���
/// 
/// �ۼ��� : �� �� ȯ
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

