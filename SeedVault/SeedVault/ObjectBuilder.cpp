#include "pch.h"
#include "CameraController.h"
#include "CameraBehavior.h"
#include "OrbitCamera.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "Partner_Move.h"
#include "Partner_AI.h"
#include "Zombie_Runner_AI.h"
#include "Zombie_Runner_Move.h"
#include "Health.h"
#include "HitPoint.h"
#include "MuzzleFlash.h"
#include "NavMeshAgent.h"

#include "EquipmentController.h"
#include "DialogueManager.h"

#include "Audio.h"
#include "AudioPercentageDefine.h"
#include "ObjectBuilder.h"

/// <summary>
/// ObjectType에 해당하는 오브젝트를 만들어서 리턴
/// </summary>
/// <param name="type">만들 오브젝트 타입</param>
/// <returns>만들어진 오브젝트</returns>
GameObject* ObjectBuilder::MakeObject(ObjectType type)
{
	GameObject* obj = nullptr;
	switch (type)
	{
	case ObjectBuilder::ObjectType::ePlayer:
		obj = MakePlayer(obj);
		break;

	case ObjectBuilder::ObjectType::eRightHand:
		obj = MakeRightHand(obj);
		break;

	case ObjectBuilder::ObjectType::ePartner:
		obj = MakePartner(obj);
		break;

	case ObjectBuilder::ObjectType::eZombie_Runner:
		obj = MakeZombie_Runner(obj);
		break;

	case ObjectBuilder::ObjectType::eWayPoint:
		obj = MakeWayPoint(obj);
		break;

	case ObjectBuilder::ObjectType::eInGameUI:
		obj = MakeIngameUI(obj);
		break;

	case ObjectBuilder::ObjectType::ePauseUI:
		obj = MakePauseUI(obj);
		break;

	default:
		break;
	}

	return obj;
}

