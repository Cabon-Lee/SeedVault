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
	: //m_pResourceManager(nullptr)
	m_pGameManager(nullptr)
	, m_pAxis(nullptr)

	, m_pNavMeshObject_1(nullptr)
	, m_pNavMeshObject_2(nullptr)

	, m_pGround(nullptr)

	, m_pPlayer(nullptr)
	, m_RightHand(nullptr)

	, m_FreeCamera(nullptr)

	, m_pTestCube_1(nullptr)
	, m_pTestCube_2(nullptr)

	, m_pZombie_Runner_1(nullptr)
	, m_pMonster_2(nullptr)
	, m_pMonster_3(nullptr)

	/// UI
	, m_InGameUI(nullptr)
{

}

CLTestSceneYH::~CLTestSceneYH()
{
}


void CLTestSceneYH::Initialize()
{
	/// <summary>
	///  배치된 씬파일 로드
	/// </summary>
	Scene::Initialize();

	/// <summary>
	/// 처음 네비메쉬를 생성하는 부분
	/// </summary>
	DLLNavMesh::AddNavMeshByResourceName("navmesh_008", "navmesh_008.bin");
	DLLNavMesh::SetCurrentNavMeshByName("navmesh_008");

	/// GameManager
	m_pGameManager = DLLEngine::CreateObject(this);
	m_pGameManager->SetObjectName("GameManager");
	m_pGameManager->AddComponent<GameManager>(new GameManager());

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
		m_pTestCube_1 = DLLEngine::CreateObject(this);
		m_pTestCube_1->SetObjectName("Test Cube1");
		DLLEngine::SetTag("Wall", m_pTestCube_1);

		m_pTestCube_1->m_Transform->SetPosition({ 0.0f, 0.0f, 15.0f });
		m_pTestCube_1->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });

		MeshFilter* m_TestCubeMesh = new MeshFilter();
		m_TestCubeMesh->SetMesh("box_size_1.bin");
		m_pTestCube_1->AddComponent<MeshFilter>(m_TestCubeMesh);

		//MeshRenderer
		//m_pTestCube_1->AddComponent<MeshRenderer>(new MeshRenderer);
		//m_pTestCube_1->GetComponent<MeshRenderer>()->SetGizmo(false);

		PhysicsActor* _CubePhysActor = new PhysicsActor({ 10.15f,  10.0f, 2.0f }, RigidType::Dynamic);
		_CubePhysActor->SetGravity(false);
		_CubePhysActor->SetKinematic(true);
		m_pTestCube_1->AddComponent(_CubePhysActor);

		_CubePhysActor->SetFreezeRotation(true, true, true);
		_CubePhysActor->SetFreezePosition(false, true, false);

		m_pTestCube_1->AddComponent<Health>(new Health());

		// 2
		m_pTestCube_2 = DLLEngine::CreateObject(this);
		m_pTestCube_2->SetObjectName("Test Cube2");
		DLLEngine::SetTag("Wall", m_pTestCube_2);

		m_pTestCube_2->m_Transform->SetPosition({ 0.0f, 4.0f, 5.0f });
		m_pTestCube_2->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });

		MeshFilter* m_TestCubeMesh2 = new MeshFilter();
		m_TestCubeMesh2->SetMesh("box_size_1.bin");
		m_pTestCube_2->AddComponent<MeshFilter>(m_TestCubeMesh2);

		//MeshRenderer
		m_pTestCube_2->AddComponent<MeshRenderer>(new MeshRenderer);
		//m_pTestCube_2->GetComponent<MeshRenderer>()->SetGizmo(false);

		//PhysicsActor* _CubePhysActor2 = new PhysicsActor({ 1.0f, 1.0f, 1.0f }, RigidType::Dynamic);
		PhysicsActor* _CubePhysActor2 = new PhysicsActor(0.2f, RigidType::Dynamic);
		//_CubePhysActor2->SetGravity(false);
		//_CubePhysActor2->SetKinematic(true);
		m_pTestCube_2->AddComponent(_CubePhysActor2);

		m_pTestCube_2->AddComponent<Health>(new Health());

	}

	////////////////////////////////////////////////////////////////////

	/// 플레이어 오브젝트
	m_pPlayer = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePlayer);
	m_pPlayer->m_Transform->SetPosition({ -13.635f, 1.741f, 3.072f });

	/// 플레이어 매쉬 손 위치에 부착할 오브젝트
	m_RightHand = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eRightHand);

	/// 프리 컨트롤 카메라 (카메라 전환 테스트)
	m_FreeCamera = DLLEngine::CreateObject(this);
	m_FreeCamera->SetObjectName("FreeCamera");

	m_FreeCamera->AddComponent<Camera>(new Camera(m_FreeCamera->GetObjectName()));

	GameObject* _MainCamera = DLLEngine::FindGameObjectByName("MainCamera");
	m_FreeCamera->m_Transform->SetPosition(_MainCamera->m_Transform->GetWorldPosition());

	m_FreeCamera->AddComponent<CameraController>(new CameraController());
	//DLLEngine::SetNowCamera("SubCamera");

	/////////////////////////////////////////////////////////////////////


	// WayPoint 2
	GameObject* _pWayPoint_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	_pWayPoint_2->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });

	// WayPoint 3
	GameObject* _pWayPoint_3 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	_pWayPoint_3->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });

	// WayPoint 4
	GameObject* _pWayPoint_4 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	_pWayPoint_4->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });

	// WayPoint 5
	GameObject* _pWayPoint_5 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	_pWayPoint_5->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });

	// WayPoint 6
	GameObject* _pWayPoint_6 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
	_pWayPoint_6->m_Transform->SetPosition({ -5.0f, 0.0f, 3.0f });



	/////////////////////////////////////////////////////////////////////

	/// 몬스터 1
	{
		// WayPoint 1
		GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint);
		_pWayPoint_1->m_Transform->SetPosition({ -8.989f, 0.0f, -3.117f });

		m_pZombie_Runner_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner);
		//m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);
		//m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_2);
		//m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_3);
		//m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_4);

		m_pZombie_Runner_1->m_Transform->SetPosition({ -8.989f, 0.489f, -3.117f });
		m_pZombie_Runner_1->m_Transform->SetRotationFromVec({ 0.0f, 127.649f, 0.0f });
	}
	


	/////////////////////////////////////////////////////////////////////

	/*
	// 몬스터 2
	{
		m_pMonster_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner);
		m_pMonster_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_2);
		m_pMonster_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_3);
		m_pMonster_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_4);
		m_pMonster_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(m_pWayPoint_1);

	}

	// 몬스터 3
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
	// 최상위 UI 부모 오브젝트 Canvas 같은거
	m_InGameUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eInGameUI);



	/////////////////////////////////////////////////////////////////////


}

void CLTestSceneYH::Update(float dTime)
{
	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F11)))
	{
		DLLNavMesh::SetCurrentNavMeshByIndex(0);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F12)))
	{
		DLLNavMesh::SetCurrentNavMeshByIndex(1);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F3)))
	{
		/// 테스트중...
		//GameObject* _selected = DLLEngine::GetEngine()->m_SelectedObject;
		//m_FreeCamera->m_Transform->LookAtPitchAndYaw(_selected->m_Transform->m_WorldPosition);
		//
		//CameraController* ct = m_FreeCamera->GetComponent<CameraController>();
		//ct->SetRotationAngles(_selected->m_Transform->m_Position);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F4)))
	{

		m_pTestCube_2->m_Transform->SetPosition({ m_pPlayer->m_Transform->m_Position.x + 0.2f, m_pPlayer->m_Transform->m_Position.y , m_pPlayer->m_Transform->m_Position.z + 0.5f });


		//_dir
		SimpleMath::Vector3 dir = { 0.0f, 0.8f, 1.0f };
		//dir *= 5.0f;
		m_pTestCube_2->GetComponent<PhysicsActor>()->SetVelocity(dir);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F5)))
	{
		m_pTestCube_2->GetComponent<PhysicsActor>()->SetVelocity({ 0.0f, 0.0f, 0.0f });
		//m_pTestCube_2->m_Transform->SetPosition({0.0f, 5.0f, 0.0f});
	}


	//m_pPlayer->GetComponent<NavMeshAgent>()->RenderPathAgentToDestination();


	// physicsActor 변경 테스트
	/*
	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_Y)))
	{
		m_pZombie_Runner_1->GetComponent<PhysicsActor>()->SetBoxCollider({ 3.0f, 5.0f, 1.0f }, RigidType::Dynamic);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_U)))
	{
		m_pZombie_Runner_1->GetComponent<PhysicsActor>()->SetBoxCollider({ 2.0f, 8.0f, 1.0f }, RigidType::Dynamic);
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
