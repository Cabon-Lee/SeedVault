#include "pch.h"
#include "CameraController.h"
#include "CameraBehavior.h"
#include "OrbitCamera.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "Zombie_Runner_AI.h"
#include "Zombie_Runner_Move.h"
#include "Health.h"
#include "HitPoint.h"
#include "MuzzleFlash.h"
#include "NavMeshAgent.h"
#include "EquipmentController.h"

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

	default:
		break;
	}

	return obj;
}

GameObject* ObjectBuilder::MakePlayer(GameObject* obj)
{
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("Player");

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

		DLLEngine::SetTag("CharacterMesh", _meshObj);

		_meshObj->m_Transform->SetPosition({ 0.0f, -0.62f, 0.0f });
		_meshObj->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
		_meshObj->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });	// 모델이 너무 작아서 스케일 조정..

		MeshFilter* _Mesh = new MeshFilter();
		_Mesh->SetMesh(CL::ResourcePath::MESH_PLAYER_Rifle);
		_meshObj->AddComponent<MeshFilter>(_Mesh);

		//MeshRenderer
		_meshObj->AddComponent<MeshRenderer>(new MeshRenderer);
		_meshObj->GetComponent<MeshRenderer>()->SetGizmo(false);

		// [Animator]
		// 애니메이터 생성
		Animator* _playerAnimator = new Animator();

		// Layer 추가
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
		{
			/// State 추가
			{
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->m_MaskingType = eMASKING_TYPE::NONE;

				// Stand
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_IDLE);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_FORWARD);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_BACKWARD);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_LEFT);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_RIGHT);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_SPRINT);

				// Crouch
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_IDLE);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_LEFT);
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_RIGHT);

				// Die
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_DIE);

				// Rifle_Dead
				_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DEAD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_DEAD);

			} // end of State

			/// Transition 설정
			{
				//Rifle_Dead
				{
					_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DEAD);
					_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDead, true);
				}

				// Stand
				{
					// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Stand_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(0)->m_FadingPeriod = 8;

						// Stand_Die -> Rifle_Dead
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE)->AddEvent(PlayerController::FinishDie, 0.99f);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Walking
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_V, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(2)->m_FadingPeriod = 80;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_V, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(3)->m_FadingPeriod = 80;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(3)->AddParameter(-0.1f, &PlayerController::s_H, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(4)->m_FadingPeriod = 80;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(4)->AddParameter(0.1f, &PlayerController::s_H, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Sprint
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(5)->m_FadingPeriod = 80;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(5)->AddParameter(0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(5)->AddParameter(&PlayerController::s_bSprint, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> "Crouch_Idle"
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(6)->AddParameter(&PlayerController::s_bCrouch, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE)->m_Transition_V->at(6)->m_FadingPeriod = 5;



					}

					// Stand_Walk_Forward
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_FORWARD -> Stand_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_V, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_H, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_H, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(2)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_HForAnim, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_HForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(4)->AddParameter(&PlayerController::s_bSprint, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(5)->m_FadingPeriod = 80;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(5)->AddParameter(1.0f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(5)->AddParameter(-0.1f, &PlayerController::s_V, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(6)->AddParameter(&PlayerController::s_bCrouch, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD)->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Backward
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_BACKWARD -> Stand_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_V, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_H, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_H, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(2)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_HForAnim, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(3)->AddParameter(-0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_HForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(4)->AddParameter(-0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(4)->AddParameter(0.1f, &PlayerController::s_V, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(5)->AddParameter(&PlayerController::s_bCrouch, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD)->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Left
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_LEFT -> Stand_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(1)->AddParameter(-1.0f, &PlayerController::s_HForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_H, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_H, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_HForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_V, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_V, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(3)->AddParameter(-0.1f, &PlayerController::s_HForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(4)->AddParameter(-0.1f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(4)->AddParameter(-0.1f, &PlayerController::s_HForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(5)->AddParameter(0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(5)->AddParameter(&PlayerController::s_bSprint, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(6)->AddParameter(&PlayerController::s_bCrouch, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT)->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Right
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_RIGHT -> Stand_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(1)->AddParameter(1.0f, &PlayerController::s_HForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_H, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_HForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_H, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_V, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_V, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_HForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_VForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(4)->AddParameter(0.1f, &PlayerController::s_HForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(4)->AddParameter(-0.1f, &PlayerController::s_VForAnim, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(5)->AddParameter(0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(5)->AddParameter(&PlayerController::s_bSprint, true);


						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(6)->AddParameter(&PlayerController::s_bCrouch, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT)->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Sprint
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_SPRINT -> Stand_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT)->m_Transition_V->at(0)->m_FadingPeriod = 80;


						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT)->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT)->m_Transition_V->at(1)->AddParameter(&PlayerController::s_bSprint, false);

					}
				} // end of Stand


				// Crouch
				{
					// "Crouch_Idle"
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_IDLE -> Crouch_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(1)->AddParameter(&PlayerController::s_bCrouch, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_VForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(3)->AddParameter(-0.1f, &PlayerController::s_VForAnim, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(4)->m_FadingPeriod = 10;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(4)->AddParameter(-0.1f, &PlayerController::s_HForAnim, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(5)->m_FadingPeriod = 10;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(5)->AddParameter(0.1f, &PlayerController::s_HForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(6)->m_FadingPeriod = 10;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE)->m_Transition_V->at(6)->AddParameter(&PlayerController::s_bSprint, true);

					}

					// Crouch_Walk_Forward
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_FORWARD -> Crouch_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_V, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(1)->AddParameter(-0.001f, &PlayerController::s_H, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(1)->AddParameter(0.001f, &PlayerController::s_H, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(2)->m_FadingPeriod = 20;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_HForAnim, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(3)->m_FadingPeriod = 20;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_HForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(4)->AddParameter(1.0f, &PlayerController::s_VForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(4)->AddParameter(-0.1f, &PlayerController::s_V, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(5)->AddParameter(&PlayerController::s_bCrouch, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(5)->m_FadingPeriod = 30;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(6)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD)->m_Transition_V->at(6)->AddParameter(&PlayerController::s_bSprint, true);

					}

					// Crouch_Walk_Backward
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_BACKWARD -> Crouch_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_V, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(1)->AddParameter(-0.001f, &PlayerController::s_H, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(1)->AddParameter(0.001f, &PlayerController::s_H, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_HForAnim, false);


						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(3)->AddParameter(-0.1f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_HForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(4)->AddParameter(-1.0f, &PlayerController::s_VForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(4)->AddParameter(0.1f, &PlayerController::s_V, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(5)->AddParameter(&PlayerController::s_bCrouch, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD)->m_Transition_V->at(5)->m_FadingPeriod = 30;

					}

					// Crouch_Walk_Left
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_LEFT -> Crouch_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_H, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(1)->AddParameter(-0.1f, &PlayerController::s_HForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(1)->AddParameter(-0.001f, &PlayerController::s_V, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(1)->AddParameter(0.001f, &PlayerController::s_V, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(2)->AddParameter(-0.1f, &PlayerController::s_HForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_VForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(3)->AddParameter(-0.1f, &PlayerController::s_HForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(3)->AddParameter(-0.1f, &PlayerController::s_VForAnim, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(4)->AddParameter(-1.0f, &PlayerController::s_HForAnim, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(4)->AddParameter(0.1f, &PlayerController::s_H, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(5)->AddParameter(&PlayerController::s_bCrouch, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT)->m_Transition_V->at(5)->m_FadingPeriod = 30;

					}

					// Crouch_Walk_Right
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_RIGHT -> Crouch_Die
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(0)->AddParameter(&PlayerController::s_bIsDie, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_H, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(1)->AddParameter(0.1f, &PlayerController::s_HForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(1)->AddParameter(-0.001f, &PlayerController::s_V, true);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(1)->AddParameter(0.001f, &PlayerController::s_V, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_HForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(2)->AddParameter(0.1f, &PlayerController::s_VForAnim, true);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(3)->AddParameter(0.1f, &PlayerController::s_HForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(3)->AddParameter(-0.1f, &PlayerController::s_VForAnim, false);

						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(4)->AddParameter(1.0f, &PlayerController::s_HForAnim, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(4)->AddParameter(-0.1f, &PlayerController::s_H, false);


						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(5)->AddParameter(&PlayerController::s_bCrouch, false);
						_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE)->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT)->m_Transition_V->at(5)->m_FadingPeriod = 30;

					}
				} // end of Crouch

			} // end of Transition

		} // end of Layer[Rifle_Movement]

		/// Aiming
		// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
		{
			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM)->SetEnabled(false);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM)->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM)->AddLocomotion("Rifle_Pitch", CL::ResourcePath::ANIM_PLAYER_RIFLE_PITCH, &PlayerController::s_PitchValue);
			//_playerAnimator->GetAnimLayer("Rifle_Aiming")->AddStateMap("Aiming", CL::ResourcePath::ANIM_PLAYER_AIMING);
		}

		/// LongGun Reload
		// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
		{
			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD)->SetEnabled(false);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD)->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD)->AddStateMap("Rifle_Reload", CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_RELOAD);

			// Crouch
			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD)->SetEnabled(false);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD)->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD)->AddStateMap("Rifle_Crouch_Reload", CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_RELOAD);
		}

		/// Swap
		// 장비별로 동작이 똑같으니까 같은걸로 돌려쓰자
		{
			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON)->SetEnabled(false);
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON)->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON)->AddStateMap("SwapWeapon", CL::ResourcePath::ANIM_PLAYER_RIFLE_SWAP_WEAPON);
		}

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
		 _EquipedMesh->m_Transform->SetPosition({ 0.05f, 0.0f, 0.16f });
		 _EquipedMesh->m_Transform->SetRotationFromVec({ 10.0f, 100.0f, 95.0f });
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
			_MuzzleFlashObj->AddComponent<MeshRenderer>(new MeshRenderer());

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

	DLLEngine::SetTag("CharacterMesh", _meshObj);

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

	std::string Attack = "Attack";
	_animator->AddAnimLayer(Attack);
	_animator->GetAnimLayer(Attack)->SetEnabled(false);
	_animator->GetAnimLayer(Attack)->m_MaskingType = eMASKING_TYPE::NONE;
	
	_animator->GetAnimLayer(Attack)->AddStateMap(Attack, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PLAYER);
	_animator->GetAnimLayer(Attack)->GetState(Attack)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->DamageToPlayer();
		}, 0.50f
	);
	_animator->GetAnimLayer(Attack)->GetState(Attack)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAttack();
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

	obj->AddComponent<MeshRenderer>(new MeshRenderer);
	obj->GetComponent<MeshRenderer>()->SetGizmo(true);

	return obj;
}

GameObject* ObjectBuilder::MakeIngameUI(GameObject* obj)
{
	/// UI 루트 컨테이너(Canvas 같은거)
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("IngameUI");
	obj->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
	//////////////////////////////////////////////////////////////////////

	GameObject* _aimPoint = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_aimPoint->SetObjectName("Aim Point");

	Sprite2D* _aimSptire = new Sprite2D;
	_aimSptire->SetType(eResourceType::eSingleImage);
	_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_Off.png");
	_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_On.png");
	_aimSptire->SetProportion(1.f, 1.f);
	_aimSptire->SetPivot(0.5f, 0.5f);
	_aimSptire->SetUIAxis(UIAxis::Center);

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
	_QuestSptire->SetUIAxis(UIAxis::RightUp);

	// (( "발전기 수리" ))
	Text* _QuestText1 = new Text();
	_QuestText1->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		280.f, 20.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 45.f, UIAxis::RightUp);

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
		290.f, 90.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 23.f, UIAxis::RightUp);

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
	_WeaponSptire->SetUIAxis(UIAxis::RightDown);

	Text* _WeaponText1 = new Text();
	_WeaponText1->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 15.f, _battleStatePosY - 30.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 51.f, UIAxis::RightDown);

	_combetStat_Weapon->AddComponent<Sprite2D>(_WeaponSptire);
	_combetStat_Weapon->AddComponent<Text>(_WeaponText1);
	_combetStat_Weapon->SetParent(obj);
	
	/// 장착 무기 남은 수량 텍스트
	GameObject* _combetStat_Weapon_sub_Text = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Weapon_sub_Text->SetObjectName("Combet Weapon Stat Sub Text");

	Text* _WeaponText2 = new Text();
	_WeaponText2->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 45.f, _battleStatePosY - 50.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 28.f, UIAxis::RightDown);

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
	_FlaskSptire->SetUIAxis(UIAxis::RightDown);

	Text* _FlaskText = new Text();
	_FlaskText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 242.f, _battleStatePosY - 65.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f, UIAxis::RightDown);

	_combetStat_Flask->AddComponent<Sprite2D>(_FlaskSptire);
	_combetStat_Flask->AddComponent<Text>(_FlaskText);
	_combetStat_Flask->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 화염병
	GameObject* _combetStat_Fire = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combetStat_Fire->SetObjectName("Combet Fire Stat");

	Sprite2D* _FireSptire = new Sprite2D;
	_FireSptire->SetType(eResourceType::eSingleImage);
	_FireSptire->m_SpriteName_V.push_back("UI_Item_Fire_On.png");
	_FireSptire->m_SpriteName_V.push_back("UI_Item_Fire_Off.png");
	_FireSptire->SetProportion(1.f, 1.f);
	_FireSptire->SetPivot(1.0f, 1.0f);
	_FireSptire->SetUIAxis(UIAxis::RightDown);

	Text* _FireText = new Text();
	_FireText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 322.f, _battleStatePosY - 65.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f, UIAxis::RightDown);

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
	_HealSptire->SetUIAxis(UIAxis::RightDown);

	Text* _HealText = new Text();
	_HealText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 402.f, _battleStatePosY - 65.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f, UIAxis::RightDown);

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
	_AlcoholSptire->SetUIAxis(UIAxis::RightDown);

	Text* _AlcoholText = new Text();
	_AlcoholText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 470.f, _battleStatePosY - 25.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f, UIAxis::RightDown);

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
	_HerbSptire->SetUIAxis(UIAxis::RightDown);

	Text* _HerbText = new Text();
	_HerbText->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		_battleStatePosX - 470.f, _battleStatePosY - 60.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f, UIAxis::RightDown);

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
	_LifeSptire->SetUIAxis(UIAxis::RightDown);

	_Life->AddComponent<Sprite2D>(_LifeSptire);
	_Life->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// (임시) 조수 Life
	/// </summary>
	GameObject* _TmpAssistLifeBar = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_TmpAssistLifeBar->SetObjectName("Assist Life Bar");
	_TmpAssistLifeBar->m_Transform->SetPosition({ 100.f, 100.f, 0.f });

	Sprite2D* _TmpAssistLifeBarSptire = new Sprite2D;
	_TmpAssistLifeBarSptire->SetType(eResourceType::eSingleImage);
	_TmpAssistLifeBarSptire->m_SpriteName_V.push_back("UI_HP_Partner_Off.png");
	_TmpAssistLifeBarSptire->SetProportion(1.0f, 1.0f);
	_TmpAssistLifeBarSptire->SetPivot(1.0f, 1.0f);

	_TmpAssistLifeBar->AddComponent<Sprite2D>(_TmpAssistLifeBarSptire);
	_TmpAssistLifeBar->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	GameObject* _TmpAssistLife = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_TmpAssistLife->SetObjectName("Assist Life");
	_TmpAssistLife->m_Transform->SetPosition({ 100.f, 100.f, 0.f });

	Sprite2D* _TmpAssistLifeSptire = new Sprite2D;
	_TmpAssistLifeSptire->SetType(eResourceType::eSingleImage);
	_TmpAssistLifeSptire->m_SpriteName_V.push_back("UI_HP_Partner_On.png");
	_TmpAssistLifeSptire->SetProportion(0.5f, 1.f);
	_TmpAssistLifeSptire->SetPivot(1.0f, 1.0f);

	_TmpAssistLife->AddComponent<Sprite2D>(_TmpAssistLifeSptire);
	_TmpAssistLife->SetParent(obj);
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

	obj->AddComponent<UIManager>(new UIManager());

	return 	obj;
}