GameObject* ObjectBuilder::MakePlayer(GameObject* obj)
{
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("Player");

	/// <summary>
	/// Audio
	/// </summary>
	obj->AddComponent<Audio>(new Audio());

	PhysicsActor* _PlayerPhysActor = new PhysicsActor({ 0.15f, 0.62f, 0.15f }, RigidType::Dynamic);
	//_PlayerPhysActor->SetGravity(false);
	//_PlayerPhysActor->SetKinematic(true);
	obj->AddComponent(_PlayerPhysActor);
	_PlayerPhysActor->SetFreezeRotation(true, false, true);
	_PlayerPhysActor->SetFreezePosition(false, true, false);

	// [PlayerController]
	PlayerController* _playerController = new PlayerController();
	obj->AddComponent<PlayerController>(_playerController);

	// [Health]
	Health* _playerHealth = new Health();
	obj->AddComponent<Health>(_playerHealth);

	// [Inventory]
	Inventory* _playerInventory = new Inventory();
	obj->AddComponent<Inventory>(_playerInventory);

	// [ParticleSpawner]
	ParticleSpawner* _particleSpawner = new ParticleSpawner();
	_particleSpawner->AddSpriteName("dia.png");
	_particleSpawner->AddSpriteName("08.png");
	obj->AddComponent<ParticleSpawner>(_particleSpawner);

	// [HitPoint]
	HitPoint* _hitPoint = new HitPoint();
	obj->AddComponent<HitPoint>(_hitPoint);

	// [NavMeshAgent]
	obj->AddComponent<NavMeshAgent>(new NavMeshAgent());
	obj->GetComponent<NavMeshAgent>()->SetDebugMode(true);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/// 플레이어 메쉬 오브젝트(플레이어이 자식으로 들어감)
	{
		GameObject* _meshObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_meshObj->SetObjectName("Player_Mesh");

		DLLEngine::SetTag("PlayerMesh", _meshObj);

		_meshObj->m_Transform->SetPosition({ 0.0f, -0.62f, 0.0f });
		_meshObj->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
		_meshObj->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });	// 모델이 너무 작아서 스케일 조정..

		MeshFilter* _Mesh = new MeshFilter();
		_Mesh->SetMesh(CL::ResourcePath::MESH_PLAYER_RIFLE);
		_meshObj->AddComponent<MeshFilter>(_Mesh);

		//MeshRenderer
		_meshObj->AddComponent<MeshRenderer>(new MeshRenderer);
		_meshObj->GetComponent<MeshRenderer>()->SetGizmo(false);

		// [Animator]
		// 애니메이터 생성
		Animator* _playerAnimator = new Animator();

		/// Layer 추가
		// Rifle_Movement
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
		auto _animLayer_Rifle = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
		_animLayer_Rifle->m_MaskingType = eMASKING_TYPE::NONE;
		{
			/// State 추가
			// Stand
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_IDLE);
			auto _animState_Rifle_Stand_Idle = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_FORWARD);
			auto _animState_Rifle_Stand_Walk_Forward = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);

			/// <summary>
			/// 걷는 애니메이션 Sound삽입
			/// </summary>
			_animState_Rifle_Stand_Walk_Forward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Rifle_Stand_Walk_Forward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_BACKWARD);
			auto _animState_Rifle_Stand_Walk_Backward = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
			_animState_Rifle_Stand_Walk_Backward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Rifle_Stand_Walk_Backward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_LEFT);
			auto _animState_Rifle_Stand_Walk_Left = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
			_animState_Rifle_Stand_Walk_Left->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Rifle_Stand_Walk_Left->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_RIGHT);
			auto _animState_Rifle_Stand_Walk_Right = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
			_animState_Rifle_Stand_Walk_Right->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Rifle_Stand_Walk_Right->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_SPRINT);
			auto _animState_Rifle_Stand_Walk_Sprint = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
			_animState_Rifle_Stand_Walk_Sprint->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 2);
				},
				PLAYER_FOOTSTEP_RUN_L
					);
			_animState_Rifle_Stand_Walk_Sprint->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 2);
				},
				PLAYER_FOOTSTEP_RUN_R
					);

			// Crouch
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_IDLE);
			auto _animState_Rifle_Crouch_Idle = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
			auto _animState_Rifle_Crouch_Walk_Forward = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
			_animState_Rifle_Crouch_Walk_Forward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Rifle_Crouch_Walk_Forward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
			auto _animState_Rifle_Crouch_Walk_Backward = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
			_animState_Rifle_Crouch_Walk_Backward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Rifle_Crouch_Walk_Backward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_LEFT);
			auto _animState_Rifle_Crouch_Walk_Left = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
			_animState_Rifle_Crouch_Walk_Left->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Rifle_Crouch_Walk_Left->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
			auto _animState_Rifle_Crouch_Walk_Right = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
			_animState_Rifle_Crouch_Walk_Right->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Rifle_Crouch_Walk_Right->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);
			// Rifle_Die
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_DIE);
			auto _animState_Rifle_Die = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);

			// Rifle_Dead
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DEAD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_DEAD);
			auto _animState_Rifle_Dead = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DEAD);

			// end of State

			/// Transition 설정
			{
				//Rifle_Dead
				{
					_animState_Rifle_Die->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DEAD);
					_animState_Rifle_Die->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDead, true);
				}

				// Stand
				{
					// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Stand_Die
						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(0)->m_FadingPeriod = 8;

						// Stand_Die -> Rifle_Dead
						_animState_Rifle_Die->AddEvent(
							[obj]() {
								obj->GetComponent<PlayerController>()->PostDie();
							}, 0.99f
						);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Walking
						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(1)->m_FadingPeriod = 00;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(2)->m_FadingPeriod = 00;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(3)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(4)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_H, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Sprint
						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(5)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> "Crouch_Idle"
						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(6)->m_FadingPeriod = 5;
					}

					// Stand_Walk_Forward
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_FORWARD -> Stand_Die
						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(4)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(5)->m_FadingPeriod = 8;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(5)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(5)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(6)->m_FadingPeriod = 8;
					}

					// Stand_Walk_Backward
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_BACKWARD -> Stand_Die
						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(2)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(5)->m_FadingPeriod = 8;
					}

					// Stand_Walk_Left
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_LEFT -> Stand_Die
						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(1)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(1)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, true);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_V, false);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(6)->m_FadingPeriod = 8;
					}

					// Stand_Walk_Right
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_RIGHT -> Stand_Die
						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(1)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(1)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_H, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_V, false);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(6)->m_FadingPeriod = 8;
					}

					// Stand_Sprint
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_SPRINT -> Stand_Die
						_animState_Rifle_Stand_Walk_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Sprint->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Sprint->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Sprint->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Sprint->m_Transition_V->at(1)->AddParameter(&_playerController->m_bSprint, false);

					}
				} // end of Stand


				// Crouch
				{
					// "Crouch_Idle"
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_IDLE -> Crouch_Die
						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(0)->m_FadingPeriod = 8;

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(1)->AddParameter(&_playerController->m_bCrouch, false);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(4)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(5)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(6)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(6)->AddParameter(&_playerController->m_bSprint, true);
					}

					// Crouch_Walk_Forward
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_FORWARD -> Crouch_Die
						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_H, true);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_H, false);

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(2)->m_FadingPeriod = 20;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(3)->m_FadingPeriod = 20;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(5)->m_FadingPeriod = 30;

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(6)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(6)->AddParameter(&_playerController->m_bSprint, true);

					}

					// Crouch_Walk_Backward
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_BACKWARD -> Crouch_Die
						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_H, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_H, false);

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(4)->AddParameter(-1.0f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Crouch_Walk_Left
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_LEFT -> Crouch_Die
						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_V, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_V, false);

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(4)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(4)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_H, true);

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Crouch_Walk_Right
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_RIGHT -> Crouch_Die
						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_V, true);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_V, false);

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(4)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(5)->m_FadingPeriod = 30;

					}
				} // end of Crouch

			} // end of Transition

		} // end of Layer[Rifle_Movement]

		// Rifle_Assassinate_Begin
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_BEGIN);
		auto _animLayer_Rifle_Assassinate_Begin = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_BEGIN);
		_animLayer_Rifle_Assassinate_Begin->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Rifle_Assassinate_Begin->SetEnabled(false);
		_animLayer_Rifle_Assassinate_Begin->AddStateMap("Assassinate_Begin", CL::ResourcePath::ANIM_PLAYER_RIFLE_ASSASSINATE_BEGIN);

		// Rifle_Assassinate_End
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_END);
		auto _animLayer_Rifle_Assassinate_End = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_END);
		_animLayer_Rifle_Assassinate_End->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Rifle_Assassinate_End->SetEnabled(false);
		_animLayer_Rifle_Assassinate_End->AddStateMap("Assassinate_End", CL::ResourcePath::ANIM_PLAYER_RIFLE_ASSASSINATE_END);

		// Event
		{
			// Assassinate_Begin -> End
			auto _animState_Rifle_Assassinate_Begin = _animLayer_Rifle_Assassinate_Begin->GetState("Assassinate_Begin");
			_animState_Rifle_Assassinate_Begin->AddEvent(
				[_meshObj]() {
					_meshObj->GetComponent<Animator>()->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_END);
					CA_TRACE("[Player] Assassinate Begin End");
				},
				0.99f
					);

			// Assassinate_End -> 해제
			auto _animState_Rifle_Assassinate_End = _animLayer_Rifle_Assassinate_End->GetState("Assassinate_End");
			_animState_Rifle_Assassinate_End->AddEvent(
				[obj]() {
					obj->GetComponent<PlayerController>()->PostAssassinate();
				},
				0.99f
					);
		}

		// // end of Layer[Rifle_Assassinate]


		/// Layer 추가
		// Pistol
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL);
		auto _animLayer_Pistol = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL);
		{
			/// State 추가
			_animLayer_Pistol->m_MaskingType = eMASKING_TYPE::NONE;

			// Stand
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_IDLE);
			auto _animState_Pistol_Stand_Idle = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_WALK_FORWARD);
			auto _animState_Pistol_Stand_Walk_Forward = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
			_animState_Pistol_Stand_Walk_Forward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Pistol_Stand_Walk_Forward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_WALK_BACKWARD);
			auto _animState_Pistol_Stand_Walk_Backward = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
			_animState_Pistol_Stand_Walk_Backward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Pistol_Stand_Walk_Backward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_WALK_LEFT);
			auto _animState_Pistol_Stand_Walk_Left = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
			_animState_Pistol_Stand_Walk_Left->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Pistol_Stand_Walk_Left->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_WALK_RIGHT);
			auto _animState_Pistol_Stand_Walk_Right = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
			_animState_Pistol_Stand_Walk_Right->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);

			_animState_Pistol_Stand_Walk_Right->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_SPRINT);
			auto _animState_Pistol_Stand_Walk_Sprint = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
			_animState_Pistol_Stand_Walk_Sprint->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_RUN_L
					);

			_animState_Pistol_Stand_Walk_Sprint->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_RUN_R
					);

			// Crouch
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_IDLE);
			auto _animState_Pistol_Crouch_Idle = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
			auto _animState_Pistol_Crouch_Walk_Forward = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
			_animState_Pistol_Crouch_Walk_Forward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Pistol_Crouch_Walk_Forward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
			auto _animState_Pistol_Crouch_Walk_Backward = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
			_animState_Pistol_Crouch_Walk_Backward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Pistol_Crouch_Walk_Backward->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_WALK_LEFT);
			auto _animState_Pistol_Crouch_Walk_Left = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
			_animState_Pistol_Crouch_Walk_Left->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Pistol_Crouch_Walk_Left->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
			auto _animState_Pistol_Crouch_Walk_Right = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
			_animState_Pistol_Crouch_Walk_Right->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Pistol_Crouch_Walk_Right->AddEvent(
				[obj]() {
					//0: walk/1: Cwalk/2: Run;
					obj->GetComponent<Audio>()->PlayEvent("event:/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			// Pistol_Die
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_DIE);
			auto _animState_Pistol_Stand_Die = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);

			// Pistol_Dead
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DEAD, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_DEAD);
			auto _animState_Pistol_Stand_Dead = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DEAD);

			// end of State

			/// Transition 설정
			{
				//Rifle_Dead
				{
					_animState_Pistol_Stand_Die->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DEAD);
					_animState_Pistol_Stand_Die->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDead, true);
				}

				// Stand
				{
					// ANIMLAYER_PLAYER_PISTOL_STAND_IDLE
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_IDLE -> Stand_Die
						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(0)->m_FadingPeriod = 8;

						// Stand_Die -> Pistol_Dead
						_animState_Pistol_Stand_Die->AddEvent(
							[obj]() {
								obj->GetComponent<PlayerController>()->PostDie();
							}, 0.99f
						);

						// ANIMLAYER_PLAYER_PISTOL_STAND_IDLE -> Walking
						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(2)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(3)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(4)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_H, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Sprint
						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(5)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> "Crouch_Idle"
						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(6)->m_FadingPeriod = 5;
					}

					// Stand_Walk_Forward
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_FORWARD -> Stand_Die
						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(2)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(4)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(5)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(5)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(5)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Backward
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_BACKWARD -> Stand_Die
						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(2)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Left
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_LEFT -> Stand_Die
						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(1)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, true);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_V, false);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Right
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_RIGHT -> Stand_Die
						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(1)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_H, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_V, false);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Sprint
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_SPRINT -> Stand_Die
						_animState_Pistol_Stand_Walk_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Sprint->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Sprint->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Sprint->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Sprint->m_Transition_V->at(1)->AddParameter(&_playerController->m_bSprint, false);

					}
				} // end of Stand


				// Crouch
				{
					// "Crouch_Idle"
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_IDLE -> Crouch_Die
						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(1)->AddParameter(&_playerController->m_bCrouch, false);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(4)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(5)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(6)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(6)->AddParameter(&_playerController->m_bSprint, true);

					}

					// Crouch_Walk_Forward
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_FORWARD -> Crouch_Die
						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_H, true);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_H, false);

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(2)->m_FadingPeriod = 20;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(3)->m_FadingPeriod = 20;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(5)->m_FadingPeriod = 30;

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(6)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(6)->AddParameter(&_playerController->m_bSprint, true);
					}

					// Crouch_Walk_Backward
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_BACKWARD -> Crouch_Die
						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_H, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_H, false);

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(4)->AddParameter(-1.0f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Crouch_Walk_Left
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_LEFT -> Crouch_Die
						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_V, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_V, false);

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(4)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_H, true);

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Crouch_Walk_Right
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_RIGHT -> Crouch_Die
						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_V, true);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_V, false);

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}
				} // end of Crouch

			} // end of Transition

		} // end of Layer[Pistol_Movement]

		// Pistol_Assassinate_Begin
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_BEGIN);
		auto _animLayer_Pistol_Assassinate_Begin = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_BEGIN);
		_animLayer_Pistol_Assassinate_Begin->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Pistol_Assassinate_Begin->SetEnabled(false);
		_animLayer_Pistol_Assassinate_Begin->AddStateMap("Assassinate_Begin", CL::ResourcePath::ANIM_PLAYER_PISTOL_ASSASSINATE_BEGIN);

		// Pistol_Assassinate_End
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_END);
		auto _animLayer_Pistol_Assassinate_End = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_END);
		_animLayer_Pistol_Assassinate_End->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Pistol_Assassinate_End->SetEnabled(false);
		_animLayer_Pistol_Assassinate_End->AddStateMap("Assassinate_End", CL::ResourcePath::ANIM_PLAYER_PISTOL_ASSASSINATE_END);

		// Event
		{
			// Assassinate_Begin -> End
			auto _animState_Pistol_Assassinate_Begin = _animLayer_Pistol_Assassinate_Begin->GetState("Assassinate_Begin");
			_animState_Pistol_Assassinate_Begin->AddEvent(
				[_meshObj]() {
					_meshObj->GetComponent<Animator>()->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_END);
					CA_TRACE("[Player] Assassinate Begin End");
				},
				0.99f
					);

			// Assassinate_End -> 해제
			auto _animState_Pistol_Assassinate_End = _animLayer_Pistol_Assassinate_End->GetState("Assassinate_End");
			_animState_Pistol_Assassinate_End->AddEvent(
				[obj]() {
					obj->GetComponent<PlayerController>()->PostAssassinate();
				},
				0.99f
					);
		} // end of Layer[Pistol_Assassinate]


		/// Aiming
		// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
		{
			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);
			auto _animLayer_Rifle_Aim = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);

			_animLayer_Rifle_Aim->SetEnabled(false);
			_animLayer_Rifle_Aim->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_animLayer_Rifle_Aim->AddLocomotion("Rifle_Pitch", CL::ResourcePath::ANIM_PLAYER_RIFLE_PITCH, &_playerController->m_PitchValue);

			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_AIM);
			auto _animLayer_Pistol_Aim = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_AIM);

			_animLayer_Pistol_Aim->SetEnabled(false);
			_animLayer_Pistol_Aim->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_animLayer_Pistol_Aim->AddLocomotion("Pistol_Pitch", CL::ResourcePath::ANIM_PLAYER_PISTOL_PITCH, &_playerController->m_PitchValue);
		}

		///  Reload
		// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
		{
			// LongGun
			{

				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD);
				auto _animLayer_Rifle_Stand_Reload = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD);

				_animLayer_Rifle_Stand_Reload->SetEnabled(false);
				_animLayer_Rifle_Stand_Reload->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Stand_Reload->AddStateMap("Rifle_Reload", CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_RELOAD);

				// Crouch
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD);
				auto _animLayer_Rifle_Crouch_Reload = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD);

				_animLayer_Rifle_Crouch_Reload->SetEnabled(false);
				_animLayer_Rifle_Crouch_Reload->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Crouch_Reload->AddStateMap("Rifle_Crouch_Reload", CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_RELOAD);
			}
		}

		/// Swap
		{
			// Rifle
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);
				auto _animLayer_Rifle_Swap = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				_animLayer_Rifle_Swap->SetEnabled(false);
				_animLayer_Rifle_Swap->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Swap->AddStateMap("SwapWeapon", CL::ResourcePath::ANIM_PLAYER_RIFLE_SWAP_WEAPON);
				auto _animState_Rifle_Swap = _animLayer_Rifle_Swap->GetState("SwapWeapon");
				_animState_Rifle_Swap->AddEvent(
					[obj] {
						obj->GetComponent<Audio>()->PlayEvent("event:/Player_WeaponChange");
					},
					PLAYER_WEAPON_SWAP
						);
			}

			// Pistol
			{

				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_SWAP_WEAPON);
				auto _animLayer_Pistol_Swap = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_SWAP_WEAPON);

				_animLayer_Pistol_Swap->SetEnabled(false);
				_animLayer_Pistol_Swap->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Pistol_Swap->AddStateMap("SwapWeapon", CL::ResourcePath::ANIM_PLAYER_PISTOL_SWAP_WEAPON);
				auto _animState_Pistol_Swap = _animLayer_Pistol_Swap->GetState("SwapWeapon");
				_animState_Pistol_Swap->AddEvent(
					[obj] {
						obj->GetComponent<Audio>()->PlayEvent("event:/Player_WeaponChange");
					},
					PLAYER_WEAPON_SWAP
						);
			}
		}

		/// Routing Item
		{
			// Rifle
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ROUTING_ITEM);
				auto _animLayer_Rifle_Routing_Item = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ROUTING_ITEM);

				_animLayer_Rifle_Routing_Item->SetEnabled(false);
				_animLayer_Rifle_Routing_Item->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Routing_Item->AddStateMap("Routing_Item", CL::ResourcePath::ANIM_PLAYER_RIFLE_ROUTING_ITEM);
			}
		}


		/// 처음 레이어 세팅
		_playerAnimator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);

		_meshObj->AddComponent<Animator>(_playerAnimator);

		/// 부모로 
		_meshObj->SetParent(obj);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/// 메인 카메라
	{
		/// 카메라 포지션(피벗) CameraParent
		GameObject* _CameraParent = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_CameraParent->SetObjectName("CameraParent");

		_CameraParent->AddComponent<OrbitCamera>(new OrbitCamera);
		_CameraParent->GetComponent<OrbitCamera>()->SetFocusTrasform(*obj);
		////////////////////////////////////////////////////////////////////////////////////////////////////


		/// 메인 카메라 오브젝트
		{
			GameObject* _MainCamera = DLLEngine::CreateObject(DLLEngine::GetNowScene());
			_MainCamera->SetObjectName("MainCamera");

			/// 부모로 
			_MainCamera->SetParent(_CameraParent);
			_MainCamera->m_Transform->SetPosition({ 0.6f, 0.23f, -1.3f });
			_MainCamera->m_Transform->SetPosition({ 0.6f, 0.23f, 0.0f });
			//m_pMainCamera->m_Transform->SetRotationFromVec({ 10.0f, 0.0f, 0.0f });

			Camera* _newCamera = new Camera(_MainCamera->GetObjectName());

			_MainCamera->AddComponent<Camera>(_newCamera);

			// CameraBehavior
			CameraBehavior* _CameraBehavior = new CameraBehavior();
			_MainCamera->AddComponent<CameraBehavior>(_CameraBehavior);

			//CameraController* _CameraController = new CameraController();
			//m_MainCamera->AddComponent<CameraController>(_CameraController);

			////////////////////////////////////////////////////////////////////				
		}

		/// 조준 위치용 오브젝트
		GameObject* _ZoomInPosition = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_ZoomInPosition->SetObjectName("ZoomInPosition");
		_ZoomInPosition->SetParent(_CameraParent);

		_ZoomInPosition->m_Transform->SetPosition({ 0.63f, 0.25f, 0.2f });
	}

	return obj;
}

