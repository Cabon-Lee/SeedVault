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
#pragma region UpdateLog
// 2021-11-04
// Ű�Է¿� ���� ����/�ȱ� ��� ��ȯ, �̵��ӵ� ���� ��� �߰�

// 2021-11-15
// �׽�Ʈ ī�޶� �� ��/�ƿ� �Լ� ����(FindObject �� �ȵǼ� ���� this�� ������ Position�� �������� �� ��/�ƿ� ��
//
// UpdateUserInput() �Լ� �߰�
// ������ ��ǲ�� �޾Ƽ� �� ���� ������ �ݿ��ϵ��� ����.(����� SetPoseMode() �� �ȿ��� ���� ����)
//
// ȸ�� ���� ����
// Rotate() -> ManualRotation() �� �̸��� ���� ����
// ConstrainAngles() �߰�(���� �׽�Ʈ ���غ� �׽�Ʈ �� ����� �ʿ�)

// 2021-11-17
// ConstrainAngles() �� ȸ�� ���� ���� ��� ����
// std::clamp �Լ��� ����ؼ� �����غ����� �������� �ʾ���..
// �׷��� �ο��ϰ� if() �� �̿��ؼ� ������ �����̴�..
// 
// DLLEngine::FindGameObjectByName() �׽�Ʈ
// ī�޶� ã�Ƽ� ������ �ôµ� �� �ȴ�!
// (������Zoom In/Out �ڵ� ���� �ʿ�!)

// 2021-11-22
// �ִϸ����� �����ؼ� State, Transition, parameter �߰� �ϴ� �ڵ� �߰�

// 2021-11-23
// ���� Ű �Է� �κ��̶� Move() �Լ� ���� ����
// �Է� �κ��� Move() ���� ó���ϴ� ���� UpdateUiserInput() ���� �ű��
// Move() ������ �̵��� ��
//
// ī�޶� �ൿ����(�� ��/�ƿ�) ����(CameraBehavior �� �̵�)

// 2021-12-14
// �ִϸ��̼� ��ȯ �ÿ� ������(�� <-> �� / �� <-> ��)�� �̵� �� idle�� ���İ��� 
// ���� �����ϰ� Ƣ�� ������ �ذ��ϱ� ���� 
// m_HForAnim , m_VForAnim ���� �߰��ϰ� GetAxis() �� ������Ʈ �ϰ� ����.

// 2021-12-20
// �÷��̾��� ���� �������� static ���� ��ȯ(m_H, m_V ��)
// �÷��̾�� �����ϴٰ� ����
// �ٸ� ��ũ��Ʈ(�ڵ�)�鿡�� �÷��̾��� ���µ��� �����ϴ� ��찡 ������ ���ϰ� �ϱ� ����

#pragma endregion  UpdateLog

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

	static bool  s_bIsAttack;			// ���� ���� �����ΰ� �Ǵ�
	static float s_AttackSpeed;			// ���� �ӵ�
	static float s_AttackCollTimeTimer;	// ���� ��Ÿ��
	static float s_AttackRange;			// ���� ����(�Ÿ�)

	static float s_PitchValue;			// �÷��̾� �Ž��� ��ü ������ ���ϱ� ���� ��(�ѱ�, ���� ����� ����)

	static bool s_bSwaping;				// ���� ��ü���ΰ� �Ǻ�
	static float s_SwapTime;			// ���ⱳü �ð�

public:
	PlayerController();
	virtual ~PlayerController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// ���� �Է� ������Ʈ
	void UpdateUserInput();

	// WASD �Է��� ���� �̵�
	void Move();

	// �ڼ��� �����ϰ� ����� �ڼ��� ����
	PoseMode SetPoseMode(PoseMode pose);

	// �÷��̾� ȸ�� ����
	void ResetRotation();

	bool IsCanAttack() const;	// ���� �����Ѱ� �Ǵ�
	void Attack();				// ���� (���� ����� ����� ����)
	void CoolDown();			// ���� ��Ÿ�� ������

private:
	void DebugRender();			// ��������� �� ������ ������ �����Ѵ�.

private:
	Animator* m_Animator;

	// �÷��̾� ���� ���
	PoseMode m_PoseMode;
	MovingMode m_MovingMode;

	// �ӵ�
	float m_StandSpeed;				// ���ִ� ������ �̵��ӵ�
	float m_CrouchSpeed;			// �ɾ� ���� �� �̵� �ӵ�
	float m_SprintSpeed;			// �޸��� �ӵ�
	float m_MoveSpeed;				// ���� �̵��ӵ�

	Transform* m_CameraParent;		// ī�޶� �θ�(�ǹ�)
	float m_RotationSpeed;			// ȸ�� �ӵ�

	/// Debug ����
	GameObject* m_LastHitTarget;		// ������ obj Ȯ���ϱ� ����

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
