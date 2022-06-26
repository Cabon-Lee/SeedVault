#pragma once
#include "ComponentBase.h"

/// <summary>
/// �տ� ��ġ�ϴ� ��� �������� ��ġ�� �����ϴ� ������Ʈ
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class EquipmentController : public ComponentBase
{
public:
	EquipmentController();
	virtual ~EquipmentController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void ChangeWeaponMesh(IItem::Type type);

protected:

private:
	// �÷��̾� �޽�
	MeshFilter* m_PlayerMesh;

	// ���� �޽�
	MeshFilter* m_WeaponMesh;

	SimpleMath::Matrix* m_WeaponParent;
	SimpleMath::Vector3 m_TargetScale;
	SimpleMath::Quaternion m_TargetQuaternion;
	SimpleMath::Vector3 m_TargetTranslation;


};

