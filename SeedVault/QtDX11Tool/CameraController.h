#pragma once

/// <summary>
/// 카메라를 제어(이동 및 회전)하는 클래스
/// -> 개발용 개발자 프리캠
/// 
/// 게임 내에서 카메라가 플레이어를 벗어나서
/// 자유시점 모드로 날아다닐 때가 있다.
/// 이를 위한 제어 클래스를 작성한다.
/// 
/// 작성자 : YoKIng
/// </summary>

#include "ComponentBase.h"

struct CameraController_Save;

enum class CameraMovingMode
{
	Normal,			// 기본 이동 모드
	Acceleration,	// 가속 모드
};

class CameraController : public ComponentBase
{
public:
	CameraController();
	~CameraController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	/// 자유 시점 모드에서의 카메라 이동
	// 무빙 모드 체크
	void UpdateMovingMode();

	// 전후좌우 및 상하 이동
	void Move();

	void Rotate();
	void SetRotationAngles(const Vector2 angles);
	void SetRotationAngles(const Vector3 angles);

private:
	CameraMovingMode m_eMovingMode;		// 카메라 이동 모드

	const float m_DefaultSpeed;			// 기본 이동속도
	const float m_AccelerationSpeed;	// 가속도
	const float m_MaxSpeed;				// 최대 속도
	float m_MoveSpeed;					// 현재 속도

	DirectX::SimpleMath::Vector2 m_RotationAngles;	// 카메라 각도(이 각도로 트랜스폼의 rotation 세팅)
public:
	virtual void SaveData();
	virtual void LoadData();

	CameraController_Save* m_SaveData;
};

struct CameraController_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;
	
	int m_eMovingMode;

	DirectX::SimpleMath::Vector2* m_RotationAngles;
};

BOOST_DESCRIBE_STRUCT(CameraController_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_eMovingMode,
	m_RotationAngles
	))