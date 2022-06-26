#pragma once

/// <summary>
/// ���ӳ� ���� ī�޶�(�÷��̾� �ȷ���)�� �ൿ�� �����ϴ� Ŭ����
/// 
/// �� ��/�ƿ� �� ī�޶� �浹(Bounding)�� ����
/// 
/// �ۼ��� : �� �� ȯ
/// 
/// </summary>

#pragma region UpdateLog
// 2021-11-22
// Ŭ���� ����(��...)

// 2021-11-23
// ī�޶� ����/�ƿ� ��� ����(���� PlayerContorller ��ũ��Ʈ�� �ִ����� �̽�)

// 2021-12-09
// �÷��̾� �������¿� ���� ����/�ƿ� ������ ����ǰ� ����
// Current/Stand/Crouch �� ���� ����
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

	// ����/�ܾƿ� ������ ����
	void SetZoomInPosition();

	// ������ ī�޶� �� ��/�ƿ�
	void CameraZoomIn();
	void CameraZoomOut();


public:
	// Save/Load �� �Լ�
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

	struct CameraBehavior_Save* m_SaveData;

protected:

private:
	class PlayerController* m_PlayerController;	// �÷��̾� ��Ʈ�ѷ�

	Camera* m_Camera;					// ����ī�޶��� ī�޶� ������Ʈ

	Transform* m_ZoomInPosition;		// parent Transform

	float m_ZoomInFov;					// ����   �ÿ� ����� Ȯ��� aspect
	float m_ZoomOutFov;					// �ܾƿ� �ÿ� ����� aspect
	float m_CurrentFov;				

	float m_CameraSpeed;				// ī�޶� �̵��ӵ�
	Vector3 m_CurrentZoomInPos;			// ���� ������ ���� ������
	Vector3 m_CurrentCameraDefaultPos;	// ���� ������ ����Ʈ ������

	Vector3* m_StandZoomInPos;			// ���ִ� ������ ���� ������(�� ������ �����ͷ� ZoomInPosObject �� ����Ű���� �Ѵ�)
	Vector3 m_StandCameraDefaultPos;	// ���ִ� ������ ����Ʈ ������

	Vector3 m_CrouchZoomInPos;			// ���� ������ ���� ������
	Vector3 m_CrouchCameraDefaultPos;	// ���� ������ ����Ʈ ������
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


