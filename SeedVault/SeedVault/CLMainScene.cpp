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
#include "CLMainScene.h"

CLMainScene::CLMainScene()
	: m_pGameManager(nullptr)
	, m_pAxis(nullptr)
	, m_pDirectionalLight(nullptr)

	, m_pPlayer(nullptr)
	, m_pPlayerMesh(nullptr)
	, m_pCameraParent(nullptr)
	, m_pMainCamera(nullptr)
	, m_pZoomInPosition(nullptr)

	, m_FreeCamera(nullptr)
{
}

CLMainScene::~CLMainScene()
{
}

void CLMainScene::Initialize()
{
	/// 프리 컨트롤 카메라 (카메라 전환 테스트)
	m_FreeCamera = DLLEngine::CreateObject(this);
	m_FreeCamera->SetObjectName("FreeCamera");

	m_FreeCamera->AddComponent<Camera>(new Camera(m_FreeCamera->GetObjectName()));
	m_FreeCamera->AddComponent<CameraController>(new CameraController());
	//*/
	////////////////////////////////////////////////////////////////////

	/// GameManager
	m_pGameManager = DLLEngine::CreateObject(this);
	m_pGameManager->SetObjectName("GameManager");
	m_pGameManager->AddComponent<GameManager>(new GameManager());
	//*/
	////////////////////////////////////////////////////////////////////

	/// Axis
	m_pAxis = DLLEngine::CreateObject(this);
	m_pAxis->SetObjectName("YH_Axis");

	m_pAxis->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });

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

	/// 플레이어 오브젝트
	{
		m_pPlayer = DLLEngine::CreateObject(this);
		m_pPlayer->SetObjectName("Player");

		m_pPlayer->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pPlayer->m_Transform->SetRotationFromVec({ 0.0f, 0.0f, 0.0f });

		//PhysicsActor* _PlayerPhysActor = new PhysicsActor({ 1.0f, 1.0f, 1.0f }, 1);
		//m_pPlayer->AddComponent(_PlayerPhysActor);
		//DLLEngine::AddPhysicsActor(_PlayerPhysActor);

		// [PlayerController]
		PlayerController* _playerController = new PlayerController();
		m_pPlayer->AddComponent<PlayerController>(_playerController);

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

			m_pPlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
			m_pPlayerMesh->m_Transform->SetScale({ 0.05f, 0.05f, 0.05f });	// 모델이 너무 커서 스케일 조정..

			MeshFilter* _Mesh = new MeshFilter();
			_Mesh->SetMesh(CL::ResourcePath::MESH_PLAYER_RIFLE);
			m_pPlayerMesh->AddComponent<MeshFilter>(_Mesh);

			MaterialComponent* _Material = new MaterialComponent();
			_Material->SetVertexShaderName("vs_stnd.cso");
			_Material->SetPixelShaderName("ps_stnd.cso");
			m_pPlayerMesh->AddComponent<MaterialComponent>(_Material);

			//MeshRenderer
			m_pPlayerMesh->AddComponent<MeshRenderer>(new MeshRenderer);
			m_pPlayerMesh->GetComponent<MeshRenderer>()->SetGizmo(false);
			//m_pPlayerMesh->AddComponent<SkinningAnimation>(new SkinningAnimation);

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

			const std::string Crouch_Idle = "Crouch_Idle";
			const std::string Crouch_Walk_Forward = "Crouch_Walk_Forward";
			const std::string Crouch_Walk_Backward = "Crouch_Walk_Backward";
			const std::string Crouch_Walk_Left = "Crouch_Walk_Left";
			const std::string Crouch_Walk_Right = "Crouch_Walk_Right";

			// Layer 추가
			_playerAnimator->AddAnimLayer(Movement);
			{
				/// State 추가
				{


					_playerAnimator->GetAnimLayer(Movement)->m_MaskingType = eMASKING_TYPE::NONE;

					// Stand
					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Idle, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_IDLE);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Walk_Forward, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_FORWARD);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Walk_Backward, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_BACKWARD);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Walk_Left, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_LEFT);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Walk_Right, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_RIGHT);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Sprint, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_SPRINT);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Sprint)->m_StateOffsetAngle = 90.0f;			// 축 맞추기용

					// Crouch
					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Idle, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_IDLE);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Idle)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Walk_Forward, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Forward)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Walk_Backward, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Backward)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Walk_Left, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_LEFT);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Left)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용

					_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Crouch_Walk_Right, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
					_playerAnimator->GetAnimLayer(Movement)->GetState(Crouch_Walk_Right)->m_StateOffsetAngle = 90.0f;		// 축 맞추기용


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

							// Stand_Idle -> "Crouch_Idle"
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->AddTrnasition(Crouch_Idle);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Idle)->m_Transition_V->at(5)->m_FadingPeriod = 5;

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

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->AddTrnasition(Crouch_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Forward)->m_Transition_V->at(5)->m_FadingPeriod = 30;
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

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->AddTrnasition(Crouch_Walk_Backward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(4)->AddParameter(&_playerController->m_bCrouch, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Backward)->m_Transition_V->at(4)->m_FadingPeriod = 30;
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

							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->AddTrnasition(Crouch_Walk_Left);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Left)->m_Transition_V->at(5)->m_FadingPeriod = 30;
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


							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->AddTrnasition(Crouch_Walk_Right);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, true);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Walk_Right)->m_Transition_V->at(5)->m_FadingPeriod = 30;
						}

						// Stand_Sprint
						{
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Sprint)->AddTrnasition(Stand_Walk_Forward);
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Sprint)->m_Transition_V->at(0)->m_FadingPeriod = 5;
							_playerAnimator->GetAnimLayer(Movement)->GetState(Stand_Sprint)->m_Transition_V->at(0)->AddParameter(&_playerController->m_bSprint, false);

						}
					} // end of Stand


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

				} // end of Transition

			} // end of Layer[Movement]

			/// Aiming
			// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
			{
				_playerAnimator->AddAnimLayer("Aiming");
				_playerAnimator->GetAnimLayer("Aiming")->SetEnabled(false);
				_playerAnimator->GetAnimLayer("Aiming")->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_playerAnimator->GetAnimLayer("Aiming")->AddLocomotion("UpAndDown", CL::ResourcePath::ANIM_PLAYER_RIFLE_PITCH, &_playerController->m_PitchValue);
				//_playerAnimator->GetAnimLayer("Aiming")->AddStateMap("Aiming", CL::ResourcePath::ANIM_PLAYER_AIMING);
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
		////////////////////////////////////////////////////////////////////////////////////////////////////
	}
}

void CLMainScene::Update(float dTime)
{
}

void CLMainScene::Reset()
{
}