/// <summary>
/// 플레이어 캐릭터 메쉬의 손에 위치하는 장비 아이템 오브젝트
/// </summary>
/// <param name="obj"></param>
/// <returns></returns>
GameObject* ObjectBuilder::MakeRightHand(GameObject* obj)
{
	/// 플레이어 매쉬 손 위치에 부착할 부모오브젝트
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("RightHand");

	// [EquipmentController]
	obj->AddComponent<EquipmentController>(new EquipmentController);

	/// 장비중인 Mesh Obj
	{
		// 초기장비 주무기(라이플)에 맞춰 세팅
		GameObject* _EquipedMesh = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_EquipedMesh->SetObjectName("EquipedMesh");
		_EquipedMesh->m_Transform->SetPosition({ 0.05f, 0.0f, 0.15f });
		_EquipedMesh->m_Transform->SetRotationFromVec({ 10.0f, 100.0f, 80.0f });
		_EquipedMesh->m_Transform->SetScale({ 0.3f, 0.3f, 0.3f });
		_EquipedMesh->SetParent(obj);

		MeshFilter* _meshFilter = new MeshFilter();
		_meshFilter->SetMesh("Rifle.bin");
		_EquipedMesh->AddComponent<MeshFilter>(_meshFilter);
		_EquipedMesh->AddComponent<MeshRenderer>(new MeshRenderer());

		/// 머즐플래쉬 Obj
		{
			GameObject* _MuzzleFlashObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
			_MuzzleFlashObj->SetObjectName("MuzzleFlash");
			_MuzzleFlashObj->SetParent(_EquipedMesh);
			_MuzzleFlashObj->m_Transform->SetPosition({ 0.0f, 1.0f, 0.0f });

			// 임시 위치 확인용 메쉬 필터
			_MuzzleFlashObj->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });
			MeshFilter* _meshFilter = new MeshFilter();
			_meshFilter->SetMesh("0_Sphere.bin");
			_MuzzleFlashObj->AddComponent<MeshFilter>(_meshFilter);
			//_MuzzleFlashObj->AddComponent<MeshRenderer>(new MeshRenderer());

			// [Light]
			_MuzzleFlashObj->AddComponent<Light>(new Light);

			// [ParticleSpawner]
			ParticleSpawner* _particleSpawner = new ParticleSpawner();
			_particleSpawner->AddSpriteName("RifleFront10.png");
			_MuzzleFlashObj->AddComponent<ParticleSpawner>(_particleSpawner);

			// [MuzzleFlash]
			MuzzleFlash* _muzzleFlash = new MuzzleFlash();
			_MuzzleFlashObj->AddComponent<MuzzleFlash>(_muzzleFlash);
		}
	}

	return obj;
}

