#include "pch.h"
#include "Managers.h"
#include "Timer.h"
#include "GameObject.h"
#include "Transform.h"
#include "SceneBase.h"
// SDK
//#include "PxPhysics.h"
//#include "PxPhysicsAPI.h"
// for wrapping
#include "PhysicsActor.h"
#include "PhysicsUtility.h"
#include "Raycast.h"
#include "PhysicsGeometry.h"

//#include "CollisionCallBack.h"
#include "BaseEventCallBack.h"

#include "PhysicsEngine.h"

//extern int s_UserIndex;
//////////////////////////////////////////////////////////////////////////

static physx::PxDefaultAllocator g_Allocator;
static physx::PxDefaultErrorCallback g_ErrorCallback;
static physx::PxDefaultCpuDispatcher* g_pDispatcher; // implementation to cpu
static physx::PxTolerancesScale g_ToleranceScale;
static physx::PxCudaContextManager* g_CudaContextManager;
static physx::PxCooking* g_Cooking;

static physx::PxPvd* g_pPVD; // for visual debugger

static physx::PxFoundation* g_pFoundation; // about settings (init, etc.)
static physx::PxPhysics* g_pPhysics; // core

physx::PxScene* g_pNowScene;
static unordered_map<std::string, physx::PxScene*> g_pScene_UM;

static physx::PxMaterial* g_pMaterial;

//////////////////////////////////////////////////////////////////////////

physx::PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT
		| PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_TOUCH_LOST
		| PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return PxFilterFlag::eDEFAULT;
}

physx::PxFilterFlags SampleSubmarineFilterShader
(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantlock, physx::PxU32 constantBlockSize)
{
	if (PxFilterObjectIsTrigger(attributes1) == true || PxFilterObjectIsTrigger(attributes0) || true)
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	return PxFilterFlag::eDEFAULT;
}


PhysicsEngine::PhysicsEngine()
	: m_BaseEvent(nullptr)
{
	//physx::PxRaycastHit _ray;
	//_ray.
}

PhysicsEngine::~PhysicsEngine()
{
	ReleasePhysics();
}

/// <summary>
/// 이거는 Managers에서 해줌
/// </summary>
/// <param name="isInteractive"></param>
void PhysicsEngine::Initialize(bool isInteractive)
{
	g_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_Allocator, g_ErrorCallback);

	g_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *g_pFoundation, PxCookingParams(PxTolerancesScale()));

	/// pvd
	g_pPVD = physx::PxCreatePvd(*g_pFoundation);
	physx::PxPvdTransport* _transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	g_pPVD->connect(*_transport, physx::PxPvdInstrumentationFlag::eALL);

	g_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_pFoundation, physx::PxTolerancesScale(), true, g_pPVD); // gPvd

	m_BaseEvent = new BaseEventCallBack();
	g_pDispatcher = physx::PxDefaultCpuDispatcherCreate(10);

	// 씬 하나 생성
	// 엔진 구조상 처음에 하나 만들어 있지 않으면 터져서
	// 일부러 하나 만들어야 함. 실제로 쓰는 씬은 여기서 만드는 씬과 상관 없음..
	physx::PxSceneDesc _sceneDesc(g_pPhysics->getTolerancesScale());
	_sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // 중력값
	_sceneDesc.cpuDispatcher = g_pDispatcher;
	_sceneDesc.simulationEventCallback = m_BaseEvent;
	//_sceneDesc.filterShader				= physx::PxDefaultSimulationFilterShader;
	_sceneDesc.filterShader = contactReportFilterShader;

	g_pNowScene = g_pPhysics->createScene(_sceneDesc);

	physx::PxPvdSceneClient* _pvdClient = g_pNowScene->getScenePvdClient();
	if (_pvdClient)
	{
		_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//////////////////////////////////////////////////////////////////////////
	/// for Test scene
	// create simulation

	// 머티리얼 설정 staticFriction, dynamicFriction, restitution
	g_pMaterial = g_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

}

/// <summary>
/// 이것도 Managers -> Engine에서 해줌
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::PhysicsLoop(bool interactive)
{
	//g_pNowScene->simulate(1.0f / 60.0f);
	g_pNowScene->simulate(Timer::GetInstance()->DeltaTime());
	g_pNowScene->fetchResults(true); // 물리를 쓸건가 안쓸건가?

	for (PhysicsActor* _actor : m_PhysicsActors_V)
	{
		// 각 피직스액터의 업데이트에서 하도록 변경함
		//UpdatePhysicsTransform(_actor);
	}
}

