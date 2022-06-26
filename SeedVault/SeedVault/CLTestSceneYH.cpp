#include "pch.h"
#include "GameManager.h"
#include "MuzzleFlash.h"
#include "EquipmentController.h"
#include "Health.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "CameraBehavior.h"
#include "OrbitCamera.h"
#include "Zombie_Runner_Move.h"
#include "Zombie_Runner_AI.h"
#include "NavMeshAgent.h"
#include "ObjectBuilder.h"
#include "CLTestSceneYH.h"

CLTestSceneYH::CLTestSceneYH()
	: m_pGameManager(nullptr)
	, m_pAxis(nullptr)

	, m_pNavMeshObject_1(nullptr)
	, m_pNavMeshObject_2(nullptr)

	, m_pGround(nullptr)

	, m_pPlayer(nullptr)
	, m_RightHand(nullptr)

	, m_FreeCamera(nullptr)

	, m_pPartner(nullptr)

	, m_pTestCube_1(nullptr)
	, m_pTestCube_2(nullptr)

	, m_pZombie_Runner_1(nullptr)
	, m_pZombie_Runner_2(nullptr)
	, m_pMonster_3(nullptr)

	/// UI
	, m_InGameUI(nullptr)

	, m_TestCube(nullptr)
	, m_BGM_Playing(false)
{

}

CLTestSceneYH::~CLTestSceneYH()
{
}


