#pragma once
#include "ComponentBase.h"

/// <summary>
/// 손에 위치하는 장비 아이템의 위치를 조정하는 컴포넌트
/// 
/// 작성자 : Yoking
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

