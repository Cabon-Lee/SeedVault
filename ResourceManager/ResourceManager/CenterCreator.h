#pragma once

class CenterCreator
{
public:

	CenterCreator();
	~CenterCreator();

public:
	DirectX::SimpleMath::Vector3 m_VectorMin;
	DirectX::SimpleMath::Vector3 m_VectorMax;

	void ApplyNowPos(const DirectX::XMFLOAT3& pos);
	void GetFinalCenter(DirectX::XMFLOAT3& __out center);
	void GetFinalExtents(DirectX::XMFLOAT3& __out extent);
	void Reset();

};


