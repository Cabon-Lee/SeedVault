#include "pch.h"

#include "ResourcesPathDef.h"
#include "GameManager.h"
#include "Health.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "CameraBehavior.h"
#include "OrbitCamera.h"
#include "Zombie_Runner_Move.h"
#include "Zombie_Runner_AI.h"
#include "CLAnimation.h"

CLAnimation::CLAnimation()
	: //m_pResourceManager(nullptr)
	m_pGameManager(nullptr)
	, m_pAxis(nullptr)
	, m_pDirectionalLight(nullptr)

	, m_pGround(nullptr)

	, m_pPlayer(nullptr)
	, m_pPlayerMesh(nullptr)
	, m_pCameraParent(nullptr)
	, m_pMainCamera(nullptr)
	, m_pZoomInPosition(nullptr)

	, m_FreeCamera(nullptr)

	, m_pTestCube_1(nullptr)
	, m_pTestCube_2(nullptr)

	, m_pWayPoint_1(nullptr)
	, m_pWayPoint_2(nullptr)
	, m_pWayPoint_3(nullptr)
	, m_pWayPoint_4(nullptr)

	, m_pZombie_Runner_1(nullptr)
	, m_pMonster_1_Mesh(nullptr)
	, m_pZombie_Runner_2(nullptr)
	, m_pMonster_2_Mesh(nullptr)

	/// UI
	// Text
	, m_txtMonster_1_Name(nullptr)
	, m_txtMonster_1_State(nullptr)

	// Sprite
	, m_sprDot(nullptr)

	// Button
	, m_Btn_1(nullptr)
{

}

CLAnimation::~CLAnimation()
{
}