/// <summary>
/// 끝낼때 쓰는 릴리즈
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::ReleasePhysics(bool interactive)
{
	Safe_Delete(m_BaseEvent);
	Safe_Release(g_CudaContextManager);

	Safe_Release(g_pNowScene);
	Safe_Release(g_pDispatcher);
	Safe_Release(g_pPhysics);

	if (g_pPVD)
	{
		physx::PxPvdTransport* _transport = g_pPVD->getTransport();
		g_pPVD->release();	g_pPVD = NULL;
		Safe_Release(_transport);
	}

	Safe_Release(g_pFoundation);
}

/// <summary>
/// 주로 씬전환할때 액터들 정리
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::ReleasePhysicsActor(bool interactive)
{
	// 벡터의 내용물(가리키는 액터 컴포넌트)들은 게임오브젝트가 release(delete)될 떄
	// 가지고 있는 컴포넌트드을 제거하면서 이미 날라가 있다.
	// 여기서 또 제거하려고 하면 이미 제거되어 있는 메모리에 접근해서 터진다.

	// 벡터만 클리어
	m_PhysicsActors_V.clear();
}

/// <summary>
/// 실험용으로 만들었던것 (참고용), pvd에서는 작동됨
/// 게임 런타임 중에 physicsActor가 동적으로 만들어지는지 확인하기위해 만들었음
/// </summary>
/// <param name="transform"></param>
/// <param name="size"></param>
void PhysicsEngine::CreateTestBox(Transform* transform, float size)
{
	physx::PxTransform _pxTr = CreatePxTransform(*transform);

	physx::PxShape* _shape = g_pPhysics->createShape(physx::PxBoxGeometry(size, size, size), *g_pMaterial);
	//physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*g_pPhysics, physx::PxPlane(0, 1, 0, 0), *g_pMaterial); // 지면
	physx::PxRigidStatic* _body = physx::PxCreateStatic(*g_pPhysics, _pxTr, *_shape);
	//physx::PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pxTr);

	_body->attachShape(*_shape);

	g_pNowScene->addActor(*_body);
}
/// <summary>
/// 참고용 (실제 안씀)
/// </summary>
/// <param name="transform"></param>
/// <param name="geometry"></param>
/// <param name="velocity"></param>
void PhysicsEngine::CreateDynamicObject(Transform* transform, int geometry, Vector3 velocity)
{
	physx::PxTransform _pxTr = CreatePxTransform(*transform);

	GeometryType _GeoType = static_cast<GeometryType>(geometry);

	physx::PxGeometry* _nowPxGeo;
	switch (_GeoType)
	{
	case Sphere:
	{
		_nowPxGeo = CreateGeometry<physx::PxSphereGeometry>();
		break;
	}
	case Box:
	{
		_nowPxGeo = CreateGeometry<physx::PxBoxGeometry>();
		break;
	}
	default:
	{
		_nowPxGeo = CreateGeometry<physx::PxSphereGeometry>();
		break;
	}
	}

	/// 아래 코드는 안만들어진다 why?
	//physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*g_pPhysics, _pxTr, *_nowPxGeo, *g_pMaterial, 10.0f); 

	/// 아래 코드는 뭔가 만들어짐
	physx::PxRigidDynamic* _dynamicObj = g_pPhysics->createRigidDynamic(_pxTr);
	physx::PxShape* _shape = g_pPhysics->createShape(physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *g_pMaterial);
	_dynamicObj->attachShape(*_shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*_dynamicObj, 10.0f);

	physx::PxVec3 _vec(velocity.x, velocity.y, velocity.z);

	_dynamicObj->setAngularDamping(0.5f);
	_dynamicObj->setLinearVelocity(_vec);

	g_pNowScene->addActor(*_dynamicObj);
}

