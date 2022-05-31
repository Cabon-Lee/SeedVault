#include "pch.h"
#include "Managers.h"
#include "EngineDLL.h"
#include "PhysicsUtility.h"
#include "Transform.h"
#include "Raycast.h"
#include "Input.h"
#include "IRenderer.h"

#include "PhysicsActor.h"

int PhysicsActor::s_UserIndex = 0;

/// <summary>
/// �����ÿ� Geo�� Ÿ���� ����
/// </summary>
PhysicsActor::PhysicsActor()
	: ComponentBase(ComponentType::Physics)
	, m_bKinematic(false)
	, m_bGravity(true)
	, m_FreezePositionAndRotation()

	, m_SaveData(new PhysicsActor_Save())
{
	m_pGeometry = new SphereGeometry(5.0f);
	m_GeometryType = GeometryType::Sphere;

	m_UserIndex = s_UserIndex;
	m_PhysicsActorUserIndex = s_UserIndex;

	s_UserIndex++;

	m_RigidType = RigidType::Static;

	///////////////////////////////////////////////
	ReserveCollisionVec();
	SetBoundingOffset();
}

PhysicsActor::PhysicsActor(float radius, RigidType type)
	: ComponentBase(ComponentType::Physics)
	, m_bKinematic(false)
	, m_bGravity(true)
	, m_FreezePositionAndRotation()
	, m_RigidType(type)

	, m_SaveData(new PhysicsActor_Save())
{
	m_pGeometry = new SphereGeometry(radius);
	m_GeometryType = GeometryType::Sphere;

	m_UserIndex = s_UserIndex;
	m_PhysicsActorUserIndex = s_UserIndex;
	s_UserIndex++;


	ReserveCollisionVec();
	SetBoundingOffset();
}

PhysicsActor::PhysicsActor(DirectX::SimpleMath::Vector3 box, RigidType type)
	: ComponentBase(ComponentType::Physics)
	, m_bKinematic(false)
	, m_bGravity(true)
	, m_FreezePositionAndRotation()
	, m_RigidType(type)

	, m_SaveData(new PhysicsActor_Save())
{
	m_pGeometry = new BoxGeometry(box.x, box.y, box.z);
	m_GeometryType = GeometryType::Box;

	m_UserIndex = s_UserIndex;
	m_PhysicsActorUserIndex = s_UserIndex;
	s_UserIndex++;

	ReserveCollisionVec();
	SetBoundingOffset();

	//CA_TRACE("PhysicsActor()");
}

PhysicsActor::PhysicsActor(float radius, float height, RigidType type)
	: ComponentBase(ComponentType::Physics)
	, m_bKinematic(false)
	, m_bGravity(true)
	, m_FreezePositionAndRotation()
	, m_RigidType(type)

	, m_SaveData(new PhysicsActor_Save())
{
	m_pGeometry = new CapsuleGeometry(radius, height);
	m_GeometryType = GeometryType::Box;

	m_UserIndex = s_UserIndex;
	m_PhysicsActorUserIndex = s_UserIndex;
	s_UserIndex++;

	ReserveCollisionVec();
	SetBoundingOffset();
}

PhysicsActor::~PhysicsActor()
{
	release();

	Safe_Delete(m_SaveData)
}

/// <summary>
/// ���� �� Ÿ���� ã�� ����� �������� ����
/// </summary>
void PhysicsActor::Start()
{
	m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();

	// ���͵��� Ž���ϰ� �־��ֱ�?
	m_pMyObject->SetHasPhysics(true);

}


void PhysicsActor::Update(float dTime)
{
	if (GetIsEnabled() == true)
	{
		Managers::GetInstance()->GetPhysicsEngine()->UpdatePhysicsTransform(this);

		/// �ӵ��� 0�̶� ȣ��(����)���� ������ OnCollisionStay �̺�Ʈ�� ������ �ʴ´�...
		/// ���ӿ����� ������ ������Ʈ�� ���� ������Ʈ�ǰ� �� �Ŀ� ��ũ��Ʈ�� Update()���� ���� �����̹Ƿ� 
		/// ������ ���� ������Ʈ���� �ӵ��� ����Ʈ(0)���� �ຸ��.
		if (m_bKinematic == false)
		{
			// �ӵ��� �������� 0���� �ʱ�ȭ�س����ϱ� �ѹ� �Է½� ������ �ӵ��� ���������ӿ� 0�̵Ǿ ��ô ������ �ȵȴ�...
			//DLLEngine::SetVelocity(this, Vector3::Zero);
		}
	}
	
}

void PhysicsActor::OnRender()
{

}