void CLAnimation::Initialize()
{
	
	/// GameManager
	m_pGameManager = DLLEngine::CreateObject(this);
	m_pGameManager->SetObjectName("GameManager");
	m_pGameManager->AddComponent<GameManager>(new GameManager());

	/// Axis
	m_pAxis = DLLEngine::CreateObject(this);
	m_pAxis->SetObjectName("YH_Axis");

	m_pAxis->m_Transform->SetPosition({ 0.0f, -3.0f, 0.0f });

	MeshFilter* _AxisMesh = new MeshFilter();
	m_pAxis->AddComponent<MeshFilter>(_AxisMesh);

	MaterialComponent* _AxisMat = new MaterialComponent();
	_AxisMat->SetVertexShaderName("vs_stnd.cso");
	_AxisMat->SetPixelShaderName("ps_stnd.cso");
	m_pAxis->AddComponent<MaterialComponent>(_AxisMat);

	m_pAxis->AddComponent<MeshRenderer>(new MeshRenderer);
	//m_pAxis->GetComponent<MeshRenderer>()->SetGizmo(false);
	//*/
	////////////////////////////////////////////////////////////////////

	/// Directional Light
	m_pDirectionalLight = DLLEngine::CreateObject(this);
	m_pDirectionalLight->SetObjectName("Directional Light");

	Light* _nowLight = new Light();
	_nowLight->m_CastShadow = true;

	m_pDirectionalLight->AddComponent<Light>(_nowLight);
	//*/
	////////////////////////////////////////////////////////////////////

	/// Ground Cube
	{
		m_pGround = DLLEngine::CreateObject(this);
		m_pGround->SetObjectName("Ground");

		m_pGround->m_Transform->SetPosition({ 0.0f, -3.0f, 0.0f });
		//m_pGround->m_Transform->SetScale({ 5.0f, 5.5f, 5.5f });

		MeshFilter* m_TestCubeMesh = new MeshFilter();
		m_TestCubeMesh->SetMesh("box_size_1.bin");
		m_pGround->AddComponent<MeshFilter>(m_TestCubeMesh);

		//MeshRenderer
		m_pGround->AddComponent<MeshRenderer>(new MeshRenderer);
		m_pGround->GetComponent<MeshRenderer>()->SetGizmo(false);

		PhysicsActor* _GroundPhysActor = new PhysicsActor({ 100.0f, 0.01f, 100.0f }, RigidType::Dynamic);
		_GroundPhysActor->SetGravity(false);
		_GroundPhysActor->SetKinematic(true);
		m_pGround->AddComponent(_GroundPhysActor);
	}
	////////////////////////////////////////////////////////////////////

	/// RayTest Cube
	{
		// 1
		m_pTestCube_1 = DLLEngine::CreateObject(this);
		m_pTestCube_1->SetObjectName("Test Cube1");

		m_pTestCube_1->m_Transform->SetPosition({ 0.0f, 0.0f, 20.0f });
		//m_pTestCube->m_Transform->SetScale({ 5.0f, 5.5f, 5.5f });

		MeshFilter* m_TestCubeMesh = new MeshFilter();
		m_TestCubeMesh->SetMesh("box_size_1.bin");
		m_pTestCube_1->AddComponent<MeshFilter>(m_TestCubeMesh);

		//MeshRenderer
		m_pTestCube_1->AddComponent<MeshRenderer>(new MeshRenderer);
		m_pTestCube_1->GetComponent<MeshRenderer>()->SetGizmo(false);

		PhysicsActor* _CubePhysActor = new PhysicsActor({ 1.0f, 1.0f, 1.0f }, RigidType::Dynamic);
		//_CubePhysActor->SetGravity(false);
		_CubePhysActor->SetKinematic(true);
		m_pTestCube_1->AddComponent(_CubePhysActor);

		m_pTestCube_1->AddComponent<Health>(new Health());

		// 2
		m_pTestCube_2 = DLLEngine::CreateObject(this);
		m_pTestCube_2->SetObjectName("Test Cube2");

		m_pTestCube_2->m_Transform->SetPosition({ 10.0f, 4.5f, 20.0f });
		//m_pTestCube->m_Transform->SetScale({ 5.0f, 5.5f, 5.5f });

		MeshFilter* m_TestCubeMesh2 = new MeshFilter();
		m_TestCubeMesh2->SetMesh("box_size_1.bin");
		m_pTestCube_2->AddComponent<MeshFilter>(m_TestCubeMesh2);

		//MeshRenderer
		m_pTestCube_2->AddComponent<MeshRenderer>(new MeshRenderer);
		m_pTestCube_2->GetComponent<MeshRenderer>()->SetGizmo(false);

		PhysicsActor* _CubePhysActor2 = new PhysicsActor({ 3.0f, 1.0f, 3.0f }, RigidType::Dynamic);
		//_CubePhysActor2->SetGravity(false);
		//_CubePhysActor2->SetKinematic(true);
		m_pTestCube_2->AddComponent(_CubePhysActor2);

		m_pTestCube_2->AddComponent<Health>(new Health());
	}



	////////////////////////////////////////////////////////////////////

	/// 플레이어 오브젝트
	{
		m_pPlayer = DLLEngine::CreateObject(this);
		m_pPlayer->SetObjectName("Player");

		PhysicsActor* _PlayerPhysActor = new PhysicsActor({ 0.15f, 0.45f, 0.15f }, RigidType::Dynamic);
		//_PlayerPhysActor->SetGravity(false);
		//_PlayerPhysActor->SetKinematic(true);
		m_pPlayer->AddComponent(_PlayerPhysActor);
		_PlayerPhysActor->SetFreezeRotation(true, false, true);
		_PlayerPhysActor->SetFreezePosition(false, true, false);

		m_pPlayer->m_Transform->SetPosition({ 0.0f, 0.3f, 5.0f });
		m_pPlayer->m_Transform->SetRotationFromVec({ 0.0f, 0.0f, 0.0f });

		// [PlayerController]
		PlayerController* _playerController = new PlayerController();
		m_pPlayer->AddComponent<PlayerController>(_playerController);

		// [Health]
		Health* _playerHealth = new Health();
		m_pPlayer->AddComponent<Health>(_playerHealth);

		/// 임시 테스트용 text 변수
		Text* text;
		text = new Text();
		text->SetSpriteTextInfo(
			L"../Data/Fonts/HYgothic.ttf",
			1100.0f,	// x
			110.0f,		// y
			1.0f,		// R
			0.0f, 		// G
			0.0f,		// B
			1.0f,		// A
			300.0f,		// lineWidth
			20.0f,		// lineSpace
			20.0f,		// fontSize
			eUIAxis::None, 
			eTextPoint::LeftUP
		);
		m_pPlayer->AddComponent<Text>(text);


		////////////////////////////////////////////////////////////////////////////////////////////////////

		/// 플레이어 메쉬 오브젝트(플레이어이 자식으로 들어감)
		{

			m_pPlayerMesh = DLLEngine::CreateObject(this);
			m_pPlayerMesh->SetObjectName("Mesh");

			m_pPlayerMesh->m_Transform->SetPosition({ 0.0f, -0.5f, 0.0f });
			m_pPlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
			m_pPlayerMesh->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });	// 모델이 너무 커서 스케일 조정..

			MeshFilter* _Mesh = new MeshFilter();
			_Mesh->SetMesh(CL::ResourcePath::MESH_PLAYER_RIFLE);
			m_pPlayerMesh->AddComponent<MeshFilter>(_Mesh);

			//MeshRenderer
			m_pPlayerMesh->AddComponent<MeshRenderer>(new MeshRenderer);
			m_pPlayerMesh->GetComponent<MeshRenderer>()->SetGizmo(false);

			// [Animator]
			// 애니메이터 생성
			Animator* _playerAnimator = new Animator();

			// String
			const std::string Movement = "Movement";
			const std::string Stand_Idle = "Stand_Idle";
			const std::string Stand_Walk_Forward = "Stand_Walk_Forward";
			const std::string Stand_Walk_Backward = "Stand_Walk_Backward";
			const std::string Stand_Walk_Left = "Stand_Walk_Left";
			const std::string Stand_Walk_Right = "Stand_Walk_Right";
			const std::string Stand_Sprint = "Stand_Sprint";
			const std::string Stand_Die = "Stand_Die";

			const std::string Crouch_Idle = "Crouch_Idle";
			const std::string Crouch_Walk_Forward = "Crouch_Walk_Forward";
			const std::string Crouch_Walk_Backward = "Crouch_Walk_Backward";
			const std::string Crouch_Walk_Left = "Crouch_Walk_Left";
			const std::string Crouch_Walk_Right = "Crouch_Walk_Right";

			const std::string Dead = "Dead";

			// Layer 추가
			_playerAnimator->AddAnimLayer(Movement);
			{
				/// State 추가
				{

					_playerAnimator->GetAnimLayer(Movement)->m_MaskingType = eMASKING_TYPE::NONE;

					// Stand
					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Idle, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_IDLE);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Walk_Forward, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_FORWARD);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Walk_Backward, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_BACKWARD);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Walk_Left, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_LEFT);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Walk_Right, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_RIGHT);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Sprint, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_SPRINT);

					/*
					// Crouch
					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Idle, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_IDLE);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Walk_Forward, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_FORWARD);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Walk_Backward, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Walk_Left, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_LEFT);

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Walk_Right, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_RIGHT);

					// Die
					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Die, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_DIE);

					// Dead
					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Dead, CL::ResourcePath::ANIM_PLAYER_RIFLE_DIE);
					*/
				} // end of State

				/// Transition 설정
				{

					// Stand
					{
						// Stand_Idle

						{
							// Stand_Idle -> Walking
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->AddTrnasition(Stand_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(0)->m_FadingPeriod = 80;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_V, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->AddTrnasition(Stand_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(1)->m_FadingPeriod = 80;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->AddTrnasition(Stand_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(2)->m_FadingPeriod = 80;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_H, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->AddTrnasition(Stand_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(3)->m_FadingPeriod = 80;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_H, true);

							// Stand_Idle -> Sprint
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->AddTrnasition(Stand_Sprint);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(4)->m_FadingPeriod = 80;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(4)->AddParameter(&_playerController->m_bSprint, true);


						}

						// Stand_Walk_Forward
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->AddTrnasition(Stand_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(0)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_V, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_H, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_H, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->AddTrnasition(Stand_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(1)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->AddTrnasition(Stand_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(2)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->AddTrnasition(Stand_Sprint);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(3)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(3)->AddParameter(&_playerController->m_bSprint, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->AddTrnasition(Stand_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(4)->m_FadingPeriod = 80;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_V, false);
						}

						// Stand_Walk_Backward
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->AddTrnasition(Stand_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(0)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_V, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_H, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_H, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->AddTrnasition(Stand_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(1)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->AddTrnasition(Stand_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(2)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->AddTrnasition(Stand_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(3)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_V, true);

						}

						// Stand_Walk_Left
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->AddTrnasition(Stand_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(0)->m_FadingPeriod = 80;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(0)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_H, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->AddTrnasition(Stand_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(1)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->AddTrnasition(Stand_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(2)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->AddTrnasition(Stand_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(3)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->AddTrnasition(Stand_Sprint);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(4)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(4)->AddParameter(&_playerController->m_bSprint, true);

						}

						// Stand_Walk_Right
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->AddTrnasition(Stand_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(0)->m_FadingPeriod = 80;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(0)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_H, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->AddTrnasition(Stand_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(1)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->AddTrnasition(Stand_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(2)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->AddTrnasition(Stand_Walk_Backward);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(3)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->AddTrnasition(Stand_Sprint);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(4)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(4)->AddParameter(&_playerController->m_bSprint, true);

						}

						/*
						// Stand_Sprint
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Sprint)->AddTrnasition(Stand_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Sprint)->m_Transition_V->at(0)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Sprint)->m_Transition_V->at(0)->AddParameter(&_playerController->m_bSprint, false);

						}
						*/

					} // end of Stand



					/*
					// Crouch
					{
						// "Crouch_Idle"
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->AddTrnasition(Stand_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(0)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(0)->AddParameter(&_playerController->m_bCrouch, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->AddTrnasition(Crouch_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(1)->m_FadingPeriod = 10;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->AddTrnasition(Crouch_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(2)->m_FadingPeriod = 10;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->AddTrnasition(Crouch_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(3)->m_FadingPeriod = 10;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->AddTrnasition(Crouch_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(4)->m_FadingPeriod = 10;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->AddTrnasition(Stand_Sprint);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(5)->m_FadingPeriod = 10;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						}

						// Crouch_Walk_Forward
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->AddTrnasition(Crouch_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(0)->m_FadingPeriod = 3;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_V, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(0)->AddParameter(-0.001f, &_playerController->m_H, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(0)->AddParameter(0.001f, &_playerController->m_H, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->AddTrnasition(Crouch_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(1)->m_FadingPeriod = 20;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->AddTrnasition(Crouch_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(2)->m_FadingPeriod = 20;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->AddTrnasition(Crouch_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(3)->m_FadingPeriod = 30;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(3)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_V, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->AddTrnasition(Stand_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(4)->AddParameter(&_playerController->m_bCrouch, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(4)->m_FadingPeriod = 30;

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->AddTrnasition(Stand_Sprint);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(5)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						}

						// Crouch_Walk_Backward
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->AddTrnasition(Crouch_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(0)->m_FadingPeriod = 3;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_V, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(0)->AddParameter(-0.001f, &_playerController->m_H, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(0)->AddParameter(0.001f, &_playerController->m_H, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->AddTrnasition(Crouch_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(1)->m_FadingPeriod = 10;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);


							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->AddTrnasition(Crouch_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(2)->m_FadingPeriod = 10;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->AddTrnasition(Crouch_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(3)->m_FadingPeriod = 15;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(3)->AddParameter(-1.0f, &_playerController->m_VForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_V, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->AddTrnasition(Stand_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(4)->AddParameter(&_playerController->m_bCrouch, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_Transition_V->at(4)->m_FadingPeriod = 30;

						}

						// Crouch_Walk_Left
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->AddTrnasition(Crouch_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(0)->m_FadingPeriod = 3;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_H, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(0)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(0)->AddParameter(-0.001f, &_playerController->m_V, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(0)->AddParameter(0.001f, &_playerController->m_V, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->AddTrnasition(Crouch_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(1)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->AddTrnasition(Crouch_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(2)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->AddTrnasition(Crouch_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(3)->m_FadingPeriod = 30;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(3)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_H, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->AddTrnasition(Stand_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(4)->AddParameter(&_playerController->m_bCrouch, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_Transition_V->at(4)->m_FadingPeriod = 30;

						}

						// Crouch_Walk_Right
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->AddTrnasition(Crouch_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(0)->m_FadingPeriod = 3;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_H, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(0)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(0)->AddParameter(-0.001f, &_playerController->m_V, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(0)->AddParameter(0.001f, &_playerController->m_V, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->AddTrnasition(Crouch_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(1)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->AddTrnasition(Crouch_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(2)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->AddTrnasition(Crouch_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(3)->m_FadingPeriod = 30;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(3)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_H, false);


							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->AddTrnasition(Stand_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(4)->AddParameter(&_playerController->m_bCrouch, false);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_Transition_V->at(4)->m_FadingPeriod = 30;

						}

					} // end of Crouch

					*/

				} // end of Transition

			} // end of Layer[Movement]

			/// Aiming
			// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
			{
			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM)->SetEnabled(false);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM)->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM)->AddLocomotion("Rifle_Pitch", CL::ResourcePath::ANIM_PLAYER_RIFLE_PITCH, &_playerController->m_PitchValue);
			//_playerAnimator->GetAnimLayer("Rifle_Aiming")->AddStateMap("Aiming", CL::ResourcePath::ANIM_PLAYER_AIMING);
			}

			/// LongGun Reload
			// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD)->SetEnabled(false);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD)->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD)->AddStateMap("Rifle_Reload", CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_RELOAD);
				//_playerAnimator->GetAnimLayer("Rifle_Aiming")->AddStateMap("Aiming", CL::ResourcePath::ANIM_PLAYER_AIMING);
			}

			/// Swap
				// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON)->SetEnabled(false);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON)->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON)->AddStateMap("SwapWeapon", CL::ResourcePath::ANIM_PLAYER_RIFLE_SWAP_WEAPON);
			}

			m_pPlayerMesh->AddComponent<Animator>(_playerAnimator);
		}


		/// 부모로 
		m_pPlayerMesh->SetParent(m_pPlayer);


		////////////////////////////////////////////////////////////////////////////////////////////////////

		/// 메인 카메라
		{
			/// 카메라 포지션(피벗) CameraParent
			m_pCameraParent = DLLEngine::CreateObject(this);
			m_pCameraParent->SetObjectName("CameraParent");

			m_pCameraParent->AddComponent<OrbitCamera>(new OrbitCamera);
			m_pCameraParent->GetComponent<OrbitCamera>()->SetFocusTrasform(*m_pPlayer);
			////////////////////////////////////////////////////////////////////////////////////////////////////


			/// 메인 카메라 오브젝트
			{
				m_pMainCamera = DLLEngine::CreateObject(this);
				m_pMainCamera->SetObjectName("MainCamera");

				/// 부모로 
				m_pMainCamera->SetParent(m_pCameraParent);
				m_pMainCamera->m_Transform->SetPosition({ 0.6f, 0.23f, -1.3f });
				//m_pMainCamera->m_Transform->SetRotationFromVec({ 10.0f, 0.0f, 0.0f });

				Camera* _newCamera = new Camera(m_pMainCamera->GetObjectName());

				m_pMainCamera->AddComponent<Camera>(_newCamera);
				DLLEngine::SetNowCamera("MainCamera");

				// CameraBehavior
				CameraBehavior* _CameraBehavior = new CameraBehavior();
				m_pMainCamera->AddComponent<CameraBehavior>(_CameraBehavior);

				//CameraController* _CameraController = new CameraController();
				//m_MainCamera->AddComponent<CameraController>(_CameraController);

				////////////////////////////////////////////////////////////////////				
			}

			/// 조준 위치용 오브젝트
			m_pZoomInPosition = DLLEngine::CreateObject(this);
			m_pZoomInPosition->SetObjectName("ZoomInPosition");
			m_pZoomInPosition->SetParent(m_pCameraParent);

			m_pZoomInPosition->m_Transform->SetPosition({ 0.63f, 0.25f, -1.4f });
		}
	}

	/// 프리 컨트롤 카메라 (카메라 전환 테스트)
	m_FreeCamera = DLLEngine::CreateObject(this);
	m_FreeCamera->SetObjectName("FreeCamera");

	m_FreeCamera->AddComponent<Camera>(new Camera(m_FreeCamera->GetObjectName()));
	m_FreeCamera->m_Transform->SetPosition(m_pMainCamera->m_Transform->GetWorldPosition());

	m_FreeCamera->AddComponent<CameraController>(new CameraController());
	//DLLEngine::SetNowCamera("SubCamera");

}

void CLAnimation::Update(float dTime)
{



}


void CLAnimation::Reset()
{
}