/// <summary>
/// 참고용 (실제 안씀)
/// </summary>
/// <param name="transform"></param>
/// <param name="size"></param>
/// <param name="halfExtent"></param>
void PhysicsEngine::CreateStack(Transform* transform, uint32_t size, float halfExtent)
{
	physx::PxTransform _pxTr = CreatePxTransform(*transform);

	physx::PxShape* _shape = g_pPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *g_pMaterial);
	for (uint32_t i = 0; i < size; i++)
	{
		for (uint32_t j = 0; j < size - i; j++)
		{
			physx::PxTransform _localTm(physx::PxVec3(float(j * 2) - float(size - i), float(i * 2 + 1), 0) * halfExtent);
			physx::PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pxTr.transform(_localTm));
			_body->attachShape(*_shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*_body, 10.0f);
			g_pNowScene->addActor(*_body);
		}
	}

	_shape->release();
}

/// <summary>
/// Managers에서 해줌 (씬이 isPhysicsScene이면 알아서 등록)
/// Tolerance하나정하면 그대로 씀
/// </summary>
/// <param name="scene"></param>
void PhysicsEngine::AddPhysicsScene(SceneBase* scene)
{
	physx::PxCudaContextManagerDesc cudaContextManagerDesc;
	//문제의 시발점
	g_CudaContextManager = PxCreateCudaContextManager(*g_pFoundation, cudaContextManagerDesc, PxGetProfilerCallback());

	m_BaseEvent = new BaseEventCallBack();

	g_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);


	static bool _isFirstScene = true;


	physx::PxSceneDesc _sceneDesc(g_pPhysics->getTolerancesScale());
	_sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);		// 중력값
	//_sceneDesc.gravity = physx::PxVec3(0.0f, -0.01, 0.0f);	// 기존의 -9.81 중력값이 값자기 충돌처리가 되지않고 
																// 바닥밑으로 쭈욱 내려가는 버그가 생겨서 임시로 변경함
																// 프레임이 밀릴경우 발생할 수 있음..

	/// 이용가능한 CPU의 쓰레드 개수를 가져오는 윈도우함수를 써보자
	_sceneDesc.cpuDispatcher = g_pDispatcher;
	_sceneDesc.simulationEventCallback = m_BaseEvent;

	//_sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; /// 뭔지모르겠다...
	_sceneDesc.filterShader = contactReportFilterShader;
	_sceneDesc.cudaContextManager = g_CudaContextManager;
	//_sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	_sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eDEFAULT;

	//_sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	//_sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;		// 형선왈 -> 비용이 심하다 총알같이 고속으로 움직이는거면 모를까..
	//_sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;

	physx::PxScene* _pScene = g_pPhysics->createScene(_sceneDesc);

	physx::PxPvdSceneClient* _pvdClient = g_pNowScene->getScenePvdClient();
	if (_pvdClient)
	{
		_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	g_pScene_UM.insert({ scene->GetSceneName(), _pScene });

	if (_isFirstScene == true)
	{
		//SetNowPhysicsScene(scene->GetSceneName());
		_isFirstScene = false;
	}
}

/// <summary>
/// 알아서 되게끔 수정필요
/// </summary>
/// <param name="name"></param>
void PhysicsEngine::SetNowPhysicsScene(std::string name)
{
	unordered_map<string, physx::PxScene*>::iterator _scene = g_pScene_UM.find(name);

	if (_scene == g_pScene_UM.end())
	{
		Trace("Can not find the scene");

		return;
	}

	if (g_pNowScene != nullptr)
	{
		ReleaseAllPxActor();
	}

	// 만들어야할것
	//g_pNowScene->ResetPhysics();
	//Managers::GetInstance()->GetCameraManager()->RemoveAllCamera();
	g_pNowScene = _scene->second;
	//m_NowScene->Initialize();

	// userIndex 초기화
	PhysicsActor::s_UserIndex = 0;
}