void CLTestSceneYH::Initialize()
{
	/// <summary>
	///  ��ġ�� ������ �ε�
	/// </summary>
	Scene::Initialize();

	/// <summary>
	/// ó�� �׺�޽��� �����ϴ� �κ�
	/// </summary>
	DLLNavMesh::AddNavMeshByResourceName("navmesh_008", "navmesh_008.bin");
	DLLNavMesh::SetCurrentNavMeshByName("navmesh_008");

	/// GameManager
	m_pGameManager = DLLEngine::CreateObject(this);
	m_pGameManager->SetObjectName("GameManager");
	m_pGameManager->AddComponent<GameManager>(new GameManager());
	m_pGameManager->AddComponent<Audio>(new Audio());
	////////////////////////////////////////////////////////////////////

	/// Ground Cube
	{
		m_pGround = DLLEngine::CreateObject(this);
		m_pGround->SetObjectName("Ground");

		m_pGround->m_Transform->SetPosition({ 0.0f, -10.0f, 0.0f });
		m_pGround->m_Transform->SetScale({ 10.0f, 0.1f, 10.0f });

		MeshFilter* m_TestCubeMesh = new MeshFilter();
		m_TestCubeMesh->SetMesh("box_size_1.bin");
		m_pGround->AddComponent<MeshFilter>(m_TestCubeMesh);

		//MeshRenderer
		//m_pGround->AddComponent<MeshRenderer>(new MeshRenderer);
		//m_pGround->GetComponent<MeshRenderer>()->SetGizmo(false);

		PhysicsActor* _GroundPhysActor = new PhysicsActor({ 50.0f, 1.0f, 50.0f }, RigidType::Dynamic);
		_GroundPhysActor->SetGravity(false);
		_GroundPhysActor->SetKinematic(true);
		m_pGround->AddComponent(_GroundPhysActor);
		//_GroundPhysActor->SetFreezePosition(false, true, false);
	}
	////////////////////////////////////////////////////////////////////


	/// RayTest Cube
	{
		// 1
		//m_pTestCube_1 = DLLEngine::CreateObject(this);
		//m_pTestCube_1->SetObjectName("Test Cube1");
		//DLLEngine::SetTag("Wall", m_pTestCube_1);
		//
		//m_pTestCube_1->m_Transform->SetPosition({ 0.0f, 0.0f, 15.0f });
		//m_pTestCube_1->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });
		//
		//MeshFilter* m_TestCubeMesh = new MeshFilter();
		//m_TestCubeMesh->SetMesh("box_size_1.bin");
		//m_pTestCube_1->AddComponent<MeshFilter>(m_TestCubeMesh);
		//
		////MeshRenderer
		////m_pTestCube_1->AddComponent<MeshRenderer>(new MeshRenderer);
		////m_pTestCube_1->GetComponent<MeshRenderer>()->SetGizmo(false);
		//
		//PhysicsActor* _CubePhysActor = new PhysicsActor({ 10.15f,  10.0f, 2.0f }, RigidType::Dynamic);
		//_CubePhysActor->SetGravity(false);
		//_CubePhysActor->SetKinematic(true);
		//m_pTestCube_1->AddComponent(_CubePhysActor);
		//
		//_CubePhysActor->SetFreezeRotation(true, true, true);
		//_CubePhysActor->SetFreezePosition(false, true, false);
		//
		//m_pTestCube_1->AddComponent<Health>(new Health());

		// 2
		//m_pTestCube_2 = DLLEngine::CreateObject(this);
		//m_pTestCube_2->SetObjectName("Test Cube2");
		//DLLEngine::SetTag("Wall", m_pTestCube_2);
		//
		//m_pTestCube_2->m_Transform->SetPosition({ 0.0f, 4.0f, 5.0f });
		//m_pTestCube_2->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });
		//
		//MeshFilter* m_TestCubeMesh2 = new MeshFilter();
		//m_TestCubeMesh2->SetMesh("box_size_1.bin");
		//m_pTestCube_2->AddComponent<MeshFilter>(m_TestCubeMesh2);
		//
		////MeshRenderer
		//m_pTestCube_2->AddComponent<MeshRenderer>(new MeshRenderer);
		////m_pTestCube_2->GetComponent<MeshRenderer>()->SetGizmo(false);
		//
		////PhysicsActor* _CubePhysActor2 = new PhysicsActor({ 1.0f, 1.0f, 1.0f }, RigidType::Dynamic);
		//PhysicsActor* _CubePhysActor2 = new PhysicsActor(0.2f, RigidType::Dynamic);
		////_CubePhysActor2->SetGravity(false);
		////_CubePhysActor2->SetKinematic(true);
		//m_pTestCube_2->AddComponent(_CubePhysActor2);
		//
		//m_pTestCube_2->AddComponent<Health>(new Health());

	}

	////////////////////////////////////////////////////////////////////

	/// �÷��̾� ������Ʈ
	m_pPlayer = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePlayer);
	m_pPlayer->m_Transform->SetPosition({ -13.635f, 1.741f, 3.072f });
	/// �÷��̾� �Ž� �� ��ġ�� ������ ������Ʈ
	m_RightHand = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eRightHand);

	/// ���� ��Ʈ�� ī�޶� (ī�޶� ��ȯ �׽�Ʈ)
	m_FreeCamera = DLLEngine::CreateObject(this);
	m_FreeCamera->SetObjectName("FreeCamera");
	m_FreeCamera->AddComponent<Camera>(new Camera(m_FreeCamera->GetObjectName()));
	m_FreeCamera->GetComponent<Camera>()->SetSkyBoxTextureName("satara_night_no_lamps_8k.hdr");

	GameObject* _MainCamera = DLLEngine::FindGameObjectByName("MainCamera");
	m_FreeCamera->m_Transform->SetPosition(_MainCamera->m_Transform->GetWorldPosition());

	m_FreeCamera->AddComponent<CameraController>(new CameraController());
	///ī�޶� ����������ʸ� �ٿ��ش�.
	_MainCamera->AddComponent<AudioListener>(new AudioListener());
	//DLLEngine::SetNowCamera("SubCamera");
	/////////////////////////////////////////////////////////////////////

	/// ��Ʈ�� ������Ʈ
	m_pPartner = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePartner);
	m_pPartner->m_Transform->SetPosition({ -13.635f, 1.741f, 2.072f });
	/////////////////////////////////////////////////////////////////////


	// WayPoint 1
	GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	_pWayPoint_1->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });

	//// WayPoint 2
	//GameObject* _pWayPoint_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	//_pWayPoint_2->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });
	//
	//// WayPoint 3
	//GameObject* _pWayPoint_3 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	//_pWayPoint_3->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });
	//
	//// WayPoint 4
	//GameObject* _pWayPoint_4 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	//_pWayPoint_4->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });
	//
	//// WayPoint 5
	//GameObject* _pWayPoint_5 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	//_pWayPoint_5->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });



	/////////////////////////////////////////////////////////////////////

	/// ���� 1
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
		_pSpawnPoint->m_Transform->SetPosition({ -8.989f, 0.0f, -3.117f });

		m_pZombie_Runner_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner);
		m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);
		//m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_3);
		//m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_4);

		m_pZombie_Runner_1->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);
		m_pZombie_Runner_1->m_Transform->SetRotationFromVec({ 0.0f, 127.649f, 0.0f });
	}



	/////////////////////////////////////////////////////////////////////

	
	// ���� 2
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
		//_pSpawnPoint->m_Transform->SetPosition({ -5.0f, 0.0f, 0.0f });
		_pSpawnPoint->m_Transform->SetPosition({ -8.989f, 0.0f, -3.817f });

		m_pZombie_Runner_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner);
		m_pZombie_Runner_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Runner_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);
		//m_pZombie_Runner_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_4);
		//m_pZombie_Runner_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);

		m_pZombie_Runner_2->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);
	}
	/*
	// ���� 3
	{
		m_pMonster_3 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner);
		m_pMonster_3->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_4);
		m_pMonster_3->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_3);
		m_pMonster_3->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_2);
		m_pMonster_3->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_1);
	}
	*/

	////////////////////////////////////////////////////

	/// UI
	// �ֻ��� UI �θ� ������Ʈ Canvas ������
	m_InGameUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eInGameUI);
	
	m_PauseUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePauseUI);

	/////////////////////////////////////////////////////////////////////
	//m_TestCube = DLLEngine::CreateObject(this);
	//m_TestCube->AddComponent<PhysicsActor>(new PhysicsActor({ 1.0f, 1.0f, 1.0f }, RigidType::Dynamic));
	//m_TestCube->GetComponent<PhysicsActor>()->SetKinematic(true);


}

