#pragma once

/// <summary>
/// 플레이어를 컨트롤 하는 클래스
/// 
/// 기본적인 키 입력으로 이동을 하고
/// 마우스 이동으로 시점 이동(회전)을 한다.
/// 
/// 플레이어는 서기, 앉기 등의 상태와
/// 걷기, 달리기 모드 등이 있다.
/// 
/// 작성자 : YoKing
/// 
/// </summary>
#pragma region UpdateLog
// 2021-11-04
// 키입력에 따른 서기/걷기 모드 변환, 이동속도 조절 기능 추가

// 2021-11-15
// 테스트 카메라 줌 인/아웃 함수 구현(FindObject 가 안되서 현재 this와 임의의 Position을 기준으로 줌 인/아웃 함
//
// UpdateUserInput() 함수 추가
// 유저의 인풋을 받아서 각 상태 변수에 반영하도록 만듦.(현재는 SetPoseMode() 도 안에서 같이 해줌)
//
// 회전 로직 수정
// Rotate() -> ManualRotation() 로 이름과 내용 수정
// ConstrainAngles() 추가(아직 테스트 안해봄 테스트 및 디버깅 필요)

// 2021-11-17
// ConstrainAngles() 축 회전 각도 제한 기능 구현
// std::clamp 함수를 사용해서 구현해봤지만 동작하지 않았음..
// 그래서 로우하게 if() 문 이용해서 구현된 상태이다..
// 
// DLLEngine::FindGameObjectByName() 테스트
// 카메라 찾아서 가져와 봤는데 잘 된다!
// (앞으로Zoom In/Out 코드 개선 필요!)

// 2021-11-22
// 애니메이터 연결해서 State, Transition, parameter 추가 하는 코드 추가

// 2021-11-23
// 유저 키 입력 부분이랑 Move() 함수 로직 변경
// 입력 부분을 Move() 에서 처리하던 것을 UpdateUiserInput() 으로 옮기고
// Move() 에서는 이동만 함
//
// 카메라 행동관련(줌 인/아웃) 제거(CameraBehavior 로 이동)

// 2021-12-14
// 애니메이션 전환 시에 역방향(전 <-> 후 / 좌 <-> 우)간 이동 시 idle을 거쳐가서 
// 보기 불편하게 튀던 현상을 해결하기 위해 
// m_HForAnim , m_VForAnim 변수 추가하고 GetAxis() 로 업데이트 하게 해줌.

// 2021-12-20
// 플레이어의 각종 변수들을 static 으로 변환(m_H, m_V 등)
// 플레이어는 유일하다고 가정
// 다른 스크립트(코드)들에서 플레이어의 상태들을 참조하는 경우가 많은데 편리하게 하기 위함

#pragma endregion  UpdateLog

#include "ComponentBase.h"
struct PlayerController_Save;

enum class PoseMode
{
	Stand,		// 선   상태
	Crouch,		// 앉은 상태
	Max,
};

enum class MovingMode
{
	Idle,		// idle
	Walking,	// 걷기   모드
	Sprint,		// 달리기 모드
	Max,
};

class PlayerController : public ComponentBase
{
public:
	/// 유저 입력 관리
	// Axis 관련
	// 애니메이터에서도 가져다 쓴다.(일단 씬에서 편하게 부르기위해 public으로 변경...)
	static float s_HForAnim;			// Horizontal Axis	애니메이션 전환에 사용
	static float s_VForAnim;			// Vertical Axis	
	static float s_H;					// Horizontal Axis	포지션 이동에 사용(애니메이터에서 idle 상태 관련해서 사용도 함)
	static float s_V;					// Vertical Axis

	static bool  s_bCrouch;				// 앉기
	static bool  s_bSprint;				// 달리기
	static bool  s_bAim;				// 에임(조준)

	static bool  s_bIsAttack;			// 공격 중인 상태인가 판단
	static float s_AttackSpeed;			// 공격 속도
	static float s_AttackCollTimeTimer;	// 공격 쿨타임
	static float s_AttackRange;			// 공격 범위(거리)

	static float s_PitchValue;			// 플레이어 매쉬의 상체 방향을 정하기 위한 값(총구, 손을 항햐는 방향)

	static bool s_bSwaping;				// 무기 교체중인가 판별
	static float s_SwapTime;			// 무기교체 시간

public:
	PlayerController();
	virtual ~PlayerController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// 유저 입력 업데이트
	void UpdateUserInput();

	// WASD 입력을 통한 이동
	void Move();

	// 자세를 변경하고 변경된 자세를 리턴
	PoseMode SetPoseMode(PoseMode pose);

	// 플레이어 회전 관련
	void ResetRotation();

	bool IsCanAttack() const;	// 공격 가능한가 판단
	void Attack();				// 공격 (현재 장비한 무기로 공격)
	void CoolDown();			// 공격 쿨타임 돌리기

private:
	void DebugRender();			// 디버깅모드일 때 렌더할 대상들을 렌더한다.

private:
	Animator* m_Animator;

	// 플레이어 상태 모드
	PoseMode m_PoseMode;
	MovingMode m_MovingMode;

	// 속도
	float m_StandSpeed;				// 서있는 상태의 이동속도
	float m_CrouchSpeed;			// 앉아 있을 때 이동 속도
	float m_SprintSpeed;			// 달리기 속도
	float m_MoveSpeed;				// 현재 이동속도

	Transform* m_CameraParent;		// 카메라 부모(피벗)
	float m_RotationSpeed;			// 회전 속도

	/// Debug 정보
	GameObject* m_LastHitTarget;		// 공격한 obj 확인하기 위해

public:
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

	PlayerController_Save* m_SaveData;

};

struct PlayerController_Save
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;

	float s_HForAnim;
	float s_VForAnim;
	float s_H;
	float s_V;
	bool  s_bCrouch;
	bool  s_bSprint;
	bool  s_bAim;

	uint  m_Animator;
	int   m_PoseMode;
	int	  m_MovingMode;

	float m_StandSpeed;				// 서있는 상태의 이동속도
	float m_CrouchSpeed;			// 앉아 있을 때 이동 속도
	float m_SprintSpeed;			// 달리기 속도
	float m_MoveSpeed;				// 현재 이동속도

	uint m_CameraParent;
	uint m_LastHitTarget;

};

BOOST_DESCRIBE_STRUCT(PlayerController_Save, (), (
	m_bEnable,
	m_ComponentId,

	s_HForAnim,
	s_VForAnim,
	s_H,
	s_V,

	s_bCrouch,
	s_bSprint,
	s_bAim,
	
	m_Animator,
	m_PoseMode,
	m_MovingMode,
	
	m_StandSpeed,			// 서있는 상태의 이동속도
	m_CrouchSpeed,			// 앉아 있을 때 이동 속도
	m_SprintSpeed,			// 달리기 속도
	m_MoveSpeed,			// 현재 이동속도
	
	m_CameraParent,
	m_LastHitTarget
	))
