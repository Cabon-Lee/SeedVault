#include "Camera2D.h"

Camera2D::Camera2D()
{
	pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	posVector = XMLoadFloat3(&pos);
	rot = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotVector = XMLoadFloat3(&rot);
	UpdateMatrix();
}

void Camera2D::SetProjectionValues(float width, float height, float nearZ, float farZ)
{
	orthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, nearZ, farZ);
}

const DirectX::XMMATRIX& Camera2D::GetOrthoMatrix() const
{
	return orthoMatrix;
}

const DirectX::XMMATRIX& Camera2D::GetWorldMatrix() const
{
	return worldMatrix;
}

void Camera2D::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	DirectX::XMMATRIX translationOffsetMatrix = DirectX::XMMatrixTranslation(-pos.x, -pos.y, 0.0f); //z component irrelevant for 2d camera
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	worldMatrix = camRotationMatrix * translationOffsetMatrix;
}