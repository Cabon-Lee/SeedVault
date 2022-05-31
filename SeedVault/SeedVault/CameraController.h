#pragma once

/// <summary>
/// ī�޶� ����(�̵� �� ȸ��)�ϴ� Ŭ����
/// -> ���߿� ������ ����ķ
/// 
/// ���� ������ ī�޶� �÷��̾ �����
/// �������� ���� ���ƴٴ� ���� �ִ�.
/// �̸� ���� ���� Ŭ������ �ۼ��Ѵ�.
/// 
/// �ۼ��� : YoKIng
/// </summary>

#include "ComponentBase.h"

struct CameraController_Save;

enum class CameraMovingMode
{
	Normal,			// �⺻ �̵� ���
	Acceleration,	// ���� ���
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

	/// ���� ���� ��忡���� ī�޶� �̵�
	// ���� ��� üũ
	void UpdateMovingMode();

	// �����¿� �� ���� �̵�
	void Move();

	void Rotate();
	void SetRotationAngles(const Vector2 angles);
	void SetRotationAngles(const Vector3 angles);

private:
	CameraMovingMode m_eMovingMode;		// ī�޶� �̵� ���

	const float m_DefaultSpeed;			// �⺻ �̵��ӵ�
	const float m_AccelerationSpeed;	// ���ӵ�
	const float m_MaxSpeed;				// �ִ� �ӵ�
	float m_MoveSpeed;					// ���� �ӵ�

	DirectX::SimpleMath::Vector2 m_RotationAngles;	// ī�޶� ����(�� ������ Ʈ�������� rotation ����)
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