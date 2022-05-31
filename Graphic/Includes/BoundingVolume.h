#pragma once
#include <memory>
#include <DirectXCollision.h>
#include <DirectXMath.h>

#include <SimpleMath.h>

class BoundingVolume
{
public:
	BoundingVolume() {};
	BoundingVolume(bool skin) { isSkinning = skin; };
	~BoundingVolume() {};


	void Update(const DirectX::XMMATRIX& world);
	void GetCorners();
	bool intersect(DirectX::XMVECTOR Origin, DirectX::XMVECTOR Direction, float& Dist, DirectX::SimpleMath::Vector3 nodePos);

	// �׳� �����͸� ȣ���� ���� ������, �ٱ����� 7 �̻��� ���� �־��� �� ������ ���� �� ����
	DirectX::XMFLOAT3& GetOriginCenter()
	{
		return m_OriginCenter;
	}

	DirectX::XMFLOAT3& GetExtent()
	{
		return m_Extent;
	}


	DirectX::BoundingSphere& GetBoundingSphere() { return m_BoundingSphere; };
	void SetRadius(float offset = 1.0f) 
	{ 
		auto _result = DirectX::XMVector3Length(DirectX::XMLoadFloat3(&m_Extent)); 
		m_BoundingSphere.Radius = _result.m128_f32[0];
		m_BoundingSphere.Radius *= offset;
	};

private:
	DirectX::BoundingSphere m_BoundingSphere;
	DirectX::XMFLOAT3 m_OriginCenter;
	DirectX::XMFLOAT3 m_Extent;
	bool isSkinning;

	//DirectX::BoundingOrientedBox& GetBoundingVolume() { return m_BoundingBox; };
	//DirectX::BoundingOrientedBox m_BoundingBox;
	//DirectX::SimpleMath::Vector3 m_Corners[8];
};


class FrustumVolume
{
public:
	FrustumVolume() {};
	FrustumVolume(const DirectX::XMMATRIX& projTM);
	~FrustumVolume();

	void Update(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& projTM, bool coner);
	const DirectX::SimpleMath::Vector3& GetCornerVector(unsigned int idx);

	DirectX::BoundingFrustum& GetFrustum();

private:
	DirectX::BoundingFrustum m_BoundingFrustum;
	DirectX::XMFLOAT3 m_Corners[8];
};
