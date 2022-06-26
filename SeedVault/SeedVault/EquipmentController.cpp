#include "pch.h"
#include "EquipmentController.h"

EquipmentController::EquipmentController()
	: ComponentBase(ComponentType::GameLogic)
	, m_PlayerMesh(nullptr)

	, m_WeaponMesh(nullptr)
	, m_WeaponParent(nullptr)
{
}

EquipmentController::~EquipmentController()
{
}

void EquipmentController::Start()
{
	// 부모 트랜스폼(오른손) 위치 연결
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_WeaponParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));

	// 무기 메쉬 오브젝트 연결
	GameObject* equipedMesh = DLLEngine::FindGameObjectByName("EquipedMesh");
	if (equipedMesh != nullptr)
	{
		m_WeaponMesh = equipedMesh->GetComponent<MeshFilter>();
	}
	assert(m_WeaponMesh != nullptr);
}

void EquipmentController::Update(float dTime)
{
	// 타겟 오브젝트의 트랜스폼 성분을 구한다
	m_WeaponParent->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// 타겟의 성분으로 myObject의 트랜스폼 구성
	m_Transform->SetPosition(m_TargetTranslation);
	m_Transform->SetRotation(m_TargetQuaternion);
	m_Transform->SetScale(m_TargetScale);

	//CA_TRACE("pos - %f, %f, %f", m_Transform->m_Position.x, m_Transform->m_Position.y, m_Transform->m_Position.z);
}

void EquipmentController::OnRender()
{
}

/// <summary>
/// 무기 교체를 했을 때 실제로 무기 메쉬와 트랜스폼을 변경, 조정하는 기능
/// </summary>
/// <param name="type"></param>
void EquipmentController::ChangeWeaponMesh(IItem::Type type)
{
	// 바뀐 플레이어 메쉬를 기준으로 부모 포지션을 다시 갱신
	m_WeaponParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));
	
	switch (type)
	{
	case IItem::Type::eLongGun:
	{
		m_WeaponMesh->SetMesh("Rifle.bin");
		m_WeaponMesh->m_Transform->SetPosition({ 0.05f, 0.0f, 0.15f });
		m_WeaponMesh->m_Transform->SetRotationFromVec({ 10.0f, 100.0f, 80.0f });
		m_WeaponMesh->m_Transform->SetScale({ 0.3f, 0.3f, 0.3f });
		break;
	}

	case IItem::Type::eHandGun:
	{
		//m_WeaponMesh->SetMesh("Rifle.bin");
		m_WeaponMesh->SetMesh("0_Sphere.bin");
		break;
	}
		
	case IItem::Type::eFlask:
		break;
	case IItem::Type::eFireFlask:
		break;
	case IItem::Type::eAlcohol:
		break;
	case IItem::Type::eHerb:
		break;
	case IItem::Type::eHealSyringe:
		break;
	case IItem::Type::eLongGunAmmo:
		break;
	case IItem::Type::eHandGunAmmo:
		break;
	case IItem::Type::eMax:
		break;
	default:
		break;
	}
}