GameObject* ObjectBuilder::MakePartner(GameObject* obj)
{
	/// 최상위 부모(루트) 오브젝트 생성
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("Partner");
	DLLEngine::SetTag("Partner", obj);

	/// <summary>
	/// Audio
	/// </summary>
	obj->AddComponent<Audio>(new Audio());

	PhysicsActor* _physicsActor = new PhysicsActor({ 0.15f, 0.60f, 0.15f }, RigidType::Dynamic);
	obj->AddComponent<PhysicsActor>(_physicsActor);
	_physicsActor->SetFreezeRotation(true, false, true);
	_physicsActor->SetFreezePosition(false, true, false);

	obj->AddComponent<Health>(new Health);

	Partner_Move* _Partner_Move = new Partner_Move();
	obj->AddComponent<Partner_Move>(_Partner_Move);
	obj->AddComponent<Partner_AI>(new Partner_AI());

	obj->AddComponent<NavMeshAgent>(new NavMeshAgent());
	obj->GetComponent<NavMeshAgent>()->SetDebugMode(true);
	////////////////////////////////////////////////////////////////////////////////////

	/// 자식 (메쉬)오브젝트 생성
	GameObject* _meshObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());

	_meshObj->SetObjectName("Partner_Mesh");
	DLLEngine::SetTag("PartnerMesh", _meshObj);

	_meshObj->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
	_meshObj->m_Transform->SetPosition({ 0.0f, -0.55f, 0.0f });

	_meshObj->AddComponent<MeshFilter>(new MeshFilter());
	_meshObj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_PARTNER);

	_meshObj->AddComponent<MeshRenderer>(new MeshRenderer);
	_meshObj->GetComponent<MeshRenderer>()->SetGizmo(false);

	// 부모 설정
	_meshObj->SetParent(obj);

	// [Animator]
	// 애니메이터 생성
	Animator* _animator = new Animator();

	/// AnimLayer
	// Partner_Movemt
	_animator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PARTNER_MOVENT);
	auto _animLayer_Partner_Movement = _animator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PARTNER_MOVENT);
	_animLayer_Partner_Movement->SetEnabled(true);
	_animLayer_Partner_Movement->m_MaskingType = eMASKING_TYPE::NONE;

	/// State 추가
	// Stand
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE, CL::ResourcePath::ANIM_PARTNER_STAND_IDLE);
	auto _animState_Partner_Stand_Idle = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE);

	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK, CL::ResourcePath::ANIM_PARTNER_STAND_WALK);
	auto _animState_Partner_Stand_Walk = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK);
	_animState_Partner_Stand_Walk->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 0);
		},
		PARTNER_FOOTSTEP_WALK_L
			);
	_animState_Partner_Stand_Walk->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 0);
		},
		PARTNER_FOOTSTEP_WALK_R
			);

	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_SPRINT, CL::ResourcePath::ANIM_PARTNER_STAND_SPRINT);
	auto _animState_Partner_Stand_Sprint = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_SPRINT);
	_animState_Partner_Stand_Sprint->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 2);
		},
		PARTNER_FOOTSTEP_RUN_L
			);
	_animState_Partner_Stand_Sprint->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 2);
		},
		PARTNER_FOOTSTEP_RUN_R
			);

	// Crouch
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_IDLE, CL::ResourcePath::ANIM_PARTNER_CROUCH_IDLE);
	auto _animState_Partner_Crouch_Idle = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_IDLE);

	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_WALK, CL::ResourcePath::ANIM_PARTNER_CROUCH_WALK);
	auto _animState_Partner_Crouch_Walk = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_WALK);
	_animState_Partner_Crouch_Walk->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 1);
		},
		PARTNER_FOOTSTEP_CROUCH_L
			);
	_animState_Partner_Crouch_Walk->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 1);
		},
		PARTNER_FOOTSTEP_CROUCH_R
			);

	// Hit
	// Begin
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN, CL::ResourcePath::ANIM_PARTNER_HIT_BEGIN);
	auto _animState_Partner_Hit_Begin = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
	_animState_Partner_Hit_Begin->AddEvent(
		[obj]() {
			obj->GetComponent<Partner_Move>()->m_bHitBegin = false;
			obj->GetComponent<Partner_Move>()->m_bHitMiddle = true;
		},
		0.99f
	);


	// Middle
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_MIDDLE, CL::ResourcePath::ANIM_PARTNER_HIT_MIDDLE);
	auto _animState_Partner_Hit_Middle = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_MIDDLE);

	// End
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_END, CL::ResourcePath::ANIM_PARTNER_HIT_END);
	auto _animState_Partner_Hit_End = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_END);
	_animState_Partner_Hit_End->AddEvent(
		[obj]() {
			obj->GetComponent<Partner_Move>()->m_bHitEnd = false;
			obj->GetComponent<Partner_Move>()->m_State = Partner_Move::State::eFollow;
			obj->GetComponent<PhysicsActor>()->SetFreezePosition(false, true, false);
		},
		0.99f
	);


	// Die
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_DIE, CL::ResourcePath::ANIM_PARTNER_DIE);
	auto _animState_Partner_Die = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_DIE);
	_animState_Partner_Die->AddEvent(
		[obj]() {
		obj->GetComponent<Partner_Move>()->m_bHitMiddle = false;
			obj->GetComponent<Partner_Move>()->m_bDie = false;
			obj->GetComponent<Partner_Move>()->m_bDead = true;
		},
		0.99f
	);


	// Dead
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_DEAD, CL::ResourcePath::ANIM_PARTNER_DEAD);
	auto _animState_Partner_Dead = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_DEAD);

	/// Transition 설정
	{
		// Stand
		{
			// ANIM_LAYER_PARTNER_STAND_IDLE
			{
				// ANIM_LAYER_PARTNER_STAND_IDLE -> HitBegin
				_animState_Partner_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Stand_Idle->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_STAND_IDLE -> Stand_Walking
				_animState_Partner_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK);
				_animState_Partner_Stand_Idle->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Idle->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bWalk, true);

				// ANIM_LAYER_PARTNER_STAND_IDLE -> Crouch_Idle
				_animState_Partner_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_IDLE);
				_animState_Partner_Stand_Idle->m_Transition_V->at(2)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Idle->m_Transition_V->at(2)->AddParameter(&_Partner_Move->m_bCrouch, true);
			}

			// ANIM_LAYER_PARTNER_STAND_WALK
			{
				// ANIM_LAYER_PARTNER_STAND_WALK -> HitBegin
				_animState_Partner_Stand_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Stand_Walk->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_STAND_WALK -> Stand_Idle
				_animState_Partner_Stand_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE);
				_animState_Partner_Stand_Walk->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Walk->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bWalk, false);

				// ANIM_LAYER_PARTNER_STAND_WALK -> Stand_Sprint
				_animState_Partner_Stand_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_SPRINT);
				_animState_Partner_Stand_Walk->m_Transition_V->at(2)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Walk->m_Transition_V->at(2)->AddParameter(&_Partner_Move->m_bSprint, true);

				// ANIM_LAYER_PARTNER_STAND_WALK -> Crouch_Walk
				_animState_Partner_Stand_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_WALK);
				_animState_Partner_Stand_Walk->m_Transition_V->at(3)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Walk->m_Transition_V->at(3)->AddParameter(&_Partner_Move->m_bCrouch, true);
			}

			// ANIM_LAYER_PARTNER_STAND_SPRINT
			{
				// ANIM_LAYER_PARTNER_STAND_SPRINT -> HitBegin
				_animState_Partner_Stand_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Stand_Sprint->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_STAND_SPRINT -> Stand_WALK
				_animState_Partner_Stand_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK);
				_animState_Partner_Stand_Sprint->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Sprint->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bSprint, false);
			}
		}

		// Crouch
		{
			// ANIM_LAYER_PARTNER_CROUCH_IDLE
			{
				// ANIM_LAYER_PARTNER_CROUCH_IDLE -> HitBegin
				_animState_Partner_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Crouch_Idle->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_CROUCH_IDLE -> Crouch_Walking
				_animState_Partner_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_WALK);
				_animState_Partner_Crouch_Idle->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Crouch_Idle->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bWalk, true);

				// ANIM_LAYER_PARTNER_CROUCH_IDLE -> Stand_Idle
				_animState_Partner_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE);
				_animState_Partner_Crouch_Idle->m_Transition_V->at(2)->m_FadingPeriod = 10;
				_animState_Partner_Crouch_Idle->m_Transition_V->at(2)->AddParameter(&_Partner_Move->m_bCrouch, false);
			}

			// ANIM_LAYER_PARTNER_CROUCH_WALK
			{
				// ANIM_LAYER_PARTNER_CROUCH_WALK -> HitBegin
				_animState_Partner_Crouch_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Crouch_Walk->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_CROUCH_WALK -> Crouch_Idle
				_animState_Partner_Crouch_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_IDLE);
				_animState_Partner_Crouch_Walk->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Crouch_Walk->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bWalk, false);

				// ANIM_LAYER_PARTNER_CROUCH_WALK -> Stand_Walk
				_animState_Partner_Crouch_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK);
				_animState_Partner_Crouch_Walk->m_Transition_V->at(2)->m_FadingPeriod = 10;
				_animState_Partner_Crouch_Walk->m_Transition_V->at(2)->AddParameter(&_Partner_Move->m_bCrouch, false);
			}
		}

		// Hit Begin -> Middle
		{
			_animState_Partner_Hit_Begin->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_MIDDLE);
			_animState_Partner_Hit_Begin->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitMiddle, true);

		}

		// Hit Middle 
		{
			// Middle -> End
			_animState_Partner_Hit_Middle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_END);
			_animState_Partner_Hit_Middle->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitEnd, true);

			// Middle -> Die
			_animState_Partner_Hit_Middle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_DIE);
			_animState_Partner_Hit_Middle->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bDie, true);

		}

		// Hit End
		{
			// End -> Follow(Stand_Idle)
			_animState_Partner_Hit_End->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE);
			_animState_Partner_Hit_End->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitEnd, false);

			// End -> Begin
			_animState_Partner_Hit_End->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
			_animState_Partner_Hit_End->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bHitBegin, true);

		}

		// Die -> Dead
		{
			_animState_Partner_Die->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_DEAD);
			_animState_Partner_Die->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bDead, true);
		}
		
	}


	_meshObj->AddComponent<Animator>(_animator);

	////////////////////////////////////////////////////////////////////////////////////

	return obj;
}


