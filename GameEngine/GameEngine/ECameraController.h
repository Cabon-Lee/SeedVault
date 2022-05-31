#pragma once

/// <summary>
/// �ӽÿ�
/// </summary>

#include "ComponentBase.h"

struct ECameraController_Save;

/// Ű ���� ���� �ӽÿ�
namespace TEMPKEY
{
	enum class KeyCode : unsigned long
	{
		CLICK_LEFT = 0x01,
		CLICK_RIGHT = 0x02,

		KEY_LEFT = 0x25,
		KEY_UP = 0x26,
		KEY_RIGHT = 0x27,
		KEY_DOWN = 0x28,

		KEY_F1 = 0x70,
		KEY_F2 = 0x71,
		KEY_F3 = 0x72,
		KEY_F4 = 0x73,
		KEY_F5 = 0x74,
		KEY_F6 = 0x75,
		KEY_F7 = 0x76,
		KEY_F8 = 0x77,
		KEY_F9 = 0x78,
		KEY_F10 = 0x79,
		KEY_F11 = 0x7A,
		KEY_F12 = 0x7b,

		KEY_0 = 0x30,
		KEY_1 = 0x31,
		KEY_2 = 0x32,
		KEY_3 = 0x33,
		KEY_4 = 0x34,
		KEY_5 = 0x35,
		KEY_6 = 0x36,
		KEY_7 = 0x37,
		KEY_8 = 0x38,
		KEY_9 = 0x39,

		KEY_A = 0x41,
		KEY_B = 0x42,
		KEY_C = 0x43,
		KEY_D = 0x44,
		KEY_E = 0x45,
		KEY_F = 0x46,
		KEY_G = 0x47,
		KEY_H = 0x48,
		KEY_I = 0x49,
		KEY_J = 0x4A,
		KEY_K = 0x4B,
		KEY_L = 0x4C,
		KEY_M = 0x4D,
		KEY_N = 0x4E,
		KEY_O = 0x4F,
		KEY_P = 0x50,
		KEY_Q = 0x51,
		KEY_R = 0x52,
		KEY_S = 0x53,
		KEY_T = 0x54,
		KEY_U = 0x55,
		KEY_V = 0x56,
		KEY_W = 0x57,
		KEY_X = 0x58,
		KEY_Y = 0x59,
		KEY_Z = 0x5A,

		KEY_LSHIFT = 0xA0,
		KEY_LCONTROL = 0xA2,
		KEY_SPACE = 0x20,

		KEY_ESCAPE = 0x1B,
	};

	namespace KeyMap
	{
		/// <summary>
		/// ���ӿ��� ����� Ű ����
		/// ���� �������� ���� ���ǵ� ���� ����ϹǷ� static���� �����ߴ�.
		/// �Դٰ� static�� �ƴϸ� ���� ���Ͽ��� include �Ǹ鼭 
		/// ������ ���� �Ǿ��ٴ� ������ �Բ� �߰Եȴ�.
		/// 
		/// Setting ��ü �ϳ� �����ϰ� ������Ʈ, ���� �ϴ°͵� �������̱��ѵ� �ϴ� �Ǵ¼����� �Ѿ��...
		/// </summary>
		static unsigned long FORWARD = static_cast<unsigned long>(KeyCode::KEY_W);
		static unsigned long BACKWARDS = static_cast<unsigned long>(KeyCode::KEY_S);
		static unsigned long LEFT = static_cast<unsigned long>(KeyCode::KEY_A);
		static unsigned long RIGHT = static_cast<unsigned long>(KeyCode::KEY_D);
		static unsigned long SPRINT = static_cast<unsigned long>(KeyCode::KEY_LSHIFT);
		static unsigned long CROUCH = static_cast<unsigned long>(KeyCode::KEY_C);
		static unsigned long INTERATION = static_cast<unsigned long>(KeyCode::KEY_F);
		static unsigned long HANDSIGN = static_cast<unsigned long>(KeyCode::KEY_C);
		static unsigned long ATTACK = static_cast<unsigned long>(KeyCode::CLICK_LEFT);
		static unsigned long AIM = static_cast<unsigned long>(KeyCode::CLICK_RIGHT);

		//static unsigned long DEBUG = static_cast<unsigned long>(KeyCode::KEY_F1);

		static float MouseSensitivity = 0.15f;
	}

	namespace Window
	{
	}
}

enum class ECameraMovingMode
{
	Normal,			// �⺻ �̵� ���
	Acceleration,	// ���� ���
};

/// <summary>
/// ����ī�޶� ��Ʈ�ѷ�
/// </summary>

class ECameraController : public ComponentBase
{
public:
	ECameraController();
	~ECameraController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL void SetMovingVelocity(float speed);
	_DLL const float GetMovingVelocity();

	/// ���� ���� ��忡���� ī�޶� �̵�
	// ���� ��� üũ
	void UpdateMovingMode();

	// �����¿� �� ���� �̵�
	void Move();

	void Rotate();
	void SetRotationAngles(const Vector2 angles);
	void SetRotationAngles(const Vector3 angles);

private:
	ECameraMovingMode m_eMovingMode;		// ī�޶� �̵� ���

	float m_DefaultSpeed;			// �⺻ �̵��ӵ�
	const float m_AccelerationSpeed;	// ���ӵ�
	const float m_MaxSpeed;				// �ִ� �ӵ�
	float m_MoveSpeed;					// ���� �ӵ�

	DirectX::SimpleMath::Vector2 m_RotationAngles;	// ī�޶� ����(�� ������ Ʈ�������� rotation ����)
public:
	virtual void SaveData();
	virtual void LoadData();

	ECameraController_Save* m_SaveData;
};

struct ECameraController_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;
	
	int m_eMovingMode;

	DirectX::SimpleMath::Vector2* m_RotationAngles;
};

BOOST_DESCRIBE_STRUCT(ECameraController_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_eMovingMode,
	m_RotationAngles
	))