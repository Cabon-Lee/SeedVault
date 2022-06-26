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
/// �̰Ŵ� Managers���� ����
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

	// �� �ϳ� ����
	// ���� ������ ó���� �ϳ� ����� ���� ������ ������
	// �Ϻη� �ϳ� ������ ��. ������ ���� ���� ���⼭ ����� ���� ��� ����..
	physx::PxSceneDesc _sceneDesc(g_pPhysics->getTolerancesScale());
	_sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // �߷°�
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

	// ��Ƽ���� ���� staticFriction, dynamicFriction, restitution
	g_pMaterial = g_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

}

/// <summary>
/// �̰͵� Managers -> Engine���� ����
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::PhysicsLoop(bool interactive)
{
	//g_pNowScene->simulate(1.0f / 60.0f);
	g_pNowScene->simulate(Timer::GetInstance()->DeltaTime());
	g_pNowScene->fetchResults(true); // ������ ���ǰ� �Ⱦ��ǰ�?

	for (PhysicsActor* _actor : m_PhysicsActors_V)
	{
		// �� ������������ ������Ʈ���� �ϵ��� ������
		//UpdatePhysicsTransform(_actor);
	}
}

/// <summary>
/// ������ ���� ������
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
/// �ַ� ����ȯ�Ҷ� ���͵� ����
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::ReleasePhysicsActor(bool interactive)
{
	// ������ ���빰(����Ű�� ���� ������Ʈ)���� ���ӿ�����Ʈ�� release(delete)�� ��
	// ������ �ִ� ������Ʈ���� �����ϸ鼭 �̹� ���� �ִ�.
	// ���⼭ �� �����Ϸ��� �ϸ� �̹� ���ŵǾ� �ִ� �޸𸮿� �����ؼ� ������.

	// ���͸� Ŭ����
	m_PhysicsActors_V.clear();
}

/// <summary>
/// ��������� ��������� (�����), pvd������ �۵���
/// ���� ��Ÿ�� �߿� physicsActor�� �������� ����������� Ȯ���ϱ����� �������
/// </summary>
/// <param name="transform"></param>
/// <param name="size"></param>
void PhysicsEngine::CreateTestBox(Transform* transform, float size)
{
	physx::PxTransform _pxTr = CreatePxTransform(*transform);

	physx::PxShape* _shape = g_pPhysics->createShape(physx::PxBoxGeometry(size, size, size), *g_pMaterial);
	//physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*g_pPhysics, physx::PxPlane(0, 1, 0, 0), *g_pMaterial); // ����
	physx::PxRigidStatic* _body = physx::PxCreateStatic(*g_pPhysics, _pxTr, *_shape);
	//physx::PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pxTr);

	_body->attachShape(*_shape);

	g_pNowScene->addActor(*_body);
}
/// <summary>
/// ����� (���� �Ⱦ�)
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

	/// �Ʒ� �ڵ�� �ȸ�������� why?
	//physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*g_pPhysics, _pxTr, *_nowPxGeo, *g_pMaterial, 10.0f); 

	/// �Ʒ� �ڵ�� ���� �������
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
/// ����� (���� �Ⱦ�)
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
/// Managers���� ���� (���� isPhysicsScene�̸� �˾Ƽ� ���)
/// Tolerance�ϳ����ϸ� �״�� ��
/// </summary>
/// <param name="scene"></param>
void PhysicsEngine::AddPhysicsScene(SceneBase* scene)
{
	physx::PxCudaContextManagerDesc cudaContextManagerDesc;
	//������ �ù���
	g_CudaContextManager = PxCreateCudaContextManager(*g_pFoundation, cudaContextManagerDesc, PxGetProfilerCallback());

	m_BaseEvent = new BaseEventCallBack();

	g_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);


	static bool _isFirstScene = true;


	physx::PxSceneDesc _sceneDesc(g_pPhysics->getTolerancesScale());
	_sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);		// �߷°�
	//_sceneDesc.gravity = physx::PxVec3(0.0f, -0.01, 0.0f);	// ������ -9.81 �߷°��� ���ڱ� �浹ó���� �����ʰ� 
																// �ٴڹ����� �޿� �������� ���װ� ���ܼ� �ӽ÷� ������
																// �������� �и���� �߻��� �� ����..

	/// �̿밡���� CPU�� ������ ������ �������� �������Լ��� �Ẹ��
	_sceneDesc.cpuDispatcher = g_pDispatcher;
	_sceneDesc.simulationEventCallback = m_BaseEvent;

	//_sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; /// �����𸣰ڴ�...
	_sceneDesc.filterShader = contactReportFilterShader;
	_sceneDesc.cudaContextManager = g_CudaContextManager;
	//_sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	_sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eDEFAULT;

	//_sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	//_sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;		// ������ -> ����� ���ϴ� �Ѿ˰��� ������� �����̴°Ÿ� �𸦱�..
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
/// �˾Ƽ� �ǰԲ� �����ʿ�
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

	// �������Ұ�
	//g_pNowScene->ResetPhysics();
	//Managers::GetInstance()->GetCameraManager()->RemoveAllCamera();
	g_pNowScene = _scene->second;
	//m_NowScene->Initialize();

	// userIndex �ʱ�ȭ
	PhysicsActor::s_UserIndex = 0;
}