void CLTestSceneYH::Update(float dTime)
{
	if (!m_BGM_Playing)
	{
		m_pGameManager->GetComponent<Audio>()->PlayEvent("event:/BGM_Tutorial");
		m_BGM_Playing = true;
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F11)))
	{
		DLLNavMesh::SetCurrentNavMeshByIndex(0);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F12)))
	{
		DLLNavMesh::SetCurrentNavMeshByIndex(1);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F8)))
	{
		//m_InGameUI->GetComponent<IngameUIManager>()->DebugStringNext();
	}

	//m_pPlayer->GetComponent<NavMeshAgent>()->RenderPathAgentToDestination();


	// physicsActor ���� �׽�Ʈ
	/*
	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_Y)))
	{
		m_pPlayer->GetComponent<PhysicsActor>()->SetBoxCollider({2.0f, 2.0f, 2.0f}, RigidType::Dynamic);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_U)))
	{
		m_pPlayer->GetComponent<PhysicsActor>()->SetBoxCollider({1.0f, 1.0f, 1.0f}, RigidType::Dynamic);
	}
	*/




#pragma region QuaternionTest
	{
		//Transform* tf = m_pPlayer->m_Transform;
		//Vector3 eulerAngle = tf->m_EulerAngles;

		//tf->QuatToEulerTest();

		//Vector3 newAngle = tf->QuatToDegreeTest(eulerAngle);


		//DirectX::SimpleMath::Matrix matrix = Matrix::CreateFromQuaternion(quat);
		//matrix.Decompose()

		//Vector3 newTranslate;



		//CA_TRACE("%f / %f / %f", newAngle.x, newAngle.y, newAngle.z);
	}
#pragma endregion QuaternionTest

	//CA_TRACE("%s", DLLNavMesh::GetCurrentnavMeshName().c_str());

}


void CLTestSceneYH::Reset()
{
}