void PhysicsActor::OnHelperRender()
{
	// ������ ����� ���� ��尡 ���������� ���� ���� �ʴ´�.
	if (Managers::GetInstance()->GetComponentSystem()->GetDebugRenderMode() == false)
	{
		return;
	}

	if (GetIsEnabled() == true)
	{
		/// ���� ���� �۾�
		switch (m_pGeometry->GetType())
		{
		case Sphere:
		{
			SphereGeometry* _sphere = static_cast<SphereGeometry*>(m_pGeometry);
			m_pIRenderer->RenderSphere(_sphere->GetSphereRadius(), m_pMyObject->m_Transform->GetWorld());
			break;
		}
		case Box:
		{
			BoxGeometry* _box = static_cast<BoxGeometry*>(m_pGeometry);
			m_pIRenderer->RenderBox(_box->GetBoxSize(), m_pMyObject->m_Transform->GetWorld());
			break;
		}
		case Capsule: /// ��������, ���̷� ĸ�� �׷�����!! ���� ȭ����
		{
			///m_pIRenderer->RenderBox(m_ActorIndex, m_pMyObject->m_Transform->GetWorld());
			break;
		}
		default:
		{
			m_pIRenderer->RenderSphere(m_PhysActorIndex, m_pMyObject->m_Transform->GetWorld());

			break;
		}
		}

		///m_pIRenderer->DrawDebugging()
	}
}

void PhysicsActor::OnEnable()
{
	if (GetIsEnabled() == false)
	{
		ComponentBase::OnEnable();

		Managers::GetInstance()->GetPhysicsEngine()->ChangePxRigidActor(this);
	}
}

void PhysicsActor::OnDisable()
{
	if (GetIsEnabled() == true)
	{
		ComponentBase::OnDisable();

		Managers::GetInstance()->GetPhysicsEngine()->ReleasePxActor(this);
	}
}

void PhysicsActor::SetPhysicalTransform(Transform* transform)
{
	m_Transform = transform;
}

GeometryBase* PhysicsActor::GetGeometry() const
{
	return m_pGeometry;
}

unsigned int PhysicsActor::GetUserIndex() const
{
	return m_UserIndex;
}

void PhysicsActor::SetUserIndex(int val)
{
	m_UserIndex = val;
}

RigidType PhysicsActor::GetRigidType() const
{
	return m_RigidType;
}

void PhysicsActor::release()
{
	// �浹 ����Ʈ ���� ���� Ŭ����
	// �ȿ� ���빰�� ������Ʈ �ý��ۿ��� CollisionUpdate �� �� ���ŵȴ�.
	m_CollisionEnter_V.clear();
	m_CollisionStay_V.clear();
	m_CollisionExit_V.clear();

	Safe_Delete(m_pGeometry);
}

_DLL void PhysicsActor::SetKinematic(const bool val)
{
	m_bKinematic = val;

	if (Managers::GetInstance()->GetPhysicsEngine()->FindPxRigidActor(this))
	{
		Managers::GetInstance()->GetPhysicsEngine()->SetKinematic(this, val);
	}
}

_DLL bool PhysicsActor::IsKinematic() const
{
	return m_bKinematic;
}

_DLL void PhysicsActor::SetGravity(const bool val)
{
	m_bGravity = val;

	if (Managers::GetInstance()->GetPhysicsEngine()->FindPxRigidActor(this))
	{
		Managers::GetInstance()->GetPhysicsEngine()->SetGravity(this, m_bGravity);
	}
}

_DLL bool PhysicsActor::IsGravity() const
{
	return m_bGravity;
}

_DLL void PhysicsActor::SetBoxCollider(const Vector3 box, const RigidType rigidType)
{
	// GeometryType ����
	m_GeometryType = GeometryType::Box;

	// ���� Geometry ���� �� ���� ����
	if (m_pGeometry)
	{
		Safe_Delete(m_pGeometry);
	}
	m_pGeometry = new BoxGeometry(box.x, box.y, box.z);

	// userIndes(userData)�� ������ �״�� ����.

	/// ������ pxActor(RigidStatic/RigidDynamic) body�� �����ϰ� �� ���͸� �߰�
	PhysicsEngine* _physicsEngine = Managers::GetInstance()->GetPhysicsEngine();
	_physicsEngine->ChangePxRigidActor(this);

	/// �ٿ�� ������ �缳��
	SetBoundingOffset();

	return;
}

_DLL void PhysicsActor::SetSphereCollider(const float radius, const RigidType rigidType)
{
	// GeometryType ����
	m_GeometryType = GeometryType::Sphere;

	// ���� Geometry ���� �� ���� ����
	if (m_pGeometry)
	{
		Safe_Delete(m_pGeometry);
	}
	m_pGeometry = new SphereGeometry(radius);

	// userIndes(userData)�� ������ �״�� ����.

	/// ������ pxActor(RigidStatic/RigidDynamic) body�� �����ϰ� �� ���͸� �߰�
	PhysicsEngine* _physicsEngine = Managers::GetInstance()->GetPhysicsEngine();
	_physicsEngine->ChangePxRigidActor(this);

	/// �ٿ�� ������ �缳��
	SetBoundingOffset();
}