/// <summary>
/// physicsActor�� �޾ƿͼ� phsyx Scene�� �־���
/// shape, rigid type���� �з�
/// </summary>
/// <param name="object">������Ʈ �����ڿ��� Geo, shape���� ������</param>
void PhysicsEngine::AddPhysicsActor(PhysicsActor* object)
{
	// RigidType üũ
	switch (object->GetRigidType())
	{
	case Static: // ���Կ� ������ ���� ����
	{
		// RigidStatic ����
		physx::PxRigidStatic* _body = CreateRigidStatic(object);

		/// ���� ���ͺ��Ϳ� �߰�
		g_pNowScene->addActor(*_body);
		m_PhysicsActors_V.push_back(object);
		break;
	}

	case Dynamic: // ������ �������� ����
	default:
	{
		// RigidDynamic ����
		physx::PxRigidDynamic* _body = CreateRigidDynamic(object);

		// ���� ���Ϳ� �߰�
		g_pNowScene->addActor(*_body);
		m_PhysicsActors_V.push_back(object);

		// �� ���� ����(Freeze) ����
		// ������ ���� ��, ���Ϳ� �߰��� �� ���Ŀ� �̷������ ��.
		SetRigidDynamicLockFlags(object, object->GetFreezePositionAndRotation());

		break;
	}
	}
}


/// <summary>
/// ����(Ŭ��)�� Ʈ���������� ���������� Ʈ�������� ����
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
/// �������� Ʈ�������� ������ Ŭ���� Ʈ�������� ������Ʈ
/// </summary>
/// <param name="actor">���͸� �޾ƿͼ� ������Ʈ</param>
void PhysicsEngine::UpdatePhysicsTransform(PhysicsActor* actor)
{
	PxRigidActor* _pxRigidActor = FindPxRigidActor(actor);
	if (_pxRigidActor)
	{
		UpdateTransformToPhysics(actor->m_Transform, _pxRigidActor->getGlobalPose());
	}

}

/// <summary>
/// Ʈ�������� �ٲ���
/// </summary>
/// <param name="object">������ ������Ʈ</param>
/// <param name="pos">�ٲ� Ʈ������</param>
void PhysicsEngine::SetTransform(PhysicsActor* actor, const Transform& trn)
{
	// ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
	//std::vector<physx::PxRigidActor*> _actor_V = GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

	physx::PxRigidDynamic* _dynamicActor = (physx::PxRigidDynamic*)GetPxRigidActor(m_PxRigidActor, actor->GetUserIndex());


	_dynamicActor->setGlobalPose(CreatePxTransform(trn));
}

