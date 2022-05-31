#include "pch.h"
#include "CenterCreator.h"


CenterCreator::CenterCreator()
	: m_VectorMin(DirectX::SimpleMath::Vector3(0, 0, 0)), m_VectorMax(DirectX::SimpleMath::Vector3(0, 0, 0))
{

}

CenterCreator::~CenterCreator()
{
}

void CenterCreator::ApplyNowPos(const DirectX::XMFLOAT3& pos)
{
	DirectX::SimpleMath::Vector3 _nowPos = XMLoadFloat3(&pos);

	m_VectorMin = XMVectorMin(m_VectorMin, _nowPos);
	m_VectorMax = XMVectorMax(m_VectorMax, _nowPos);
}

void CenterCreator::GetFinalCenter(DirectX::XMFLOAT3& __out center)
{
	XMStoreFloat3(&center, 0.5f * (m_VectorMin + m_VectorMax));
}

void CenterCreator::GetFinalExtents(DirectX::XMFLOAT3& __out extent)
{
	XMStoreFloat3(&extent, 0.5f * (m_VectorMax - m_VectorMin));
}

void CenterCreator::Reset()
{
	m_VectorMin.x = 0.0f; 	m_VectorMin.y = 0.0f;	m_VectorMin.z = 0.0f;
	m_VectorMax.x = 0.0f;	m_VectorMax.y = 0.0f;	m_VectorMax.z = 0.0f;

}