void PhysicsActor::SetBoundingOffset()
{
	switch (m_GeometryType)
	{
	case Sphere:
	{
		SphereGeometry* _sphere = dynamic_cast<SphereGeometry*>(m_pGeometry);
		m_BoundingOffset.Forward = _sphere->GetSphereRadius();
		m_BoundingOffset.Right = _sphere->GetSphereRadius();
		m_BoundingOffset.Bottom = _sphere->GetSphereRadius();
		break;
	}

	case Box:
	{
		BoxGeometry* _boxGeometry = dynamic_cast<BoxGeometry*>(m_pGeometry);
		m_BoundingOffset.Forward = _boxGeometry->GetBoxSize().z / 2.0f;
		m_BoundingOffset.Right = _boxGeometry->GetBoxSize().x / 2.0f;
		m_BoundingOffset.Bottom = _boxGeometry->GetBoxSize().y / 2.0f;
		break;
	}
	case Capsule:
		break;
	case GeometryCount:
		break;
	case Invalid:
		break;
	default:
		break;
	}

}

_DLL BoundingOffset& PhysicsActor::GetBoundingOffset()
{
	return m_BoundingOffset;
}

/// <summary>
/// �浹 ����Ʈ ���� ���� ����
/// �뷫 10������ ��Ƴ����� �ǰ���? ��� ����
/// (���ÿ� �浹(Enter/Stay/Exit) �Ǵ� ����� ��)
/// </summary>
void PhysicsActor::ReserveCollisionVec()
{
	const unsigned int reserveSize = 10;

	m_CollisionEnter_V.reserve(reserveSize);
	m_CollisionStay_V.reserve(reserveSize);
	m_CollisionExit_V.reserve(reserveSize);
}

FreezePositionAndRotation& PhysicsActor::GetFreezePositionAndRotation()
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return m_FreezePositionAndRotation;
}


FreezePositionAndRotation::Position& PhysicsActor::GetFreezePosition()
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return m_FreezePositionAndRotation.position;
}

void PhysicsActor::SetFreezePosition(bool x, bool y, bool z)
{
	m_FreezePositionAndRotation.position.x = x;
	m_FreezePositionAndRotation.position.y = y;
	m_FreezePositionAndRotation.position.z = z;

	Managers::GetInstance()->GetPhysicsEngine()->SetRigidDynamicLockFlags(this, m_FreezePositionAndRotation);
}


FreezePositionAndRotation::Rotation& PhysicsActor::GetFreezeRotation()
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return m_FreezePositionAndRotation.rotation;
}

void PhysicsActor::SetFreezeRotation(bool x, bool y, bool z)
{
	m_FreezePositionAndRotation.rotation.x = x;
	m_FreezePositionAndRotation.rotation.y = y;
	m_FreezePositionAndRotation.rotation.z = z;

	Managers::GetInstance()->GetPhysicsEngine()->SetRigidDynamicLockFlags(this, m_FreezePositionAndRotation);
}

_DLL void PhysicsActor::SetVelocity(const Vector3& dir)
{
	Managers::GetInstance()->GetPhysicsEngine()->SetVelocity(this, dir);
}

void PhysicsActor::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	//save::WriteVectorValue(&(*m_Value)["m_CollidedActor_V"], m_CollidedActor_V);

	m_SaveData->s_UserIndex = s_UserIndex;

	m_SaveData->m_UserIndex = m_UserIndex;
	m_SaveData->m_PhysActorIndex = m_PhysActorIndex;

	m_SaveData->m_GeometryType = static_cast<int>(m_GeometryType);
	m_SaveData->m_RigidType = static_cast<int>(m_RigidType);

	m_SaveData->m_Velcity = &m_Velcity;

	m_SaveData->m_bKinematic = m_bKinematic;
	m_SaveData->m_bGravity = m_bGravity;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void PhysicsActor::LoadData()
{
	m_SaveData->m_Velcity = &m_Velcity;

	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	//save::WriteVectorValue(&(*m_Value)["m_CollidedActor_V"], m_CollidedActor_V);

	s_UserIndex = m_SaveData->s_UserIndex;

	m_UserIndex = m_SaveData->m_UserIndex;
	m_PhysActorIndex = m_SaveData->m_PhysActorIndex;

	m_GeometryType = static_cast<decltype(m_GeometryType)>(m_SaveData->m_GeometryType);
	CreateGeometry(m_GeometryType);

	m_RigidType = static_cast<decltype(m_RigidType)>(m_SaveData->m_RigidType);

	m_bKinematic = m_SaveData->m_bKinematic;
	m_bGravity = m_SaveData->m_bGravity;

	DLLEngine::AddPhysicsActorToPhysEngine(this);
}

void PhysicsActor::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
}


void PhysicsActor::CreateGeometry(GeometryType type)
{
	switch (type)
	{
	case Sphere:
		m_pGeometry = new SphereGeometry();
		break;

	case Box:
		m_pGeometry = new BoxGeometry();
		break;

	case Capsule:
		m_pGeometry = new CapsuleGeometry();
		break;

	default:
		break;
	}
}