/// <summary>
/// ���� �ӵ� �ִ� �Լ� (AddForce������)
/// </summary>
/// <param name="object">������ ������Ʈ</param>
/// <param name="vel">������ �ӵ�</param>
void PhysicsEngine::SetVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel)
{
	// ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
	//GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC, m_PxRigidActor.data());
	/// �̸� �޾Ƽ� �� ����� ������ �����غ���
	physx::PxRigidDynamic* _dynamicActor = (physx::PxRigidDynamic*)GetPxRigidActor(m_PxRigidActor, actor->GetUserIndex());

	if (_dynamicActor)
	{
		// �⺻ MoveSpeed�� Ʈ������ �̵��� ���߾� �����Ǿ� ����.
		// �̸� �״�� �����̵��� �����ϸ� ����� �������� �ʾƼ� �̵����� offset�� �������ش�.
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
/// ���� ȸ���ӵ� �ִ� �Լ� (AddForce������)
/// </summary>
/// <param name="object">������ ������Ʈ</param>
/// <param name="vel">������ ���ӵ�</param>
void PhysicsEngine::SetAngularVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel)
{
	// ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
	/// �̸� �޾Ƽ� �� ����� ������ �����غ���
	physx::PxRigidDynamic* _dynamicActor = (physx::PxRigidDynamic*)GetPxRigidActor(m_PxRigidActor, actor->GetUserIndex());

	_dynamicActor->setAngularVelocity(CreatePxVec3(vel));
}

/// <summary>
/// ���ӿ�����Ʈ ��������
/// </summary>
/// <param name="index">�ε������ؼ�</param>
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
/// ���������� ��������
/// </summary>
/// <param name="index">�ε��� ���ؼ�</param>
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
/// ���ӿ�����Ʈ�κ��� �ε��� ��������
/// </summary>
/// <param name="object"></param>
/// <returns></returns>
unsigned int PhysicsEngine::GetIndexFromGameObject(GameObject* object)
{
	return object->GetComponent<PhysicsActor>()->GetUserIndex();
}

/// <summary>
/// ���������ͷκ��� �ε��� ��������
/// </summary>
/// <param name="actor"></param>
/// <returns></returns>
unsigned int PhysicsEngine::GetIndexFromPhysicsActor(PhysicsActor* actor)
{
	return actor->GetUserIndex();
}

/// collision event�� �ʿ������ ��
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
//   /// �۵��� ���Ѵ�... ��...?
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
/// ����ĳ��Ʈ�� ���� ���ӿ�����Ʈ�� ������
/// </summary>
/// <param name="origin">��������</param>
/// <param name="dir">���� ����</param>
/// <param name="dist">���� ����</param>
/// <returns>���̿� ���� ���ӿ�����Ʈ</returns>
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
/// ����ĳ��Ʈ��Ʈ�� ���� ���� ��������
/// </summary>
/// <param name="origin">��������</param>
/// <param name="dir">���� ����</param>
/// <param name="dist">���� ����</param>
/// <returns>����ĳ��Ʈ��Ʈ����</returns>
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
		return RaycastHit(); // �󲮵���
	}
	else
	{
		RaycastHit _hitData;
		_hitData.actor = GetPhysicsActorByIndex(reinterpret_cast<int>(_rayHit.actor->userData));
		_hitData.faceIndex = _rayHit.faceIndex;
		_hitData.flag = CreateHitFlag(physx::PxHitFlag::ePOSITION); // �ϴ� ����������
		_hitData.position = CreateVector3(_rayHit.position);
		_hitData.normal = CreateVector3(_rayHit.normal);
		_hitData.distance = _rayHit.distance;
		_hitData.u = _rayHit.u;
		_hitData.v = _rayHit.v;

		return _hitData;
	}
}

