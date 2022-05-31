#pragma once

/// <summary>
/// ī�޶��� �˵� �̵� Ŭ����
/// Input(Ű or ���콺)�� ���� target ������ ���� �ؾ� �Ѵ�.
/// 
/// �ΰ��ӿ��� ���콺 �̵��� ���� ī�޶� �˵� �̵� �ؾ���
/// 
/// �ۼ��� : YoKing
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
	void SetFocusTrasform(Transform& targetTf);		// ����(Ÿ��) ����
	void SetFocusTrasform(GameObject& targetObj);	// ����(Ÿ��) ����
	void UpdateFocusPoint();						// Ÿ�� ������Ʈ�� Position ������Ʈ
	bool ManualRotation();							// �Է¿� ���� ȸ��(����, �Ŵ���)

	void ConstrainAngles();

public:
	virtual void SaveData() override;
	virtual void LoadData() override;
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component) override;

	struct OrbitCamera_Save* m_SaveData;

private:
	Transform* m_FocusTarget;			// ����(Ÿ�� ������Ʈ)
	Vector3 m_FocusPosition;			// ����(Ÿ��) ������

	float m_Distance;					// �������κ����� �Ÿ�
	float m_RotationSpeed;				// ȸ�� �ӵ�

	Vector2 m_RotationAngles;			// �˵� ��
	
	// ���Ѱ���
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
