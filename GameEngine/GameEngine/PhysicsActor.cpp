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
/// 생성시에 Geo나 타입을 정함
/// </summary>
PhysicsActor::PhysicsActor()
	: ComponentBase(ComponentType::Physics)
	, m_bKinematic(false)
	, m_bGravity(true)
	, m_FreezePositionAndRotation()

	, m_PhysicsActorWorldTM()

	, m_SaveData(new PhysicsActor_Save())
{
	m_pGeometry = new SphereGeometry(5.0f);
	m_GeometryType = GeometryType::Sphere;

	m_UserIndex = s_UserIndex;
	m_PhysicsActorUserIndex = s_UserIndex;

	s_UserIndex++;

	m_RigidType = RigidType::Static;

	///////////////////////////////////////////////
	//ReserveCollisionVec();
	//SetBoundingOffset();
}

PhysicsActor::PhysicsActor(float radius, RigidType type)
	: ComponentBase(ComponentType::Physics)
	, m_bKinematic(false)
	, m_bGravity(true)
	, m_FreezePositionAndRotation()
	, m_RigidType(type)

	, m_PhysicsActorWorldTM()

	, m_SaveData(new PhysicsActor_Save())
{
	m_pGeometry = new SphereGeometry(radius);
	m_GeometryType = GeometryType::Sphere;

	m_UserIndex = s_UserIndex;
	m_PhysicsActorUserIndex = s_UserIndex;
	s_UserIndex++;


	//ReserveCollisionVec();
	//SetBoundingOffset();
}

PhysicsActor::PhysicsActor(DirectX::SimpleMath::Vector3 box, RigidType type)
	: ComponentBase(ComponentType::Physics)
	, m_bKinematic(false)
	, m_bGravity(true)
	, m_FreezePositionAndRotation()
	, m_RigidType(type)

	, m_PhysicsActorWorldTM()

	, m_SaveData(new PhysicsActor_Save())
{
	m_pGeometry = new BoxGeometry(box.x, box.y, box.z);
	m_GeometryType = GeometryType::Box;

	m_UserIndex = s_UserIndex;
	m_PhysicsActorUserIndex = s_UserIndex;
	s_UserIndex++;

	//ReserveCollisionVec();
	//SetBoundingOffset();

	//CA_TRACE("PhysicsActor()");
}

PhysicsActor::PhysicsActor(float radius, float height, RigidType type)
	: ComponentBase(ComponentType::Physics)
	, m_bKinematic(false)
	, m_bGravity(true)
	, m_FreezePositionAndRotation()
	, m_RigidType(type)

	, m_PhysicsActorWorldTM()

	, m_SaveData(new PhysicsActor_Save())
{
	m_pGeometry = new CapsuleGeometry(radius, height);
	m_GeometryType = GeometryType::Box;

	m_UserIndex = s_UserIndex;
	m_PhysicsActorUserIndex = s_UserIndex;
	s_UserIndex++;

	//ReserveCollisionVec();
	//SetBoundingOffset();
}

PhysicsActor::~PhysicsActor()
{
	release();

	Safe_Delete(m_SaveData)
}

/// <summary>
/// 시작 시 타입을 찾아 디버깅 렌더러를 붙임
/// </summary>
void PhysicsActor::Start()
{
	m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();

	// 액터들을 탐색하고 넣어주기?
	m_pMyObject->SetHasPhysics(true);

	ReserveCollisionVec();

	// 바운딩 오프셋 계산
	SetBoundingOffset();

}