/// <summary>
/// PxScene���� ���� ������ PhysicsActor�� �ش��ϴ� PxActor(PxRigidStatic/PxRigidDynamic)�� 
/// ã�Ƽ� �����Ѵ�.
/// </summary>
/// <param name="actor">���� ������ ������ ����</param>
physx::PxRigidActor* PhysicsEngine::FindPxRigidActor(PhysicsActor* actor)
{
	// ���� PxScene�� ���Ͱ� � �ִ��� ī��Ʈ�Ѵ�.
	uint32_t _actorNum = g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

	// PxActor ī��Ʈ��ŭ ���� �Ҵ�
	std::vector<physx::PxRigidActor*> _actor_V(_actorNum);

	// ���� PxScene �� ��� PxActor �����͸� ����.
	g_pNowScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC,
		reinterpret_cast<physx::PxActor**>(_actor_V.data()), _actorNum);

	// ���͸� ��ȸ�ϸ鼭 ã�����ϴ� userData(index)�� �ش��ϴ� PxActor�� ã�´�.
	for (auto pxRigidActor : _actor_V)
	{
		unsigned int _userData = reinterpret_cast<unsigned int>(pxRigidActor->userData);

		// �ش��ϴ� PxRigidActor�� ã��!
		if (_userData == actor->GetUserIndex())
		{
			return pxRigidActor;
		}
	}

	// �� ã�Ҵ�.
	return nullptr;
}
/// <summary>
/// ������ pxActor�� �����ϰ� �� pxActor�� ����
/// ���� ������ PhysicsActor �� userIndex(userData)�� �����Ѵ�.
/// -> ���ӿ�����Ʈ�� ���ΰ� �ݶ��̴�(pxActor)�� �ٲٰ� ������(������ ���� ��..)
/// </summary>
void PhysicsEngine::ChangePxRigidActor(PhysicsActor* actor)
{
	/// PhysicsActor�� ����� PxActor�� ã�´�.
	// ���� PxActor ����
	physx::PxRigidActor* _pxRigidActor = FindPxRigidActor(actor);
	if (_pxRigidActor != nullptr)
	{
		_pxRigidActor->release();
	}

	/// �� PxActor ����
	// RigidType üũ
	switch (actor->GetRigidType())
	{
	case Static:
	{
		// RigidStatic ����
		physx::PxRigidStatic* _body = CreateRigidStatic(actor);

		// ���� RigidStatic �߰�
		g_pNowScene->addActor(*_body);

		// PhysicsActor �� �̹� m_PhysicsActors_V �� �� �����Ƿ� �� �߰����� �ʴ´�.
	}

	case Dynamic:
	default:
	{
		// RigidDynamic ����
		physx::PxRigidDynamic* _body = CreateRigidDynamic(actor);

		// ���� RigidDynamic �߰�
		g_pNowScene->addActor(*_body);

		// PhysicsActor �� �̹� m_PhysicsActors_V �� �� �����Ƿ� �� �߰����� �ʴ´�.

		// �� ���� ����(Freeze) ����
		// ������ ���� ��, ���Ϳ� �߰��� �� ���Ŀ� �̷������ ��.
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
/// ���� PxScene�� �ִ� Px���͵� (RigidStatic/RigidDynamic)�� �������Ѵ�.
/// </summary>
void PhysicsEngine::ReleaseAllPxActor()
{
	uint32_t _actorNum = g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
	std::vector<physx::PxRigidActor*> _actor_V(_actorNum);
	g_pNowScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<physx::PxActor**>(_actor_V.data()), _actorNum);

	for (unsigned int i = 0; i < _actorNum; i++)	// ���� ������
	{
		_actor_V[i]->release();
	}

	ReleasePhysicsActor();
}

void PhysicsEngine::GetPxRigidActors()
{
	// ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
	m_PxRigidActor.clear();
	m_PxRigidActor.resize(g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC));
	GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC, m_PxRigidActor.data());
}

