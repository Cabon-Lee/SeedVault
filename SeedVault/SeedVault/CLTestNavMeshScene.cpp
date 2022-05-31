#include "pch.h"
#include "ResourcesPathDef.h"
#include "CameraController.h"
#include "NavMeshAgent.h"
#include "ObjectBuilder.h"
#include "Zombie_Runner_Move.h"
#include "GameManager.h"
#include "CLTestNavMeshScene.h"

CLTestNavMeshScene::CLTestNavMeshScene()
	: m_pAxis(nullptr)
	, m_pDirectionalLight(nullptr)

	, m_pGameManager(nullptr)

	, m_pPlayer(nullptr)
	, m_RightHand(nullptr)

	, m_FreeCamera(nullptr)

	, m_pGround(nullptr)

	, m_pTargetObject_1(nullptr)
	, m_pTargetObject_2(nullptr)

	, m_InGameUI(nullptr)
{
}

CLTestNavMeshScene::~CLTestNavMeshScene()
{
}

void CLTestNavMeshScene::Initialize()
{
	//Scene::Initialize();

	/// <summary>
	/// 처음 네비메쉬를 생성하는 부분
	/// </summary>
	//DLLNavMesh::AddNavMeshByResourceName("navmesh_008fbx", "navmesh_008fbx.bin");
	DLLNavMesh::AddNavMeshByResourceName("TestNevy", "TestNevy.bin");
	DLLNavMesh::SetCurrentNavMeshByName("TestNevy");
	////////////////////////////////////////////////////////////////////

	/// Directional Light
	m_pDirectionalLight = DLLEngine::CreateObject(this);
	m_pDirectionalLight->SetObjectName("Directional Light");

	Light* _nowLight = new Light();
	_nowLight->m_CastShadow = true;

	m_pDirectionalLight->AddComponent<Light>(_nowLight);
	//*/
	////////////////////////////////////////////////////////////////////

	m_pGameManager = DLLEngine::CreateObject(this);
	m_pGameManager->SetObjectName("GameManager");
	m_pGameManager->AddComponent<GameManager>(new GameManager());

	/// Ground Cube
	{
		m_pGround = DLLEngine::CreateObject(this);
		m_pGround->SetObjectName("Ground");

		m_pGround->m_Transform->SetPosition({ 0.0f, -0.3f, 0.0f });
		m_pGround->m_Transform->SetScale({ 50.0f, 1.0f, 50.0f });

		MeshFilter* m_TestCubeMesh = new MeshFilter();
		m_TestCubeMesh->SetMesh("0_APlane.bin");
		m_pGround->AddComponent<MeshFilter>(m_TestCubeMesh);

		//MeshRenderer
		m_pGround->AddComponent<MeshRenderer>(new MeshRenderer);
		m_pGround->GetComponent<MeshRenderer>()->SetGizmo(false);

		PhysicsActor* _GroundPhysActor = new PhysicsActor({ 100.0f, 0.2f, 100.0f }, RigidType::Dynamic);
		_GroundPhysActor->SetGravity(false);
		_GroundPhysActor->SetKinematic(true);
		m_pGround->AddComponent(_GroundPhysActor);
	}


	/// Target Cube
	{
		/*
		// 1
		m_pTargetObject_1 = DLLEngine::CreateObject(this);
		m_pTargetObject_1->SetObjectName("Target Cube1");

		m_pTargetObject_1->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pTargetObject_1->m_Transform->SetScale({ 0.3f, 0.3f, 0.3f });

		MeshFilter* m_TestCubeMesh = new MeshFilter();
		m_TestCubeMesh->SetMesh("box_size_1.bin");
		m_pTargetObject_1->AddComponent<MeshFilter>(m_TestCubeMesh);

		//MeshRenderer
		m_pTargetObject_1->AddComponent<MeshRenderer>(new MeshRenderer);
		m_pTargetObject_1->GetComponent<MeshRenderer>()->SetGizmo(true);

		// 2
		m_pTargetObject_2 = DLLEngine::CreateObject(this);
		m_pTargetObject_2->SetObjectName("Target Cube2");

		m_pTargetObject_2->m_Transform->SetPosition({ 10.0f, 4.5f, -10.0f });
		m_pTargetObject_2->m_Transform->SetScale({ 0.012f, 0.012f, 0.012f });

		MeshFilter* m_TestCubeMesh2 = new MeshFilter();
		m_TestCubeMesh2->SetMesh("box_size_1.bin");
		m_pTargetObject_2->AddComponent<MeshFilter>(m_TestCubeMesh2);

		//MeshRenderer
		m_pTargetObject_2->AddComponent<MeshRenderer>(new MeshRenderer);
		m_pTargetObject_2->GetComponent<MeshRenderer>()->SetGizmo(true);
		*/
	}

	/// 플레이어 오브젝트
	m_pPlayer = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePlayer);
	m_pPlayer->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });

	/// 플레이어 매쉬 손 위치에 부착할 오브젝트
	m_RightHand = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eRightHand);

	/// 프리 컨트롤 카메라 (카메라 전환 테스트)
	m_FreeCamera = DLLEngine::CreateObject(this);
	m_FreeCamera->SetObjectName("FreeCamera");
	
	m_FreeCamera->AddComponent<Camera>(new Camera(m_FreeCamera->GetObjectName()));

	GameObject* _MainCamera = DLLEngine::FindGameObjectByName("MainCamera");
	m_FreeCamera->m_Transform->SetPosition(_MainCamera->m_Transform->GetWorldPosition());

	m_FreeCamera->AddComponent<CameraController>(new CameraController());

	for (uint i = 0; i < 10; i++)
	{
		GameObject* pMonster = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner);
		pMonster->GetComponent<Zombie_Runner_Move>()->m_DetectionRange = 200.0f;
		pMonster->GetComponent<Zombie_Runner_Move>()->m_ViewSight.angle = 350.0f;
		pMonster->m_Transform->SetPosition({ static_cast<float>(i * 1.5f), 0.0f, 0.0f });
	}

	/// UI
	m_InGameUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eInGameUI);

}

void CLTestNavMeshScene::Update(float dTime)
{
	//if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_1)))
	//{
	//	DLLNavMesh::SetCurrentNavMeshByName("navmesh_007fbx");
	//}
	//
	//if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_2)))
	//{
	//	DLLNavMesh::SetCurrentNavMeshByName("navmesh_008fbx");
	//}

	//if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_Z)))
	//{
	//	//m_pNavMeshAgent_01->GetComponent<NavMeshAgent>()->SetDestination(m_pTargetObject_1);
	//}
	//
	//if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_X)))
	//{
	//	//m_pNavMeshAgent_01->GetComponent<NavMeshAgent>()->SetDestination(m_pTargetObject_2);
	//}
	//
	//if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_SPACE)))
	//{
	//	m_pNavMeshAgent_01->GetComponent<NavMeshAgent>()->SetDestination(nullptr);
	//}
	//
	//if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_X)))
	//{
	//	m_pNavMeshAgent_01->GetComponent<NavMeshAgent>()->SetDestination(m_pTargetObject_1);
	//}

}

void CLTestNavMeshScene::Reset()
{
}