/// <summary>
/// 러너 좀비 생성
/// 기본 컴포넌트들을 추가하고 세팅한다.
/// (각각 별도로 세팅해야하는 웨이포인트 같은 것들은 따로 해줘야 한다.)
/// </summary>
/// <param name="obj">생성할 오브젝트 포인터</param>
/// <returns>생성된 오브젝트 포인터</returns>
GameObject* ObjectBuilder::MakeZombie_Runner(GameObject* obj)
{
	static uint num = 0;
	std::string runnerNum = std::to_string(++num);

	/// 최상위 부모(루트) 오브젝트 생성
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("Zombie_Runner" + runnerNum);
	DLLEngine::SetTag("Zombie", obj);

	PhysicsActor* _physicsActor = new PhysicsActor({ 0.15f, 0.45f, 0.15f }, RigidType::Dynamic);
	obj->AddComponent<PhysicsActor>(_physicsActor);
	_physicsActor->SetFreezeRotation(true, false, true);
	_physicsActor->SetFreezePosition(false, true, false);
	//obj->SetKinematic(true);

	obj->AddComponent<Health>(new Health);

	obj->AddComponent<Zombie_Runner_Move>(new Zombie_Runner_Move());
	obj->GetComponent<Zombie_Runner_Move>()->m_MoveSpeed = 4.0f;
	obj->GetComponent<Zombie_Runner_Move>()->m_DetectionRange = 6.0f;
	obj->GetComponent<Zombie_Runner_Move>()->m_ViewSight.angle = 140.0f;
	obj->AddComponent<Zombie_Runner_AI>(new Zombie_Runner_AI());

	obj->AddComponent<NavMeshAgent>(new NavMeshAgent());
	obj->GetComponent<NavMeshAgent>()->SetDebugMode(true);
	////////////////////////////////////////////////////////////////////////////////////

	/// 자식 (메쉬)오브젝트 생성
	GameObject* _meshObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());

	std::string meshName = "Zombie_Runner" + runnerNum + "_Mesh";
	_meshObj->SetObjectName(meshName);

	DLLEngine::SetTag("ZombieMesh", _meshObj);

	_meshObj->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
	_meshObj->m_Transform->SetPosition({ 0.0f, -0.45f, 0.0f });

	_meshObj->AddComponent<MeshFilter>(new MeshFilter());
	_meshObj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_Zombie_RUNNER);

	_meshObj->AddComponent<MeshRenderer>(new MeshRenderer);
	_meshObj->GetComponent<MeshRenderer>()->SetGizmo(false);

	// 부모 설정
	_meshObj->SetParent(obj);

	// [Animator]
	// 애니메이터 생성
	Animator* _animator = new Animator();

	/// AnimLayer

	std::string Hunt = "Hunt";
	_animator->AddAnimLayer(Hunt);
	_animator->GetAnimLayer(Hunt)->SetEnabled(false);
	_animator->GetAnimLayer(Hunt)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(Hunt)->AddStateMap(Hunt, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_HUNT);

	std::string Die = "Die";
	_animator->AddAnimLayer(Die);
	_animator->GetAnimLayer(Die)->SetEnabled(false);
	_animator->GetAnimLayer(Die)->m_MaskingType = eMASKING_TYPE::NONE;

	_animator->GetAnimLayer(Die)->AddStateMap(Die, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_DIE);
	_animator->GetAnimLayer(Die)->AddStateMap("Dead", CL::ResourcePath::ANIM_ZOMBIE_RUNNER_DEAD);

	_animator->GetAnimLayer(Die)->GetState(Die)->AddTrnasition("Dead");
	_animator->GetAnimLayer(Die)->GetState(Die)->m_Transition_V->at(0)->AddParameter(&obj->GetComponent<Zombie_Runner_Move>()->m_bIsDead, true);

	_animator->GetAnimLayer(Die)->GetState(Die)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->m_bIsDie = false;
			obj->GetComponent<Zombie_Runner_Move>()->m_bIsDead = true;
		}, 0.99f
	);


	std::string patrol = "Patrol";
	_animator->AddAnimLayer(patrol);
	_animator->GetAnimLayer(patrol)->SetEnabled(true);
	_animator->GetAnimLayer(patrol)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(patrol)->AddStateMap(patrol, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_PATROL);

	std::string Wait = "Wait";
	_animator->AddAnimLayer(Wait);
	_animator->GetAnimLayer(Wait)->SetEnabled(false);
	_animator->GetAnimLayer(Wait)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(Wait)->AddStateMap(Wait, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_WAIT);

	std::string AttackPartner = "AttackPartner";
	_animator->AddAnimLayer(AttackPartner);
	_animator->GetAnimLayer(AttackPartner)->SetEnabled(false);
	_animator->GetAnimLayer(AttackPartner)->m_MaskingType = eMASKING_TYPE::NONE;
	{
		std::string AttackPartner_Begin = "AttackPartner_Begin";
		std::string AttackPartner_Middle = "AttackPartner_Middle";
		std::string AttackPartner_End = "AttackPartner_End";

		_animator->GetAnimLayer(AttackPartner)->AddStateMap(AttackPartner_Begin, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_BEGIN);
		auto _AttackPartner_Begin = _animator->GetAnimLayer(AttackPartner)->GetState(AttackPartner_Begin);

		_AttackPartner_Begin->AddEvent(
			[obj]() {
				obj->GetComponent<Zombie_Runner_Move>()->m_bIsAttackPartner_Middle = true;
			},
			0.99
		);

		_animator->GetAnimLayer(AttackPartner)->AddStateMap(AttackPartner_Middle, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_MIDDLE);
		auto _AttackPartner_Middle = _animator->GetAnimLayer(AttackPartner)->GetState(AttackPartner_Middle);
		
		_animator->GetAnimLayer(AttackPartner)->AddStateMap(AttackPartner_End, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_END);
		auto _AttackPartner_End = _animator->GetAnimLayer(AttackPartner)->GetState(AttackPartner_End);
		_AttackPartner_End->AddEvent(
			[obj]() {
				obj->GetComponent<Zombie_Runner_Move>()->m_State &= ~Zombie_Runner_Move::State::eAttackPartner;
				obj->GetComponent<Zombie_Runner_Move>()->m_bIsAttackPartner_End = false;

				obj->GetComponent<Zombie_Runner_Move>()->m_State |= Zombie_Runner_Move::State::eReturn;
			},
			0.99
		);


		/// Transition
		// Begin -> Middle
		{
			_AttackPartner_Begin->AddTrnasition(AttackPartner_Middle);
			_AttackPartner_Begin->m_Transition_V->at(0)->AddParameter(&obj->GetComponent< Zombie_Runner_Move>()->m_bIsAttackPartner_Middle, true);
		}

		// Middle -> End
		{
			_AttackPartner_Middle->AddTrnasition(AttackPartner_End);
			_AttackPartner_Middle->m_Transition_V->at(0)->AddParameter(&obj->GetComponent< Zombie_Runner_Move>()->m_bIsAttackPartner_End, true);
		}
	}

	std::string AttackPlayer = "AttackPlayer";
	_animator->AddAnimLayer(AttackPlayer);
	_animator->GetAnimLayer(AttackPlayer)->SetEnabled(false);
	_animator->GetAnimLayer(AttackPlayer)->m_MaskingType = eMASKING_TYPE::NONE;

	_animator->GetAnimLayer(AttackPlayer)->AddStateMap(AttackPlayer, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PLAYER);
	_animator->GetAnimLayer(AttackPlayer)->GetState(AttackPlayer)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->DamageToPlayer();
		}, 0.50f
	);
	_animator->GetAnimLayer(AttackPlayer)->GetState(AttackPlayer)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAttackPlayer();
		}, 0.99f
	);

	std::string AwakenSight = "AwakenSight";
	_animator->AddAnimLayer(AwakenSight);
	_animator->GetAnimLayer(AwakenSight)->SetEnabled(false);
	_animator->GetAnimLayer(AwakenSight)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(AwakenSight)->AddStateMap(AwakenSight, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_AWAKEN_SIGHT);
	_animator->GetAnimLayer(AwakenSight)->GetState(AwakenSight)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAwakenSight();

		}, 0.99f
	);

	std::string AwakenSound = "AwakenSound";
	_animator->AddAnimLayer(AwakenSound);
	_animator->GetAnimLayer(AwakenSound)->SetEnabled(false);
	_animator->GetAnimLayer(AwakenSound)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(AwakenSound)->AddStateMap(AwakenSound, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_AWAKEN_SOUNED);
	_animator->GetAnimLayer(AwakenSound)->GetState(AwakenSound)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAwakenSound();
		}, 
		0.99f
	);

	std::string Assassinated = "Assassinated";
	_animator->AddAnimLayer(Assassinated);
	_animator->GetAnimLayer(Assassinated)->SetEnabled(false);
	_animator->GetAnimLayer(Assassinated)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(Assassinated)->AddStateMap(Assassinated, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ASSASSINATED);
	_animator->GetAnimLayer(Assassinated)->GetState(Assassinated)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAssassinated();

		}, 0.99f
	);

	_meshObj->AddComponent<Animator>(_animator);
	////////////////////////////////////////////////////////////////////////////////////

	return obj;
}