/// <summary>
/// physicsActor를 받아와서 phsyx Scene에 넣어줌
/// shape, rigid type등을 분류
/// </summary>
/// <param name="object">오브젝트 생성자에서 Geo, shape들을 가져옴</param>
void PhysicsEngine::AddPhysicsActor(PhysicsActor* object)
{
	// RigidType 체크
	switch (object->GetRigidType())
	{
	case Static: // 무게와 관성의 영향 없음
	{
		// RigidStatic 생성
		physx::PxRigidStatic* _body = CreateRigidStatic(object);

		/// 씬과 액터벡터에 추가
		g_pNowScene->addActor(*_body);
		m_PhysicsActors_V.push_back(object);
		break;
	}

	case Dynamic: // 가변적 물리량을 가짐
	default:
	{
		// RigidDynamic 생성
		physx::PxRigidDynamic* _body = CreateRigidDynamic(object);

		// 씬과 벡터에 추가
		g_pNowScene->addActor(*_body);
		m_PhysicsActors_V.push_back(object);

		// 축 고정 여부(Freeze) 세팅
		// 위에서 먼저 씬, 벡터에 추가가 된 이후에 이루어져야 함.
		SetRigidDynamicLockFlags(object, object->GetFreezePositionAndRotation());

		break;
	}
	}
}


/// <summary>
/// 엔진(클라)의 트랜스폼으로 피직스씬의 트랜스폼을 세팅
/// </summary>
/// <param name="transform"></param>
void PhysicsEngine::SetPhysicsTransform(Transform* transform)
{
	PhysicsActor* actor = transform->GetComponent<PhysicsActor>();

	if (actor->GetIsEnabled() == true)
	{
		PxRigidActor* pxActor = FindPxRigidActor(actor);
		pxActor->setGlobalPose(CreatePxTransform(*transform));
	}
}

/// <summary>
/// 피직스쪽 트랜스폼을 가져와 클라쪽 트랜스폼을 업데이트
/// </summary>
/// <param name="actor">액터를 받아와서 업데이트</param>
void PhysicsEngine::UpdatePhysicsTransform(PhysicsActor* actor)
{
	PxRigidActor* _pxRigidActor = FindPxRigidActor(actor);
	if (_pxRigidActor)
	{
		UpdateTransformToPhysics(actor->m_Transform, _pxRigidActor->getGlobalPose());
	}

}

/// <summary>
/// 트랜스폼을 바꿔줌
/// </summary>
/// <param name="object">변경할 오브젝트</param>
/// <param name="pos">바꿀 트랜스폼</param>
void PhysicsEngine::SetTransform(PhysicsActor* actor, const Transform& trn)
{
	// 씬에서 액터를 탐색하고 _actor_V에 넣어준다
	//std::vector<physx::PxRigidActor*> _actor_V = GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

	physx::PxRigidDynamic* _dynamicActor = (physx::PxRigidDynamic*)GetPxRigidActor(m_PxRigidActor, actor->GetUserIndex());


	_dynamicActor->setGlobalPose(CreatePxTransform(trn));
}

/// <summary>
/// 일정 속도 주는 함수 (AddForce같은것)
/// </summary>
/// <param name="object">변경할 오브젝트</param>
/// <param name="vel">변경할 속도</param>
void PhysicsEngine::SetVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel)
{
	// 씬에서 액터를 탐색하고 _actor_V에 넣어준다
	//GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC, m_PxRigidActor.data());
	/// 미리 받아서 쓸 방법이 없는지 생각해보자
	physx::PxRigidDynamic* _dynamicActor = (physx::PxRigidDynamic*)GetPxRigidActor(m_PxRigidActor, actor->GetUserIndex());

	if (_dynamicActor)
	{
		// 기본 MoveSpeed는 트랜스폼 이동에 맞추어 설정되어 있음.
		// 이를 그대로 물리이동에 적용하면 제대로 움직이지 않아서 이동량에 offset을 보정해준다.
		const float _offset = 15.0f;
		SimpleMath::Vector3 _vel = SimpleMath::Vector3::Zero;

		if (actor->IsGravity() == false)
		{
			_vel = { vel.x,	vel.y, vel.z };
		}

		else
		{
			_vel = { vel.x,	_dynamicActor->getLinearVelocity().y + vel.y, vel.z };
		}

		_vel *= _offset;
		_dynamicActor->setLinearVelocity(CreatePxVec3(_vel));
	}
}

/// <summary>
/// 일정 회전속도 주는 함수 (AddForce같은것)
/// </summary>
/// <param name="object">변경할 오브젝트</param>
/// <param name="vel">변경할 각속도</param>
void PhysicsEngine::SetAngularVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel)
{
	// 씬에서 액터를 탐색하고 _actor_V에 넣어준다
	/// 미리 받아서 쓸 방법이 없는지 생각해보자
	physx::PxRigidDynamic* _dynamicActor = (physx::PxRigidDynamic*)GetPxRigidActor(m_PxRigidActor, actor->GetUserIndex());

	_dynamicActor->setAngularVelocity(CreatePxVec3(vel));
}

