#pragma once

/// <summary>
/// 카메라의 궤도 이동 클래스
/// Input(키 or 마우스)에 따라서 target 주위를 공전 해야 한다.
/// 
/// 인게임에선 마우스 이동에 따라서 카메라가 궤도 이동 해야함
/// 
/// 작성자 : YoKing
/// 
/// </summary>

#include "ComponentBase.h"

class OrbitCamera final : public ComponentBase
{
public:
	OrbitCamera();
	~OrbitCamera();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

public:
	void SetFocusTrasform(Transform& targetTf);		// 초점(타겟) 설정
	void SetFocusTrasform(GameObject& targetObj);	// 초점(타겟) 설정
	void UpdateFocusPoint();						// 타겟 오브젝트의 Position 업데이트
	bool ManualRotation();							// 입력에 따른 회전(수동, 매뉴얼)

	void ConstrainAngles();

public:
	virtual void SaveData() override;
	virtual void LoadData() override;
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component) override;

	struct OrbitCamera_Save* m_SaveData;

private:
	Transform* m_FocusTarget;			// 초점(타겟 오브젝트)
	Vector3 m_FocusPosition;			// 초점(타겟) 포지션

	float m_Distance;					// 초점으로부터의 거리
	float m_RotationSpeed;				// 회전 속도

	Vector2 m_RotationAngles;			// 궤도 각
	
	// 제한각도
	float m_MaxVerticalAngle;			// x(pitch)
	float m_MinVerticalAngle;			// y(yaw)

};

struct OrbitCamera_Save
{
	bool m_bEnable;
	uint m_ComponentId;

	uint m_FocusTarget;

	DirectX::SimpleMath::Vector3* m_FocusPosition;

	float m_Distance;
	float m_RotationSpeed;

	DirectX::SimpleMath::Vector2* m_RotationAngles;

	float m_MaxVerticalAngle;
	float m_MinVerticalAngle;
};

BOOST_DESCRIBE_STRUCT(OrbitCamera_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_FocusTarget,
	
	m_FocusPosition,
	
	m_Distance,
	m_RotationSpeed,
	
	m_RotationAngles,

	m_MaxVerticalAngle,
	m_MinVerticalAngle
	))