GameObject* ObjectBuilder::MakeWayPoint(GameObject* obj)
{
	static uint num = 0;
	std::string waypointNum = std::to_string(++num);

	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("WayPoint_" + waypointNum);
	obj->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	obj->AddComponent<MeshFilter>(new MeshFilter());
	obj->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//obj->AddComponent<MeshRenderer>(new MeshRenderer);
	//obj->GetComponent<MeshRenderer>()->SetGizmo(true);

	return obj;
}

GameObject* ObjectBuilder::MakeIngameUI(GameObject* obj)
{
	/// UI 루트 컨테이너(Canvas 같은거)
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("IngameUI");
	obj->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
	//////////////////////////////////////////////////////////////////////

	GameObject* _DialogueManager = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_DialogueManager->SetObjectName("Dialogue Manager");

	_DialogueManager->AddComponent<DialogueManager>(new DialogueManager());

	GameObject* _DialogueText = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_DialogueText->SetObjectName("Dialogue Text");

	Text* _DialogueText1 = new Text();
	_DialogueText1->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		0.f, 100.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 45.f,
		eUIAxis::Center,
		eTextPoint::LeftUP);

	_DialogueText->AddComponent<Text>(_DialogueText1);
	_DialogueText->SetParent(_DialogueManager);

	//////////////////////////////////////////////////////////////////////

	GameObject* _CSVLoader = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_CSVLoader->SetObjectName("CSVLoader");

	_CSVLoader->AddComponent<CSVLoader>(new CSVLoader);
	_CSVLoader->SetParent(obj);
	//////////////////////////////////////////////////////////////////////

	GameObject* _aimPoint = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_aimPoint->SetObjectName("Aim Point");

	Sprite2D* _aimSptire = new Sprite2D;
	_aimSptire->SetType(eResourceType::eSingleImage);
	_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_Off.png");
	_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_On.png");
	_aimSptire->SetProportion(1.f, 1.f);
	_aimSptire->SetPivot(0.5f, 0.5f);
	_aimSptire->SetUIAxis(eUIAxis::Center);

	_aimPoint->AddComponent<Sprite2D>(_aimSptire);
	_aimPoint->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	/// 퀘스트 텍스트(텍스트 X2, sprite) 
	///퀘스트 제목
	//(텍스트 , sprite)
	GameObject* _questText = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_questText->SetObjectName("Quest Text");

	Sprite2D* _QuestSptire = new Sprite2D;
	_QuestSptire->SetType(eResourceType::eSingleImage);
	_QuestSptire->m_SpriteName_V.push_back("Quest_Text_Line.png");
	_QuestSptire->SetProportion(1.0f, 1.0f);
	_QuestSptire->SetPivot(0.5f, 0.5f);
	_QuestSptire->SetUIAxis(eUIAxis::RightUp);

	// (( "발전기 수리" ))
	Text* _QuestText1 = new Text();
	_QuestText1->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		190.f, 40.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 45.f,
		eUIAxis::RightUp,
		eTextPoint::Center);

	_questText->AddComponent<Sprite2D>(_QuestSptire);
	_questText->AddComponent<Text>(_QuestText1);
	_questText->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	///	퀘스트 내용
	//(텍스트)
	GameObject* _questText_Sub = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_questText_Sub->SetObjectName("Quest Text Sub");

	Text* _QuestText2 = new Text();
	_QuestText2->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		190.f, 100.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 23.f,
		eUIAxis::RightUp,
		eTextPoint::Center);

	_questText_Sub->AddComponent<Text>(_QuestText2);
	_questText_Sub->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	float _battleStatePosX = 535.f;
	float _battleStatePosY = 150.f;

	///
	/// 전투 스탯
	///
	//(스프라이트 X6, 텍스트 X6, 스프라이트(life) X6)
	//GameObject* _combetStat = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	//_combetStat->SetObjectName("Combet Stat");

	/// 장착 무기
	GameObject* _combetStat_Weapon = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Weapon->SetObjectName("Combet Weapon Stat");

	Sprite2D* _WeaponSptire = new Sprite2D;
	_WeaponSptire->SetType(eResourceType::eSingleImage);
	_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Pistol.png");
	_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Riflel.png");
	_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Flask.png");
	_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Fire.png");
	_WeaponSptire->SetProportion(1.f, 1.f);
	_WeaponSptire->SetPivot(1.0f, 1.0f);
	_WeaponSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _WeaponText1 = new Text();
	_WeaponText1->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 23.f, _battleStatePosY - 48.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 45.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combetStat_Weapon->AddComponent<Sprite2D>(_WeaponSptire);
	_combetStat_Weapon->AddComponent<Text>(_WeaponText1);
	_combetStat_Weapon->SetParent(obj);

	/// 장착 무기 남은 수량 텍스트
	GameObject* _combetStat_Weapon_sub_Text = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Weapon_sub_Text->SetObjectName("Combet Weapon Stat Sub Text");

	Text* _WeaponText2 = new Text();
	_WeaponText2->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 45.f, _battleStatePosY - 61.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 28.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combetStat_Weapon_sub_Text->AddComponent<Text>(_WeaponText2);
	_combetStat_Weapon_sub_Text->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 플라스크
	GameObject* _combetStat_Flask = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Flask->SetObjectName("Combet Flask Stat");

	Sprite2D* _FlaskSptire = new Sprite2D;
	_FlaskSptire->SetType(eResourceType::eSingleImage);
	_FlaskSptire->m_SpriteName_V.push_back("UI_Item_Flask_On.png");
	_FlaskSptire->m_SpriteName_V.push_back("UI_Item_Flask_Off.png");
	_FlaskSptire->SetProportion(1.f, 1.f);
	_FlaskSptire->SetPivot(1.0f, 1.0f);
	_FlaskSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _FlaskText = new Text();
	_FlaskText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 242.f, _battleStatePosY - 65.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combetStat_Flask->AddComponent<Sprite2D>(_FlaskSptire);
	_combetStat_Flask->AddComponent<Text>(_FlaskText);
	_combetStat_Flask->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 화염병
	GameObject* _combetStat_Fire = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Fire->SetObjectName("Combet Fire Stat");

	Sprite2D* _FireSptire = new Sprite2D;
	_FireSptire->SetType(eResourceType::eSingleImage);
	_FireSptire->m_SpriteName_V.push_back("UI_Item_Bottle_On.png");
	_FireSptire->m_SpriteName_V.push_back("UI_Item_Bottle_Off.png");
	_FireSptire->SetProportion(1.f, 1.f);
	_FireSptire->SetPivot(1.0f, 1.0f);
	_FireSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _FireText = new Text();
	_FireText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 322.f, _battleStatePosY - 65.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combetStat_Fire->AddComponent<Sprite2D>(_FireSptire);
	_combetStat_Fire->AddComponent<Text>(_FireText);
	_combetStat_Fire->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 힐
	GameObject* _combetStat_Heal = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Heal->SetObjectName("Combet Heal");

	Sprite2D* _HealSptire = new Sprite2D;
	_HealSptire->SetType(eResourceType::eSingleImage);
	_HealSptire->m_SpriteName_V.push_back("UI_Item_Heal_On.png");
	_HealSptire->m_SpriteName_V.push_back("UI_Item_Heal_Off.png");
	_HealSptire->SetProportion(1.f, 1.f);
	_HealSptire->SetPivot(1.0f, 1.0f);
	_HealSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _HealText = new Text();
	_HealText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 402.f, _battleStatePosY - 65.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combetStat_Heal->AddComponent<Sprite2D>(_HealSptire);
	_combetStat_Heal->AddComponent<Text>(_HealText);
	_combetStat_Heal->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 알코올
	GameObject* _combetStat_Alcohol = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Alcohol->SetObjectName("Combet Alcohol");

	Sprite2D* _AlcoholSptire = new Sprite2D;
	_AlcoholSptire->SetType(eResourceType::eSingleImage);
	_AlcoholSptire->m_SpriteName_V.push_back("UI_Item_Alcohol_On.png");
	_AlcoholSptire->m_SpriteName_V.push_back("UI_Item_Alcohol_Off.png");
	_AlcoholSptire->SetProportion(1.0f, 1.0f);
	_AlcoholSptire->SetPivot(1.0f, 1.0f);
	_AlcoholSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _AlcoholText = new Text();
	_AlcoholText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 470.f, _battleStatePosY - 25.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combetStat_Alcohol->AddComponent<Sprite2D>(_AlcoholSptire);
	_combetStat_Alcohol->AddComponent<Text>(_AlcoholText);
	_combetStat_Alcohol->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 허브
	GameObject* _combetStat_Herb = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Herb->SetObjectName("Combet Herb");

	Sprite2D* _HerbSptire = new Sprite2D;
	_HerbSptire->SetType(eResourceType::eSingleImage);
	_HerbSptire->m_SpriteName_V.push_back("UI_Item_Herb_On.png");
	_HerbSptire->m_SpriteName_V.push_back("UI_Item_Herb_Off.png");
	_HerbSptire->SetProportion(1.0f, 1.0f);
	_HerbSptire->SetPivot(1.0f, 1.0f);
	_HerbSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _HerbText = new Text();
	_HerbText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 470.f, _battleStatePosY - 60.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combetStat_Herb->AddComponent<Sprite2D>(_HerbSptire);
	_combetStat_Herb->AddComponent<Text>(_HerbText);
	_combetStat_Herb->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	///
	/// 플레이어 Life
	///
	GameObject* _Life = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Life->SetObjectName("Player Life Stat");

	Sprite2D* _LifeSptire = new Sprite2D;
	_LifeSptire->SetType(eResourceType::eSingleImage);
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_0.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_1.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_2.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_3.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_4.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_5.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_6.png");
	_LifeSptire->SetProportion(1.0f, 1.0f);
	_LifeSptire->SetPivot(1.0f, 1.0f);
	_LifeSptire->SetUIAxis(eUIAxis::RightDown);

	_Life->AddComponent<Sprite2D>(_LifeSptire);
	_Life->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	GameObject* _AssassinIcon = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_AssassinIcon->SetObjectName("AssassinIcon");
	_AssassinIcon->m_Transform->SetPosition({ 0.f, 0.f, 0.f });
	
	Billboard* _AssassinSprite = new Billboard;
	_AssassinSprite->SeteResourceType(eResourceType::eSingleImage);
	_AssassinSprite->SetRotationType(eRotationType::LookAt);
	_AssassinSprite->m_SpriteName_V.push_back("UI_Assassin.png");
	_AssassinSprite->SetProportion(0.001f, 0.001f);
	_AssassinSprite->SetPivot(1.0f, 1.0f);
	
	_AssassinIcon->AddComponent<Billboard>(_AssassinSprite);
	_AssassinIcon->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// (임시) 조수 Life
	/// </summary>
	//GameObject* _TmpAssistLifeBar = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	//_TmpAssistLifeBar->SetObjectName("Assist Life Bar");
	//_TmpAssistLifeBar->m_Transform->SetPosition({ 100.f, 100.f, 0.f });
	//
	//Sprite2D* _TmpAssistLifeBarSptire = new Sprite2D;
	//_TmpAssistLifeBarSptire->SetType(eResourceType::eSingleImage);
	//_TmpAssistLifeBarSptire->m_SpriteName_V.push_back("UI_HP_Partner_Off.png");
	//_TmpAssistLifeBarSptire->SetProportion(1.0f, 1.0f);
	//_TmpAssistLifeBarSptire->SetPivot(1.0f, 1.0f);
	//
	//_TmpAssistLifeBar->AddComponent<Sprite2D>(_TmpAssistLifeBarSptire);
	//_TmpAssistLifeBar->SetParent(obj);
	//
	//////////////////////////////////////////////////////////////////////////
	//
	//GameObject* _TmpAssistLife = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	//_TmpAssistLife->SetObjectName("Assist Life");
	//_TmpAssistLife->m_Transform->SetPosition({ 100.f, 100.f, 0.f });
	//
	//Sprite2D* _TmpAssistLifeSptire = new Sprite2D;
	//_TmpAssistLifeSptire->SetType(eResourceType::eSingleImage);
	//_TmpAssistLifeSptire->m_SpriteName_V.push_back("UI_HP_Partner_On.png");
	//_TmpAssistLifeSptire->SetProportion(0.5f, 1.f);
	//_TmpAssistLifeSptire->SetPivot(1.0f, 1.0f);
	//
	//_TmpAssistLife->AddComponent<Sprite2D>(_TmpAssistLifeSptire);
	//_TmpAssistLife->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	_questText->m_Transform->SetPosition({ 150.f, 80.f, 0.0f });

	_combetStat_Weapon->m_Transform->SetPosition({ _battleStatePosX, _battleStatePosY, 0.0f });
	_combetStat_Flask->m_Transform->SetPosition({ _battleStatePosX - 190.f, _battleStatePosY - 20.f, 0.0f });
	_combetStat_Fire->m_Transform->SetPosition({ _battleStatePosX - 270.f, _battleStatePosY - 20.f, 0.0f });
	_combetStat_Heal->m_Transform->SetPosition({ _battleStatePosX - 350.f, _battleStatePosY - 20.f, 0.0f });
	_combetStat_Alcohol->m_Transform->SetPosition({ _battleStatePosX - 430.f, _battleStatePosY - 20.f, 0.0f });
	_combetStat_Herb->m_Transform->SetPosition({ _battleStatePosX - 430.f, _battleStatePosY - 53.f, 0.0f });
	_Life->m_Transform->SetPosition({ _battleStatePosX, _battleStatePosY - 100.f, 0.0f });

	///////////////////////////////////////////////////////////////////////////

	obj->AddComponent<IngameUIManager>(new IngameUIManager());

	//////////////////////////////////////////////////////////////////////////

	return 	obj;
}