/// <summary>
/// 게임오브젝트 가져오기
/// </summary>
/// <param name="index">인덱스통해서</param>
/// <returns></returns>
GameObject* PhysicsEngine::GetGameObjectByIndex(unsigned int index)
{
	for (PhysicsActor* _actor : m_PhysicsActors_V)
	{
		if (index == _actor->GetUserIndex())
		{
			return _actor->GetMyObject();
		}
	}

	return nullptr;
}

/// <summary>
/// 피직스액터 가져오기
/// </summary>
/// <param name="index">인덱스 통해서</param>
/// <returns></returns>
PhysicsActor* PhysicsEngine::GetPhysicsActorByIndex(unsigned int index)
{
	for (PhysicsActor* _actor : m_PhysicsActors_V)
	{
		if (index == _actor->GetUserIndex())
		{
			return _actor;
		}
	}

	return nullptr;
}

/// <summary>
/// 게임오브젝트로부터 인덱스 가져오기
/// </summary>
/// <param name="object"></param>
/// <returns></returns>
unsigned int PhysicsEngine::GetIndexFromGameObject(GameObject* object)
{
	return object->GetComponent<PhysicsActor>()->GetUserIndex();
}

/// <summary>
/// 피직스액터로부터 인덱스 가져오기
/// </summary>
/// <param name="actor"></param>
/// <returns></returns>
unsigned int PhysicsEngine::GetIndexFromPhysicsActor(PhysicsActor* actor)
{
	return actor->GetUserIndex();
}

/// collision event가 필요없을때 씀
//_DLL GameObject* PhysicsEngine::RaycastCheckGameObject(DirectX::SimpleMath::Vector3 rayOrigin,
//   DirectX::SimpleMath::Vector3 rayDir,
//   GeometryBase* geometry, Transform* transform,
//   float maxDist, int hitFlag)
//{
//   physx::PxVec3 _origin = CreatePxVec3(rayOrigin);
//   physx::PxVec3 _dir = CreatePxVec3(rayDir);
//   physx::PxGeometry* _geo = CreatePxGeometry(geometry);
//   physx::PxTransform _trn = CreatePxTransform(*transform);
//   //physx::PxHitFlag::Enum _hit = CreatePxHitFlag((HitFlag)hitFlag);
//   physx::PxHitFlags _hitFlag = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
//   physx::PxRaycastHit _rayHit;
//
//
//   /// 작동을 안한다... 왜...?
//   physx::PxGeometryQuery::raycast(_origin, _dir, *_geo, _trn, maxDist, _hitFlag, 10, &_rayHit);
//
//
//   //RaycastHit _hitInfo;
//
//   if (_rayHit.actor == nullptr)
//   {
//      return nullptr;
//   }
//   else
//   {
//      return GetPhysicsActorByIndex((int)_rayHit.actor->userData);
//   }
//}

/// <summary>
/// 레이캐스트를 쏴서 게임오브젝트를 가져옴
/// </summary>
/// <param name="origin">내포지션</param>
/// <param name="dir">보는 방향</param>
/// <param name="dist">방향 길이</param>
/// <returns>레이에 닿은 게임오브젝트</returns>
GameObject* PhysicsEngine::RaycastCheckGameObject(const DirectX::SimpleMath::Vector3& origin,
	const DirectX::SimpleMath::Vector3& dir, float dist)
{
	physx::PxVec3 _origin = CreatePxVec3(origin);
	physx::PxVec3 _dir = CreatePxVec3(dir);
	physx::PxRaycastBuffer _hitBuffer;
	physx::PxRaycastHit _rayHit;
	physx::PxHitFlags _hitFlag = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;

	g_pNowScene->raycast(_origin, _dir, dist, _hitBuffer, _hitFlag);

	_rayHit = _hitBuffer.block;

	if (_rayHit.actor == nullptr)
	{
		return nullptr;
	}
	else
	{
		return GetGameObjectByIndex(reinterpret_cast<int>(_rayHit.actor->userData));
	}
}

