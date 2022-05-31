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
	// Ÿ�� ������Ʈ�� Ʈ������ ������ ���Ѵ�
	m_TargetGun->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// Ÿ���� �������� myObject�� Ʈ������ ����
	m_Transform->SetPosition(m_TargetTranslation);
	m_Transform->SetRotation(m_TargetQuaternion);
	m_Transform->SetScale(m_TargetScale);

	//CA_TRACE("pos - %f, %f, %f", m_Transform->m_Position.x, m_Transform->m_Position.y, m_Transform->m_Position.z);
}

void EquipmentController::OnRender()
{
}
