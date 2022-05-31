#pragma once
#include "ComponentBase.h"

/// <summary>
/// �տ� ��ġ�ϴ� ��� �������� ��ġ�� �����ϴ� ������Ʈ
/// 
/// �ۼ��� : Yoking
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

protected:

private:
	SimpleMath::Matrix* m_TargetGun;
	SimpleMath::Vector3 m_TargetScale;
	SimpleMath::Quaternion m_TargetQuaternion;
	SimpleMath::Vector3 m_TargetTranslation;


};

