#pragma once
#include "GameObject2D.h"

class Camera2D : public GameObject2D
{
public:
	Camera2D();
	void SetProjectionValues(float width, float height, float nearZ, float farZ);

	const DirectX::XMMATRIX& GetOrthoMatrix() const;
	const DirectX::XMMATRIX& GetWorldMatrix() const;
private:
	void UpdateMatrix() override;

	DirectX::XMMATRIX orthoMatrix;
	DirectX::XMMATRIX worldMatrix;
};