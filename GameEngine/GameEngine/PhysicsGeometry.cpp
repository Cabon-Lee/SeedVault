#include "pch.h"
#include "PhysicsGeometry.h"

void BoxGeometry::SetBoxSize(const DirectX::SimpleMath::Vector3 scale)
{
	m_X = scale.x;
	m_Y = scale.y;
	m_Z = scale.z;
}
