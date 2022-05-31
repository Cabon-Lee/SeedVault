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

	// 플레이어 애니메이션 트랜지션을 위해
	static bool  s_bIsReloading;		// 장전 중인 상태인가 판단
	static bool  s_bIsThrowing;			// 투척 중인 상태인가 판단
	static bool  s_bIsHealing;			// 힐(주사) 중인 상태인가 판단

	static bool  s_bIsSwaping;			// 장비 교체 중인 상태인가 판단
	static uint  s_NextSlotNum;			// 교체할 장비 슬롯

	static float s_PitchValue;			// 플레이어 매쉬의 상체 방향을 정하기 위한 값(총구, 손을 항햐는 방향)

	static bool s_bIsDie;				// 플레이어 사망 여부(쓰러지고 있는 상태)
	static bool s_bIsDead;				// 완전히 누워 있는 상태

	// test func
	static void FinishDie();
	static bool IsDead();

public:
	PlayerController();
	virtual ~PlayerController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	
	virtual void OnCollisionEnter(Collision collision) override;
	virtual void OnCollisionStay(Collision collision) override;
	virtual void OnCollisionExit(Collision collision) override;
	

	// 유저 입력 업데이트
	void UpdateUserInput();

	// 생존 여부 업데이트
	void UpdateAlive();

	// WASD 입력을 통한 이동
	void Move();
	void MoveByTransform();
	void MoveByPhysX();

	// 자세를 변경하고 변경된 자세를 리턴
	PoseMode SetPoseMode(PoseMode pose);

	// 아이템 사용
	bool UseEquipedItem() const;

	// 주무기 (1번) 장착
	bool EquipMainSlot();

	// 장비교체 쿨타임 쿨다운
	void SwapCoolDown();

	// 장전
	void Reload() const;

private:
	// 조준 가능한가?
	bool CanAim() const;

	// 장비 스왑 가능한가?
	bool CanSwap() const;

	// 아무 액션(동작)도 안하고 있는가?
	bool IsNoAction() const;

	void DebugRender();			// 디버깅모드일 때 렌더할 대상들을 렌더한다.

private:
	class Health* m_Health;			// Health Component
	class Inventory* m_Inventory;	// Inventory
	class Animator* m_Animator;
	class PhysicsActor* m_MyPhysicsActor;

	class GameObject* m_PlayerMesh;

	// 플레이어 상태 모드
	PoseMode m_PoseMode;
	MovingMode m_MovingMode;

	// 속도
	float m_StandSpeed;				// 서있는 상태의 이동속도
	float m_CrouchSpeed;			// 앉아 있을 때 이동 속도
	float m_SprintSpeed;			// 달리기 속도
	float m_MoveSpeed;				// 현재 이동속도
	float m_SwapSpeed;				// 장비 교체 속도
	float m_SwapCoolTimer;			// 장비 교체 타이머

	Transform* m_CameraParent;		// 카메라 부모(피벗)

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
