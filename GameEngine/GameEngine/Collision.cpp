#include "pch.h"
#include "GameObject.h"
#include "PhysicsActor.h"
#include "Collision.h"

Collision::Collision(PhysicsActor* actor)
	: m_Actor(actor)
	, m_GameObject(m_Actor->GetMyObject())
	, m_Transform(m_Actor->m_Transform)
{
	
}

Collision::~Collision()
{
}
