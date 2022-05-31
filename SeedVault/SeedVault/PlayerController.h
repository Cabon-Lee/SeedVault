#pragma once

/// <summary>
/// �÷��̾ ��Ʈ�� �ϴ� Ŭ����
/// 
/// �⺻���� Ű �Է����� �̵��� �ϰ�
/// ���콺 �̵����� ���� �̵�(ȸ��)�� �Ѵ�.
/// 
/// �÷��̾�� ����, �ɱ� ���� ���¿�
/// �ȱ�, �޸��� ��� ���� �ִ�.
/// 
/// �ۼ��� : YoKing
/// 
/// </summary>

#include "ComponentBase.h"
struct PlayerController_Save;

enum class PoseMode
{
	Stand,		// ��   ����
	Crouch,		// ���� ����
	Max,
};

enum class MovingMode
{
	Idle,		// idle
	Walking,	// �ȱ�   ���
	Sprint,		// �޸��� ���
	Max,
};

class PlayerController : public ComponentBase
{
public:
	/// ���� �Է� ����
	// Axis ����
	// �ִϸ����Ϳ����� ������ ����.(�ϴ� ������ ���ϰ� �θ������� public���� ����...)
	static float s_HForAnim;			// Horizontal Axis	�ִϸ��̼� ��ȯ�� ���
	static float s_VForAnim;			// Vertical Axis	
	static float s_H;					// Horizontal Axis	������ �̵��� ���(�ִϸ����Ϳ��� idle ���� �����ؼ� ��뵵 ��)
	static float s_V;					// Vertical Axis

	static bool  s_bCrouch;				// �ɱ�
	static bool  s_bSprint;				// �޸���
	static bool  s_bAim;				// ����(����)

	// �÷��̾� �ִϸ��̼� Ʈ�������� ����
	static bool  s_bIsReloading;		// ���� ���� �����ΰ� �Ǵ�
	static bool  s_bIsThrowing;			// ��ô ���� �����ΰ� �Ǵ�
	static bool  s_bIsHealing;			// ��(�ֻ�) ���� �����ΰ� �Ǵ�

	static bool  s_bIsSwaping;			// ��� ��ü ���� �����ΰ� �Ǵ�
	static uint  s_NextSlotNum;			// ��ü�� ��� ����

	static float s_PitchValue;			// �÷��̾� �Ž��� ��ü ������ ���ϱ� ���� ��(�ѱ�, ���� ����� ����)

	static bool s_bIsDie;				// �÷��̾� ��� ����(�������� �ִ� ����)
	static bool s_bIsDead;				// ������ ���� �ִ� ����

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
	

	// ���� �Է� ������Ʈ
	void UpdateUserInput();

	// ���� ���� ������Ʈ
	void UpdateAlive();

	// WASD �Է��� ���� �̵�
	void Move();
	void MoveByTransform();
	void MoveByPhysX();

	// �ڼ��� �����ϰ� ����� �ڼ��� ����
	PoseMode SetPoseMode(PoseMode pose);

	// ������ ���
	bool UseEquipedItem() const;

	// �ֹ��� (1��) ����
	bool EquipMainSlot();

	// ���ü ��Ÿ�� ��ٿ�
	void SwapCoolDown();

	// ����
	void Reload() const;

private:
	// ���� �����Ѱ�?
	bool CanAim() const;

	// ��� ���� �����Ѱ�?
	bool CanSwap() const;

	// �ƹ� �׼�(����)�� ���ϰ� �ִ°�?
	bool IsNoAction() const;

	void DebugRender();			// ��������� �� ������ ������ �����Ѵ�.

private:
	class Health* m_Health;			// Health Component
	class Inventory* m_Inventory;	// Inventory
	class Animator* m_Animator;
	class PhysicsActor* m_MyPhysicsActor;

	class GameObject* m_PlayerMesh;

	// �÷��̾� ���� ���
	PoseMode m_PoseMode;
	MovingMode m_MovingMode;

	// �ӵ�
	float m_StandSpeed;				// ���ִ� ������ �̵��ӵ�
	float m_CrouchSpeed;			// �ɾ� ���� �� �̵� �ӵ�
	float m_SprintSpeed;			// �޸��� �ӵ�
	float m_MoveSpeed;				// ���� �̵��ӵ�
	float m_SwapSpeed;				// ��� ��ü �ӵ�
	float m_SwapCoolTimer;			// ��� ��ü Ÿ�̸�

	Transform* m_CameraParent;		// ī�޶� �θ�(�ǹ�)

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

	float m_StandSpeed;				// ���ִ� ������ �̵��ӵ�
	float m_CrouchSpeed;			// �ɾ� ���� �� �̵� �ӵ�
	float m_SprintSpeed;			// �޸��� �ӵ�
	float m_MoveSpeed;				// ���� �̵��ӵ�

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
	
	m_StandSpeed,			// ���ִ� ������ �̵��ӵ�
	m_CrouchSpeed,			// �ɾ� ���� �� �̵� �ӵ�
	m_SprintSpeed,			// �޸��� �ӵ�
	m_MoveSpeed,			// ���� �̵��ӵ�
	
	m_CameraParent,
	m_LastHitTarget
	))
