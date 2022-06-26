#pragma once

/// <summary>
/// 기획자가 작업을 위한 씬
/// (배치, 확인 등)
///
/// 기본 밑작업은 프로그래머가 준비하고 (오브젝트 생성 등 -> 당장 실행시 Create가 안되므로..)
/// 기획자가 오브젝트의 메쉬 설정과 트랜스폼(위치, 회전, 크기)등을 설정해 볼 수 있도록 한다.
/// 
/// 작성자 최 요 환
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

