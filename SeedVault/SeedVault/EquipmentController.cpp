#include "pch.h"
#include "EquipmentController.h"

EquipmentController::EquipmentController()
	: ComponentBase(ComponentType::GameLogic)
	, m_TargetGun(nullptr)
{
}

EquipmentController::~EquipmentController()
{
}

void EquipmentController::Start()
{
	MeshFilter* _meshFilter = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_TargetGun = reinterpret_cast<SimpleMath::Matrix*>(_meshFilter->GetBoneDataWorldTM("RightHand"));
}

void EquipmentController::Update(float dTime)
{
	// 타겟 오브젝트의 트랜스폼 성분을 구한다
	m_TargetGun->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// 타겟의 성분으로 myObject의 트랜스폼 구성
	m_Transform->SetPosition(m_TargetTranslation);
	m_Transform->SetRotation(m_TargetQuaternion);
	m_Transform->SetScale(m_TargetScale);

	//CA_TRACE("pos - %f, %f, %f", m_Transform->m_Position.x, m_Transform->m_Position.y, m_Transform->m_Position.z);
}

void EquipmentController::OnRender()
{
}