void PhysicsActor::Update(float dTime)
{
	if (GetIsEnabled() == true)
	{
		SimpleMath::Vector3 _scale, _translation;
		SimpleMath::Quaternion _rotation;

		SimpleMath::Matrix _PhysicsWorldTM = m_Transform->m_WorldTM;
		_PhysicsWorldTM.Decompose(_scale, _rotation, _translation);

		// Transform 의 scale에 영향을 받지 않게 하기위해 One으로 고정
		// Geometry 의 Size에 맞게 그린다
		_scale = { SimpleMath::Vector3::One };

		m_PhysicsActorWorldTM =
			::SimpleMath::Matrix::CreateScale(_scale) *
			::SimpleMath::Matrix::CreateFromQuaternion(_rotation) *
			::SimpleMath::Matrix::CreateTranslation(_translation);
		

		Managers::GetInstance()->GetPhysicsEngine()->UpdatePhysicsTransform(this);

		/// 속도가 0이라도 호출(갱신)되지 않으면 OnCollisionStay 이벤트에 들어오지 않는다...
		/// 게임엔진이 피직스 업데이트가 먼저 업데이트되고 그 후에 스크립트의 Update()문이 도는 구조이므로 
		/// 피직스 액터 업데이트에서 속도를 디폴트(0)으로 줘보자.
		if (m_bKinematic == false)
		{
			// 속도를 매프레임 0으로 초기화해놓으니까 한번 입력시 세팅한 속도가 다음프레임에 0이되어서 투척 같은게 안된다...
			//DLLEngine::SetVelocity(this, Vector3::Zero);
		}
	}
	
}

void PhysicsActor::OnRender()
{

}