GameObject* ObjectBuilder::MakePauseUI(GameObject* obj)
{
	/// UI 루트 컨테이너(Canvas 같은거)
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("PauseUI");
	obj->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });

	///////////////////////////////////////////////////////////////////////////

	/// 반투명 배경
	GameObject* _backGround01 = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_backGround01->SetObjectName("BackGround01");

	Sprite2D* _backGround01Sptire = new Sprite2D;
	_backGround01Sptire->SetType(eResourceType::eSingleImage);
	_backGround01Sptire->m_SpriteName_V.push_back("PauseUIBackground01.png");
	_backGround01Sptire->SetProportion(1.f, 1.f);
	_backGround01Sptire->SetPivot(0.5f, 0.5f);
	_backGround01Sptire->SetUIAxis(eUIAxis::Center);

	_backGround01->AddComponent<Sprite2D>(_backGround01Sptire);
	_backGround01->SetParent(obj);

	/// 버튼 배경
	GameObject* _backGround02 = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_backGround02->SetObjectName("BackGround01");

	Sprite2D* _backGround02Sptire = new Sprite2D;
	_backGround02Sptire->SetType(eResourceType::eSingleImage);
	_backGround02Sptire->m_SpriteName_V.push_back("PauseUIBackground02.png");
	_backGround02Sptire->SetProportion(1.f, 1.f);
	_backGround02Sptire->SetPivot(0.5f, 0.5f);
	_backGround02Sptire->SetUIAxis(eUIAxis::Center);

	_backGround02->AddComponent<Sprite2D>(_backGround02Sptire);
	_backGround02->SetParent(obj);

	float _x = 330.f;
	float _y = 0.f;

	///Continue 버튼
	GameObject* _Continue = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Continue->m_Transform->SetPosition({ _x, _y, 1.0f });
	_Continue->SetObjectName("PauseContinueButton");

	Button* _ContinueButton = new Button();

	_ContinueButton->m_OffSprite = "Continue.png";
	_ContinueButton->m_OnSprite = "Continue_Select.png";

	_ContinueButton->SetProportion(1.0f, 1.0f);
	_ContinueButton->SetPivot(1.f, 1.f);
	_ContinueButton->SetAxis(eUIAxis::Center);
	_ContinueButton->SetFunc([]() { CA_TRACE("PauseUI 창이 꺼지는 버튼"); });

	_Continue->AddComponent<Button>(_ContinueButton);

	///Chapter 버튼
	GameObject* _Chapter = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Chapter->m_Transform->SetPosition({ _x, _y + 100, 1.0f });
	_Chapter->SetObjectName("PauseChapterButton");

	Button* _ChapterButton = new Button();

	_ChapterButton->m_OffSprite = "Chapter.png";
	_ChapterButton->m_OnSprite = "Chapter_Select.png";

	_ChapterButton->SetProportion(1.0f, 1.0f);
	_ChapterButton->SetPivot(1.f, 1.f);
	_ChapterButton->SetAxis(eUIAxis::Center);
	_ChapterButton->SetFunc([]() { CA_TRACE("PauseUI 챕터선택 버튼"); });

	_Chapter->AddComponent<Button>(_ChapterButton);

	///Title 버튼
	GameObject* _Title = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Title->m_Transform->SetPosition({ _x, _y + 200, 1.0f });
	_Title->SetObjectName("PauseTitleButton");

	Button* _TitleButton = new Button();

	_TitleButton->m_OffSprite = "Title.png";
	_TitleButton->m_OnSprite = "Title_Select.png";

	_TitleButton->SetProportion(1.0f, 1.0f);
	_TitleButton->SetPivot(1.f, 1.f);
	_TitleButton->SetAxis(eUIAxis::Center);
	_TitleButton->SetFunc([]() { DLLEngine::PickScene("CLStartScene"); });

	_Title->AddComponent<Button>(_TitleButton);

	///Exit 버튼
	GameObject* _Exit = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Exit->m_Transform->SetPosition({ _x, _y + 300, 1.0f });
	_Exit->SetObjectName("PauseExitButton");

	Button* _ExitButton = new Button();

	_ExitButton->m_OffSprite = "Exit.png";
	_ExitButton->m_OnSprite = "Exit_Select.png";

	_ExitButton->SetProportion(1.0f, 1.0f);
	_ExitButton->SetPivot(1.f, 1.f);
	_ExitButton->SetAxis(eUIAxis::Center);
	_ExitButton->SetFunc([]() { CA_TRACE("PauseUI 게임 종료 버튼"); });

	_Exit->AddComponent<Button>(_ExitButton);


	/// PopUp창
	GameObject* _popUp = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_popUp->SetObjectName("PopUp");

	Sprite2D* _popUpSptire = new Sprite2D;
	_popUpSptire->SetType(eResourceType::eSingleImage);
	_popUpSptire->m_SpriteName_V.push_back("PopUp.png");
	_popUpSptire->SetProportion(1.f, 1.f);
	_popUpSptire->SetPivot(0.5f, 0.5f);
	_popUpSptire->SetUIAxis(eUIAxis::Center);

	_popUp->AddComponent<Sprite2D>(_popUpSptire);
	_popUp->SetParent(obj);

	float _popUpx = -50.f;
	float _popUpy = 50.f;

	///Yes버튼
	GameObject* _Yes = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Yes->m_Transform->SetPosition({ _popUpx, _popUpy, 1.0f });
	_Yes->SetObjectName("ButtonOld");

	Button* _YesButton = new Button();

	_YesButton->m_OffSprite = "button_start_up.png";
	_YesButton->m_OnSprite = "button_start_down.png";

	_YesButton->SetProportion(1.0f, 1.0f);
	_YesButton->SetPivot(1.f, 1.f);
	_YesButton->SetAxis(eUIAxis::Center);
	_YesButton->SetFunc([]() { DLLEngine::PickScene("TestSceneYH"); });

	_Yes->AddComponent<Button>(_YesButton);

	///No버튼
	GameObject* _No = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_No->m_Transform->SetPosition({ _popUpx + 100, _popUpy, 1.0f });
	_No->SetObjectName("ButtonOld");

	Button* _NoButton = new Button();

	_NoButton->m_OffSprite = "button_start_up.png";
	_NoButton->m_OnSprite = "button_start_down.png";

	_NoButton->SetProportion(1.0f, 1.0f);
	_NoButton->SetPivot(1.f, 1.f);
	_NoButton->SetAxis(eUIAxis::Center);
	_NoButton->SetFunc([]() { DLLEngine::PickScene("TestSceneYH"); });

	_No->AddComponent<Button>(_NoButton);




	obj->AddComponent<PauseUIManager>(new PauseUIManager());

	return 	obj;

}