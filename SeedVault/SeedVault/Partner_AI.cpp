#include "pch.h"
#include "Partner_Move.h"
#include "Partner_AI.h"

bool BehaviorTree::Partner::IsDead::Invoke()
{
	return m_Partner->IsDead();
}


bool BehaviorTree::Partner::IsFollow::CheckCondition()
{
	return m_Partner->IsFollow();;
}

bool BehaviorTree::Partner::MoveToPlayer::Invoke()
{
	return m_Partner->MoveToPlayer();
}

bool BehaviorTree::Partner::IsHit::CheckCondition()
{
	return m_Partner->IsHit();
}

bool BehaviorTree::Partner::Hit::Invoke()
{
	return m_Partner->Hit();
}

bool BehaviorTree::Partner::IsEscape::CheckCondition()
{
	return m_Partner->IsEscape();
}

bool BehaviorTree::Partner::Escape::Invoke()
{
	return m_Partner->Escape();
}


Partner_AI::Partner_AI()
	: ComponentBase(ComponentType::GameLogic)
	, m_Partner(nullptr)

	, m_Root(nullptr)
	, m_Selector(nullptr)

	, m_IsDead(nullptr)

	, m_IsHit(nullptr)
	, m_Hit(nullptr)

	, m_IsEscape(nullptr)
	, m_Escape(nullptr)

	, m_IsWait(nullptr)

	, m_IsFollow(nullptr)
	, m_MoveToPlayer(nullptr)

{
	CA_TRACE("[Partner_AI] Create Partner_AI !!");
}

Partner_AI::~Partner_AI()
{
}

void Partner_AI::Start()
{
	CA_TRACE("[Partner_AI] Start()");
	m_Partner = m_pMyObject->GetComponent<Partner_Move>();

	m_Root = std::make_unique<BehaviorTree::Sequence>();
	m_Selector = std::make_unique<BehaviorTree::Selector>();

	m_IsDead = std::make_unique<BehaviorTree::Partner::IsDead>(m_Partner);
	
	m_IsHit = std::make_unique<BehaviorTree::Partner::IsHit>(m_Partner);
	m_Hit = std::make_unique<BehaviorTree::Partner::Hit>(m_Partner);

	m_IsEscape = std::make_unique<BehaviorTree::Partner::IsEscape>(m_Partner);
	m_Escape = std::make_unique<BehaviorTree::Partner::Escape>(m_Partner);

	m_IsFollow = std::make_unique<BehaviorTree::Partner::IsFollow>(m_Partner);
	m_MoveToPlayer = std::make_unique<BehaviorTree::Partner::MoveToPlayer>(m_Partner);

	Initialize();
}

void Partner_AI::Update(float dTime)
{
	m_Root->Invoke();
}

void Partner_AI::OnRender()
{
}

void Partner_AI::Initialize()
{
	// Root Node
	m_Root->AddChild(m_Selector.get());

	// Dead
	m_Selector->AddChild(m_IsDead.get());

	// Escape
	m_Selector->AddChild(m_IsEscape.get());
	{
		m_IsEscape->SetChild(m_Escape.get());
	}

	// Hit
	m_Selector->AddChild(m_IsHit.get());
	{
		m_IsHit->SetChild(m_Hit.get());
	}

	// Follow
	m_Selector->AddChild(m_IsFollow.get());
	{
		m_IsFollow->SetChild(m_MoveToPlayer.get());
	}
}