/// <summary>
/// psyx Actor�� pos�� rot ���� ������ų�� �����ϴ� �Լ�
/// </summary>
/// <param name="PhysicsActor">���� ������ ����������</param>
/// <param name="freezeValue">freeze �� �� ����</param>
void PhysicsEngine::SetRigidDynamicLockFlags(PhysicsActor* actor, FreezePositionAndRotation freezeValue)
{
	// Ȥ�� static ������Ʈ�� freeze���� �����Ϸ����� ���� �׳� �����Ѵ�.
	// (static ������Ʈ�� ���ʿ� �����̴�)
	if (actor->GetRigidType() == RigidType::Static)
	{
		return;
	}

	/// ������ ����(rigidDynamic)�� physx ������ ã�´�
	physx::PxRigidDynamic* rigidDynamic = reinterpret_cast<PxRigidDynamic*>(FindPxRigidActor(actor));

	if (rigidDynamic == nullptr)
	{
		return;
	}

	// ������ �÷��װ� ����
	PxRigidDynamicLockFlags _flag;

	(freezeValue.position.x == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X : _flag &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
	(freezeValue.position.y == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y : _flag &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
	(freezeValue.position.z == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z : _flag &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;

	(freezeValue.rotation.x == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X : _flag &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
	(freezeValue.rotation.y == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y : _flag &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
	(freezeValue.rotation.z == true) ? _flag |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z : _flag &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	// ���� ���� �÷��װ����� �� ����
	rigidDynamic->setRigidDynamicLockFlags(_flag);
}

_DLL void PhysicsEngine::SetKinematic(PhysicsActor* actor, bool val)
{
	// Ȥ�� static ������Ʈ�� freeze���� �����Ϸ����� ���� �׳� �����Ѵ�.
	// (static ������Ʈ�� ���ʿ� �����̴�)
	if (actor->GetRigidType() == RigidType::Static)
	{
		return;
	}

	/// ������ ����(rigidDynamic)�� physx ������ ã�´�
	physx::PxRigidDynamic* rigidDynamic = reinterpret_cast<PxRigidDynamic*>(FindPxRigidActor(actor));

	rigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, val);
}

_DLL void PhysicsEngine::SetGravity(PhysicsActor* actor, bool val)
{
	// Ȥ�� static ������Ʈ�� freeze���� �����Ϸ����� ���� �׳� �����Ѵ�.
	// (static ������Ʈ�� ���ʿ� �����̴�)
	if (actor->GetRigidType() == RigidType::Static)
	{
		return;
	}

	/// ������ ����(rigidDynamic)�� physx ������ ã�´�
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
	// shape ����
	physx::PxShape* pxShape = pxShape = CreateGeometryAndShape(actor);;

	// ���� ������Ʈ�� Ʈ���������� PxTransform ����
	physx::PxTransform _pTransform = CreatePxTransform(*actor->m_Transform);

	// RigidStatic ����
	physx::PxRigidStatic* _body = g_pPhysics->createRigidStatic(_pTransform);
	_body->userData = reinterpret_cast<void*>(actor->GetUserIndex());

	// shape ����
	_body->attachShape(*pxShape);

	return _body;
}

physx::PxRigidDynamic* PhysicsEngine::CreateRigidDynamic(const PhysicsActor* actor)
{
	// shape ����
	physx::PxShape* pxShape = pxShape = CreateGeometryAndShape(actor);;

	// ���� ������Ʈ�� Ʈ���������� PxTransform ����
	physx::PxTransform _pTransform = CreatePxTransform(*actor->m_Transform);

	// RigidDynamic ����
	PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pTransform);
	_body->userData = reinterpret_cast<void*>(actor->GetUserIndex());
	_body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, actor->IsKinematic());
	_body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !actor->IsGravity());

	physx::PxRigidBodyExt::updateMassAndInertia(*_body, 10.0f);

	// shape ����
	_body->attachShape(*pxShape);

	return _body;
}