/// <summary>
/// 레이캐스트히트에 대한 정보 가져오기
/// </summary>
/// <param name="origin">내포지션</param>
/// <param name="dir">보는 방향</param>
/// <param name="dist">방향 길이</param>
/// <returns>레이캐스트히트정보</returns>
RaycastHit PhysicsEngine::CheckRaycastHit(const DirectX::SimpleMath::Vector3& origin,
	const DirectX::SimpleMath::Vector3& dir, float dist)
{
	physx::PxVec3 _origin = CreatePxVec3(origin);
	physx::PxVec3 _dir = CreatePxVec3(dir);
	physx::PxRaycastBuffer _hitBuffer;
	physx::PxRaycastHit _rayHit;
	physx::PxHitFlags _hitFlag = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;

	g_pNowScene->raycast(_origin, _dir, dist, _hitBuffer, _hitFlag);

	_rayHit = _hitBuffer.block;

	if (_rayHit.actor == nullptr)
	{
		return RaycastHit(); // 빈껍데기
	}
	else
	{
		RaycastHit _hitData;
		_hitData.actor = GetPhysicsActorByIndex(reinterpret_cast<int>(_rayHit.actor->userData));
		_hitData.faceIndex = _rayHit.faceIndex;
		_hitData.flag = CreateHitFlag(physx::PxHitFlag::ePOSITION); // 일단 포지션으로
		_hitData.position = CreateVector3(_rayHit.position);
		_hitData.normal = CreateVector3(_rayHit.normal);
		_hitData.distance = _rayHit.distance;
		_hitData.u = _rayHit.u;
		_hitData.v = _rayHit.v;

		return _hitData;
	}
}

/// <summary>
/// PxScene에서 게임 엔진의 PhysicsActor에 해당하는 PxActor(PxRigidStatic/PxRigidDynamic)를 
/// 찾아서 리턴한다.
/// </summary>
/// <param name="actor">게임 엔진의 피직스 액터</param>
physx::PxRigidActor* PhysicsEngine::FindPxRigidActor(PhysicsActor* actor)
{
	// 현재 PxScene에 액터가 몇개 있는지 카운트한다.
	uint32_t _actorNum = g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

	// PxActor 카운트만큼 벡터 할당
	std::vector<physx::PxRigidActor*> _actor_V(_actorNum);

	// 현재 PxScene 의 모든 PxActor 데이터를 얻어옴.
	g_pNowScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC,
		reinterpret_cast<physx::PxActor**>(_actor_V.data()), _actorNum);

	// 벡터를 순회하면서 찾고자하는 userData(index)에 해당하는 PxActor를 찾는다.
	for (auto pxRigidActor : _actor_V)
	{
		unsigned int _userData = reinterpret_cast<unsigned int>(pxRigidActor->userData);

		// 해당하는 PxRigidActor를 찾음!
		if (_userData == actor->GetUserIndex())
		{
			return pxRigidActor;
		}
	}

	// 못 찾았다.
	return nullptr;
}
/// <summary>
/// 기존의 pxActor를 제거하고 새 pxActor를 만들어서
/// 게임 엔진의 PhysicsActor 와 userIndex(userData)를 연결한다.
/// -> 게임오브젝트는 냅두고 콜라이더(pxActor)만 바꾸고 싶을때(사이즈 조절 등..)
/// </summary>
void PhysicsEngine::ChangePxRigidActor(PhysicsActor* actor)
{
	/// PhysicsActor와 연결된 PxActor를 찾는다.
	// 기존 PxActor 제거
	physx::PxRigidActor* _pxRigidActor = FindPxRigidActor(actor);
	if (_pxRigidActor != nullptr)
	{
		_pxRigidActor->release();
	}

	/// 새 PxActor 생성
	// RigidType 체크
	switch (actor->GetRigidType())
	{
	case Static:
	{
		// RigidStatic 생성
		physx::PxRigidStatic* _body = CreateRigidStatic(actor);

		// 씬에 RigidStatic 추가
		g_pNowScene->addActor(*_body);

		// PhysicsActor 는 이미 m_PhysicsActors_V 에 들어가 있으므로 더 추가하지 않는다.
	}

	case Dynamic:
	default:
	{
		// RigidDynamic 생성
		physx::PxRigidDynamic* _body = CreateRigidDynamic(actor);

		// 씬에 RigidDynamic 추가
		g_pNowScene->addActor(*_body);

		// PhysicsActor 는 이미 m_PhysicsActors_V 에 들어가 있으므로 더 추가하지 않는다.

		// 축 고정 여부(Freeze) 세팅
		// 위에서 먼저 씬, 벡터에 추가가 된 이후에 이루어져야 함.
		SetRigidDynamicLockFlags(actor, actor->GetFreezePositionAndRotation());
	}
	}

	GetPxRigidActors();
}

