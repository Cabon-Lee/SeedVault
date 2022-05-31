#include "BoundingVolume.h"

#include "CATrace.h"

void BoundingVolume::Update(const DirectX::XMMATRIX& world)
{

	auto _translation = world.r[3];

	m_BoundingSphere.Center.x = _translation.m128_f32[0] + m_OriginCenter.x;
	m_BoundingSphere.Center.y = _translation.m128_f32[1] + m_OriginCenter.y;
	m_BoundingSphere.Center.z = _translation.m128_f32[2] + m_OriginCenter.z;

	/*
	// WorldTM�� Decompose�ؼ� scale, rot, pos���� ��´�
	DirectX::SimpleMath::Matrix _tm = world;

	DirectX::SimpleMath::Vector3 _pos, _scale;
	DirectX::SimpleMath::Quaternion _rot;

	_tm.Decompose(_scale, _rot, _pos);

	// XMVECTOR �����ε� ������ �ִ´�
	DirectX::XMVECTOR _translation, _VectorScale, _quaterion;
	_translation = _pos;
	_VectorScale = DirectX::XMVectorReplicate(_scale.x);
	_quaterion = _rot;

	// ���� �߽����� ��
	DirectX::SimpleMath::Vector3 _center = m_OriginCenter;

	// �߽ɰ��� SRT������ ���ش�
	_center = DirectX::XMVectorAdd(
		DirectX::XMVector3Rotate(
			DirectX::XMVectorMultiply(_center, _VectorScale), _quaterion), _translation);

	//���Ͱ��� ȸ������ �����ش�
	m_BoundingBox.Center = _center;
	if (isSkinning != true)
		m_BoundingBox.Orientation = _rot;

	// �ڳʰ��� �˾ư���
	m_BoundingBox.GetCorners(m_Corners);
	*/
}

FrustumVolume::FrustumVolume(const DirectX::XMMATRIX& projTM)
{
	m_BoundingFrustum = DirectX::BoundingFrustum(projTM);
	memset(m_Corners, 0, sizeof(DirectX::XMFLOAT3) * 8);
}

FrustumVolume::~FrustumVolume()
{

}

void FrustumVolume::Update(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& projTM, bool coner)
{
	//m_BoundingFrustum = DirectX::BoundingFrustum(projTM);
	DirectX::BoundingFrustum::CreateFromMatrix(m_BoundingFrustum, projTM);
	m_BoundingFrustum.Transform(m_BoundingFrustum, world);

	if (coner == true)
	{
		m_BoundingFrustum.GetCorners(m_Corners);
	}
}

const DirectX::SimpleMath::Vector3& FrustumVolume::GetCornerVector(unsigned int idx)
{
	if (idx > 7)
	{
		CA_TRACE("Bounding Volume Invalid Corner Index.");
		return m_Corners[7];
	}
	return m_Corners[idx];
}

DirectX::BoundingFrustum& FrustumVolume::GetFrustum()
{
	return m_BoundingFrustum;
}