void PhysicsActor::OnHelperRender()
{
	// 엔진의 디버그 렌더 모드가 꺼져있으면 렌더 하지 않는다.
	if (Managers::GetInstance()->GetComponentSystem()->GetDebugRenderMode() == false)
	{
		return;
	}

	if (GetIsEnabled() == true)
	{
		/// 이후 수종 작업
		switch (m_pGeometry->GetType())
		{
		case Sphere:
		{
			SphereGeometry* _sphere = static_cast<SphereGeometry*>(m_pGeometry);
			m_pIRenderer->RenderSphere(_sphere->GetSphereRadius(), m_PhysicsActorWorldTM);
			break;
		}
		case Box:
		{
			BoxGeometry* _box = static_cast<BoxGeometry*>(m_pGeometry);
			m_pIRenderer->RenderBox(_box->GetBoxSize(), m_PhysicsActorWorldTM);
			break;
		}
		case Capsule: /// 반지름과, 높이로 캡슐 그려야함!! 수종 화이팅
		{
			///m_pIRenderer->RenderBox(m_ActorIndex, m_pMyObject->m_Transform->GetWorld());
			break;
		}
		default:
		{
			//m_pIRenderer->RenderSphere(m_PhysActorIndex, m_pMyObject->m_Transform->GetWorld());

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

_DLL GeometryType PhysicsActor::GetGetGeometryType() const
{
	return m_GeometryType;
}

void PhysicsActor::release()
{
	// 충돌 리스트 관리 벡터 클리어
	// 안에 내용물은 컴포넌트 시스템에서 CollisionUpdate 할 때 제거된다.
	m_CollisionEnter_V.clear();
	m_CollisionStay_V.clear();
	m_CollisionExit_V.clear();

	Safe_Delete(m_pGeometry);
}

_DLL void PhysicsActor::SetKinematic(const bool val)
{
	// 같은 값을 또 설정하려고 하면 하지 않음
	if (m_bKinematic == val)
	{
		return;
	}

	m_bKinematic = val;

	if (Managers::GetInstance()->GetPhysicsEngine()->FindPxRigidActor(this))
	{
		Managers::GetInstance()->GetPhysicsEngine()->SetKinematic(this, val);
	}

	if (m_bKinematic == false)
	{
		SetVelocity({ 0.0f, 0.0f, 0.0f });
	}
}

_DLL bool PhysicsActor::IsKinematic() const
{
	return m_bKinematic;
}

_DLL void PhysicsActor::SetGravity(const bool val)
{
	// 같은 값을 또 설정하려고 하면 하지 않음
	if (m_bGravity == val)
	{
		return;
	}

	m_bGravity = val;

	if (Managers::GetInstance()->GetPhysicsEngine()->FindPxRigidActor(this))
	{
		Managers::GetInstance()->GetPhysicsEngine()->SetGravity(this, m_bGravity);
	}

	// 속도를 초기화한다
	SetVelocity({ 0.0f,0.0f,0.0f });	// -> 한번 해줘야 적용됨...
}

_DLL bool PhysicsActor::IsGravity() const
{
 	return m_bGravity;
}

_DLL void PhysicsActor::SetBoxCollider(const Vector3 box, const RigidType rigidType)
{
	// RigidType 설정
	m_RigidType = rigidType;

	// GeometryType 설정
	m_GeometryType = GeometryType::Box;

	// 기존 Geometry 제거 후 새로 생성
	if (m_pGeometry)
	{
		Safe_Delete(m_pGeometry);
	}
	m_pGeometry = new BoxGeometry(box.x, box.y, box.z);

	// userIndes(userData)는 기존거 그대로 쓴다.

	/// 기존에 pxActor(RigidStatic/RigidDynamic) body를 제거하고 새 액터를 추가
	PhysicsEngine* _physicsEngine = Managers::GetInstance()->GetPhysicsEngine();
	_physicsEngine->ChangePxRigidActor(this);

	/// 바운딩 오프셋 재설정
	SetBoundingOffset();

	return;
}

_DLL void PhysicsActor::SetSphereCollider(const float radius, const RigidType rigidType)
{
	// RigidType 설정
	m_RigidType = rigidType;

	// GeometryType 설정
	m_GeometryType = GeometryType::Sphere;

	// 기존 Geometry 제거 후 새로 생성
	if (m_pGeometry)
	{
		Safe_Delete(m_pGeometry);
	}
	m_pGeometry = new SphereGeometry(radius);

	// userIndes(userData)는 기존거 그대로 쓴다.

	/// 기존에 pxActor(RigidStatic/RigidDynamic) body를 제거하고 새 액터를 추가
	PhysicsEngine* _physicsEngine = Managers::GetInstance()->GetPhysicsEngine();
	_physicsEngine->ChangePxRigidActor(this);

	/// 바운딩 오프셋 재설정
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
/// 충돌 리스트 벡터 공간 예약
/// 대략 10개정도 잡아놓으면 되겠지? 라는 생각
/// (동시에 충돌(Enter/Stay/Exit) 되는 경우의 수)
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
	// TODO: 여기에 return 문을 삽입합니다.
	return m_FreezePositionAndRotation;
}


FreezePositionAndRotation::Position& PhysicsActor::GetFreezePosition()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_FreezePositionAndRotation.position;
}

void PhysicsActor::SetFreezePosition(bool x, bool y, bool z)
{
	m_FreezePositionAndRotation.position.x = x;
	m_FreezePositionAndRotation.position.y = y;
	m_FreezePositionAndRotation.position.z = z;

	Managers::GetInstance()->GetPhysicsEngine()->SetRigidDynamicLockFlags(this, m_FreezePositionAndRotation);

	SetVelocity({ 0.0f, 0.0f, 0.0f });
}


FreezePositionAndRotation::Rotation& PhysicsActor::GetFreezeRotation()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_FreezePositionAndRotation.rotation;
}

void PhysicsActor::SetFreezeRotation(bool x, bool y, bool z)
{
	m_FreezePositionAndRotation.rotation.x = x;
	m_FreezePositionAndRotation.rotation.y = y;
	m_FreezePositionAndRotation.rotation.z = z;

	Managers::GetInstance()->GetPhysicsEngine()->SetRigidDynamicLockFlags(this, m_FreezePositionAndRotation);

	SetVelocity({ 0.0f, 0.0f, 0.0f });
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

	//m_SaveData->s_UserIndex = s_UserIndex;
	//m_SaveData->m_UserIndex = m_UserIndex;
	//m_SaveData->m_PhysActorIndex = m_PhysActorIndex;

	m_SaveData->m_GeometryType = static_cast<int>(m_GeometryType);
	m_SaveData->m_RigidType = static_cast<int>(m_RigidType);

	// type 에 따라 사이즈 저장
	switch (m_GeometryType)
	{
	case Sphere:
	{
		SphereGeometry* _sphereGeometry = dynamic_cast<SphereGeometry*>(m_pGeometry);
		m_SaveData->m_SaveRadius = _sphereGeometry->GetSphereRadius();
		break;
	}

	case Box:
	{
		BoxGeometry* _boxGeometry = dynamic_cast<BoxGeometry*>(m_pGeometry);
		m_SaveData->m_SaveBoxSizeX = _boxGeometry->GetBoxSize().x;
		m_SaveData->m_SaveBoxSizeY = _boxGeometry->GetBoxSize().y;
		m_SaveData->m_SaveBoxSizeZ = _boxGeometry->GetBoxSize().z;
		break;
	}
	}

	m_SaveData->m_bKinematic = m_bKinematic;
	m_SaveData->m_bGravity = m_bGravity;

	// Freeze position and rotation
	m_SaveData->m_FreezePositionX = m_FreezePositionAndRotation.position.x;
	m_SaveData->m_FreezePositionY = m_FreezePositionAndRotation.position.y;
	m_SaveData->m_FreezePositionZ = m_FreezePositionAndRotation.position.z;
	m_SaveData->m_FreezeRotationX = m_FreezePositionAndRotation.rotation.x;
	m_SaveData->m_FreezeRotationY = m_FreezePositionAndRotation.rotation.y;
	m_SaveData->m_FreezeRotationZ = m_FreezePositionAndRotation.rotation.z;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void PhysicsActor::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	//save::WriteVectorValue(&(*m_Value)["m_CollidedActor_V"], m_CollidedActor_V);

	//s_UserIndex = m_SaveData->s_UserIndex;
	//m_UserIndex = m_SaveData->m_UserIndex;
	//m_PhysActorIndex = m_SaveData->m_PhysActorIndex;

	m_GeometryType = static_cast<decltype(m_GeometryType)>(m_SaveData->m_GeometryType);
	CreateGeometry(m_GeometryType);

	m_RigidType = static_cast<decltype(m_RigidType)>(m_SaveData->m_RigidType);

	m_bKinematic = m_SaveData->m_bKinematic;
	m_bGravity = m_SaveData->m_bGravity;

	// Freeze position and rotation
	m_FreezePositionAndRotation.position.x = m_SaveData->m_FreezePositionX;
	m_FreezePositionAndRotation.position.y = m_SaveData->m_FreezePositionY;
	m_FreezePositionAndRotation.position.z = m_SaveData->m_FreezePositionZ;
	m_FreezePositionAndRotation.rotation.x = m_SaveData->m_FreezeRotationX;
	m_FreezePositionAndRotation.rotation.y = m_SaveData->m_FreezeRotationY;
	m_FreezePositionAndRotation.rotation.z = m_SaveData->m_FreezeRotationZ;

	// type 에 따라 사이즈 저장
	switch (m_GeometryType)
	{
	case Sphere:
	{
		m_pGeometry = new SphereGeometry(m_SaveData->m_SaveRadius);
		SetSphereCollider(m_SaveData->m_SaveRadius, static_cast<RigidType>(m_SaveData->m_RigidType));
		break;
	}

	case Box:
	{
		m_pGeometry = new BoxGeometry(m_SaveData->m_SaveBoxSizeX, m_SaveData->m_SaveBoxSizeY, m_SaveData->m_SaveBoxSizeZ);
		SetBoxCollider({ m_SaveData->m_SaveBoxSizeX, m_SaveData->m_SaveBoxSizeY, m_SaveData->m_SaveBoxSizeZ }, static_cast<RigidType>(m_SaveData->m_RigidType));
		break;
	}
	}

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

