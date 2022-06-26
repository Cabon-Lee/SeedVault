#pragma once

/// <summary>
/// 게임내 메인 카메라(플레이어 팔로잉)의 행동을 정의하는 클래스
/// 
/// 줌 인/아웃 및 카메라 충돌(Bounding)등 종합
/// 
/// 작성자 : 최 요 환
/// 
/// </summary>

#pragma region UpdateLog
// 2021-11-22
// 클래스 생성(만...)

// 2021-11-23
// 카메라 줌인/아웃 기능 구현(기존 PlayerContorller 스크립트에 있던것을 이식)

// 2021-12-09
// 플레이어 앉은상태에 따라 줌인/아웃 포지션 변경되게 수정
// Current/Stand/Crouch 로 변수 구분
#pragma endregion  UpdateLog

#include "ComponentBase.h"

class CameraBehavior : public ComponentBase
{
public:
	CameraBehavior();
	virtual ~CameraBehavior();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// 줌인/줌아웃 포지션 설정
	void SetZoomInPosition();

	// 조준중 카메라 줌 인/아웃
	void CameraZoomIn();
	void CameraZoomOut();


public:
	// Save/Load 용 함수
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

	struct CameraBehavior_Save* m_SaveData;

protected:

private:
	class PlayerController* m_PlayerController;	// 플레이어 컨트롤러

	Camera* m_Camera;					// 메인카메라의 카메라 컴포넌트

	Transform* m_ZoomInPosition;		// parent Transform

	float m_ZoomInFov;					// 줌인   시에 사용할 확대용 aspect
	float m_ZoomOutFov;					// 줌아웃 시에 사용할 aspect
	float m_CurrentFov;				

	float m_CameraSpeed;				// 카메라 이동속도
	Vector3 m_CurrentZoomInPos;			// 현재 설정된 줌인 포지션
	Vector3 m_CurrentCameraDefaultPos;	// 현재 설정된 디폴트 포지션

	Vector3* m_StandZoomInPos;			// 서있는 상태의 줌인 포지션(이 변수만 포인터로 ZoomInPosObject 를 가리키도록 한다)
	Vector3 m_StandCameraDefaultPos;	// 서있는 상태의 디폴트 포지션

	Vector3 m_CrouchZoomInPos;			// 앉은 상태의 줌인 포지션
	Vector3 m_CrouchCameraDefaultPos;	// 앉은 상태의 디폴트 포지션
};


struct CameraBehavior_Save
{
	bool		 m_bEnable;
	unsigned int m_ComponentId;

	unsigned int m_ZoomInPosition;

	float m_ZoomInFov;
	float m_ZoomOutFov;
	float m_CurrentFov;

	float m_CameraSpeed;

	DirectX::SimpleMath::Vector3* m_CurrentZoomInPos;
	DirectX::SimpleMath::Vector3* m_CurrentCameraDefaultPos;

	//DirectX::SimpleMath::Vector3* m_StandZoomInPos;
	DirectX::SimpleMath::Vector3* m_StandCameraDefaultPos;

	DirectX::SimpleMath::Vector3* m_CrouchZoomInPos;
	DirectX::SimpleMath::Vector3* m_CrouchCameraDefaultPos;

	unsigned int m_pParent;		///Parent ComponentID
	unsigned int m_pObject;		///Parent ObjectID
};

BOOST_DESCRIBE_STRUCT(CameraBehavior_Save, (), (
	m_bEnable,
	m_ComponentId,
	
	m_ZoomInPosition,

	m_ZoomInFov,
	m_ZoomOutFov,
	m_CurrentFov,

	m_CameraSpeed,

	m_CurrentZoomInPos,
	m_CurrentCameraDefaultPos,

	//m_StandZoomInPos,
	m_StandCameraDefaultPos,

	m_CrouchZoomInPos,
	m_CrouchCameraDefaultPos,

	m_pParent,
	m_pObject
	))


