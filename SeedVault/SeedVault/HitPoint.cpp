#include "pch.h"
#include "HitPoint.h"

HitPoint::HitPoint()
	: ComponentBase(ComponentType::GameLogic)
{

}

HitPoint::~HitPoint()
{

}

void HitPoint::Start()
{
	m_MyParticleSpawner = m_pMyObject->GetComponent<ParticleSpawner>();



	m_MyParticleSpawner->SetForce(1.6f);	// 중력
	m_MyParticleSpawner->SetDeltaAlpha(0.4f);
	m_MyParticleSpawner->SetBirthTime(0.0f);
	m_MyParticleSpawner->SetDeadTime(1.5f);
}

void HitPoint::Update(float dTime)
{

}

void HitPoint::OnRender()
{

}

GameObject* HitPoint::GetHitPoint()
{
	DLLEngine::CursorUpdate();
	auto _postion = DLLEngine::GetCursorWorldPosition();
	m_CursorPostion.x = _postion.x;
	m_CursorPostion.y = _postion.y;
	m_CursorPostion.z = _postion.z;
	m_DepthFromCamera = _postion.w;

	m_CursorNormal = DLLEngine::GetCursorWorldNormal();
	m_ObjectID = DLLEngine::GetCursorObjectID();

	GameObject* _hitObj = DLLEngine::FindGameObjectById(m_ObjectID);

	// m_ObjectID을 활용하기 위해 일단 태그가 있어야 할 것 같음
	// 태그에 따라서 텍스쳐를 설정하고 그리게하면 될듯

	m_MyParticleSpawner->SetActive(true);
	m_MyParticleSpawner->SetPosition(m_CursorPostion);
	m_MyParticleSpawner->SetDirection(m_CursorNormal);

	/// 엔진(희정)이 태그 수정작업 완료하면 이후에 태그비교해서 작동하는 코드 작성
	//unsigned int _tag = 0;
	//unsigned int _tag = _hitObj->GetTag().size();
	switch (0)
	{
	case 13:	// "CharacterMesh"
		m_Size.x = 0.3f; m_Size.y = 0.3f;
		m_MyParticleSpawner->SetSize(m_Size);
		m_MyParticleSpawner->SetVelocity(20.f);
		m_MyParticleSpawner->SetDeltaSize(-0.5f);
		m_MyParticleSpawner->SetRandomDirWeight(0.3f);
		m_MyParticleSpawner->SetRandomSizeWeight(0.0);
		m_MyParticleSpawner->SetRandowmRotWeight(0.3);
		m_MyParticleSpawner->SetSpriteIndex(1);
		m_MyParticleSpawner->SetParticleCount(2 + rand() % 5);
		m_MyParticleSpawner->SetStopTime(0.05f);
		m_MyParticleSpawner->SetDeadTime(0.5f);
		m_MyParticleSpawner->SetDeltaVelocity(0.2f);
		break;


	default:
		m_Size.x = 0.01f; m_Size.y = 0.01f;
		m_MyParticleSpawner->SetSize(m_Size);
		m_MyParticleSpawner->SetRandomDirWeight(0.5f);
		m_MyParticleSpawner->SetVelocity(15.f);
		m_MyParticleSpawner->SetDeltaSize(-0.5f);
		m_MyParticleSpawner->SetRandomSizeWeight(0.15);
		m_MyParticleSpawner->SetRandowmRotWeight(0.6);
		m_MyParticleSpawner->SetSpriteIndex(0);
		m_MyParticleSpawner->SetParticleCount(8 + rand() % 24);
		m_MyParticleSpawner->SetStopTime(0.05f);
		m_MyParticleSpawner->SetDeadTime(1.8f);
		m_MyParticleSpawner->SetDeltaVelocity(0.2f);
		break;
	}

	return _hitObj;
}