void PhysicsEngine::ReleasePxActor(PhysicsActor* actor)
{
	auto rigidActor = FindPxRigidActor(actor);

	rigidActor->release();
	GetPxRigidActors();
}

/// <summary>
/// 현재 PxScene에 있는 Px액터들 (RigidStatic/RigidDynamic)을 릴리즈한다.
/// </summary>
void PhysicsEngine::ReleaseAllPxActor()
{
	uint32_t _actorNum = g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
	std::vector<physx::PxRigidActor*> _actor_V(_actorNum);
	g_pNowScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<physx::PxActor**>(_actor_V.data()), _actorNum);

	for (unsigned int i = 0; i < _actorNum; i++)	// 전부 릴리즈
	{
		_actor_V[i]->release();
	}

	ReleasePhysicsActor();
}

void PhysicsEngine::GetPxRigidActors()
{
	// 씬에서 액터를 탐색하고 _actor_V에 넣어준다
	m_PxRigidActor.clear();
	m_PxRigidActor.resize(g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC));
	GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC, m_PxRigidActor.data());
}

/// <summary>
/// psyx Actor에 pos과 rot 값을 고정시킬지 세팅하는 함수
/// </summary>
/// <param name="PhysicsActor">게임 엔진의 피직스액터</param>
/// <param name="freezeValue">freeze 할 값 정보</param>
void PhysicsEngine::SetRigidDynamicLockFlags(PhysicsActor* actor, FreezePositionAndRotation freezeValue)
{
	// 혹시 static 오브젝트의 freeze값을 변경하려고할 때는 그냥 리턴한다.
	// (static 오브젝트는 애초에 고정이다)
	if (actor->GetRigidType() == RigidType::Static)
	{
		return;
	}

	/// 적용할 액터(rigidDynamic)를 physx 씬에서 찾는다
	physx::PxRigidDynamic* rigidDynamic = reinterpret_cast<PxRigidDynamic*>(FindPxRigidActor(actor));

	if (rigidDynamic == nullptr)
	{
		return;
	}

	// 고정할 플래그값 생성
	PxRigidDynamicLockFlags _flag;

	(freezeValue.position.x == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X : _flag &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
	(freezeValue.position.y == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y : _flag &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
	(freezeValue.position.z == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z : _flag &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;

	(freezeValue.rotation.x == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X : _flag &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
	(freezeValue.rotation.y == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y : _flag &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
	(freezeValue.rotation.z == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z : _flag &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	// 새로 만든 플래그값으로 재 설정
	rigidDynamic->setRigidDynamicLockFlags(_flag);
}

_DLL void PhysicsEngine::SetKinematic(PhysicsActor* actor, bool val)
{
	// 혹시 static 오브젝트의 freeze값을 변경하려고할 때는 그냥 리턴한다.
	// (static 오브젝트는 애초에 고정이다)
	if (actor->GetRigidType() == RigidType::Static)
	{
		return;
	}

	/// 적용할 액터(rigidDynamic)를 physx 씬에서 찾는다
	physx::PxRigidDynamic* rigidDynamic = reinterpret_cast<PxRigidDynamic*>(FindPxRigidActor(actor));

	rigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, val);
}

_DLL void PhysicsEngine::SetGravity(PhysicsActor* actor, bool val)
{
	// 혹시 static 오브젝트의 freeze값을 변경하려고할 때는 그냥 리턴한다.
	// (static 오브젝트는 애초에 고정이다)
	if (actor->GetRigidType() == RigidType::Static)
	{
		return;
	}

	/// 적용할 액터(rigidDynamic)를 physx 씬에서 찾는다
	physx::PxRigidDynamic* rigidDynamic = reinterpret_cast<PxRigidDynamic*>(FindPxRigidActor(actor));

	rigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !val);
}

physx::PxShape* PhysicsEngine::CreateGeometryAndShape(const PhysicsActor* actor)
{
	physx::PxGeometry* pxGeo = nullptr;
	physx::PxShape* pxShape = nullptr;

	GeometryType _geometryType = actor->GetGeometry()->GetType();

	switch (_geometryType)
	{
	case Sphere:
	{
		pxGeo = CreateGeometry<physx::PxSphereGeometry>();
		SphereGeometry* _sphere = dynamic_cast<SphereGeometry*>(actor->GetGeometry());
		if (_sphere == nullptr)
		{
			pxShape = g_pPhysics->createShape(physx::PxSphereGeometry(), *g_pMaterial);
			break;
		}
		pxShape = g_pPhysics->createShape(physx::PxSphereGeometry(_sphere->GetSphereRadius()), *g_pMaterial);
		break;
	}

	case Box:
	{
		pxGeo = CreateGeometry<physx::PxBoxGeometry>();
		BoxGeometry* _box = dynamic_cast<BoxGeometry*>(actor->GetGeometry());
		if (_box == nullptr)
		{
			pxShape = g_pPhysics->createShape(physx::PxBoxGeometry(), *g_pMaterial);
			break;
		}
		pxShape = g_pPhysics->createShape(physx::PxBoxGeometry(_box->GetBoxSize().x, _box->GetBoxSize().y, _box->GetBoxSize().z), *g_pMaterial);

		break;
	}

	case Capsule:
	{
		pxGeo = CreateGeometry<physx::PxCapsuleGeometry>();
		CapsuleGeometry* _capsule = dynamic_cast<CapsuleGeometry*>(actor->GetGeometry());
		if (_capsule == nullptr)
		{
			pxShape = g_pPhysics->createShape(physx::PxCapsuleGeometry(), *g_pMaterial);
			break;
		}
		pxShape = g_pPhysics->createShape(physx::PxCapsuleGeometry(_capsule->GetCapsuleRadius(), _capsule->GetCapsuleHeight()), *g_pMaterial);
		break;
	}

	case GeometryCount:
		break;

	case Invalid:
		break;

	default:
	{
		pxGeo = CreateGeometry<physx::PxSphereGeometry>();
		SphereGeometry* _sphere = dynamic_cast<SphereGeometry*>(actor->GetGeometry());
		if (_sphere == nullptr)
		{
			pxShape = g_pPhysics->createShape(physx::PxSphereGeometry(), *g_pMaterial);
			break;
		}
		pxShape = g_pPhysics->createShape(physx::PxSphereGeometry(_sphere->GetSphereRadius()), *g_pMaterial);
		break;
	}
	}

	return pxShape;
}

physx::PxRigidStatic* PhysicsEngine::CreateRigidStatic(const PhysicsActor* actor)
{
	// shape 생성
	physx::PxShape* pxShape = pxShape = CreateGeometryAndShape(actor);;

	// 게임 오브젝트의 트랜스폼으로 PxTransform 생성
	physx::PxTransform _pTransform = CreatePxTransform(*actor->m_Transform);

	// RigidStatic 생성
	physx::PxRigidStatic* _body = g_pPhysics->createRigidStatic(_pTransform);
	_body->userData = reinterpret_cast<void*>(actor->GetUserIndex());

	// shape 부착
	_body->attachShape(*pxShape);

	return _body;
}

physx::PxRigidDynamic* PhysicsEngine::CreateRigidDynamic(const PhysicsActor* actor)
{
	// shape 생성
	physx::PxShape* pxShape = pxShape = CreateGeometryAndShape(actor);;

	// 게임 오브젝트의 트랜스폼으로 PxTransform 생성
	physx::PxTransform _pTransform = CreatePxTransform(*actor->m_Transform);

	// RigidDynamic 생성
	PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pTransform);
	_body->userData = reinterpret_cast<void*>(actor->GetUserIndex());
	_body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, actor->IsKinematic());
	_body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !actor->IsGravity());

	physx::PxRigidBodyExt::updateMassAndInertia(*_body, 10.0f);

	// shape 부착
	_body->attachShape(*pxShape);

	return _body;
}
