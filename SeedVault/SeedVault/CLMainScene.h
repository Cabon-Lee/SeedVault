#pragma once

/// <summary>
/// ��ȹ�ڰ� �۾��� ���� ��
/// (��ġ, Ȯ�� ��)
///
/// �⺻ ���۾��� ���α׷��Ӱ� �غ��ϰ� (������Ʈ ���� �� -> ���� ����� Create�� �ȵǹǷ�..)
/// ��ȹ�ڰ� ������Ʈ�� �޽� ������ Ʈ������(��ġ, ȸ��, ũ��)���� ������ �� �� �ֵ��� �Ѵ�.
/// 
/// �ۼ��� �� �� ȯ
/// 
/// </summary>

#include "SceneBase.h"

class CLMainScene : public SceneBase
{
public:
	CLMainScene();
	virtual ~CLMainScene();
	CLMainScene(const CLMainScene& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

private:
	GameObject* m_pGameManager;

	GameObject* m_pAxis;
	GameObject* m_pDirectionalLight;

	GameObject* m_pPlayer;
	GameObject* m_pPlayerMesh;
	GameObject* m_pCameraParent;
	GameObject* m_pMainCamera;
	GameObject* m_pZoomInPosition;

	GameObject* m_FreeCamera;

	/*
	GameObject* m_Object_1;
	GameObject* m_Object_2;
	GameObject* m_Object_3;
	GameObject* m_Object_4;
	GameObject* m_Object_5;
	GameObject* m_Object_6;
	GameObject* m_Object_7;
	*/
};

