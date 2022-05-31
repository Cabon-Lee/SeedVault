#include "GameObject3D.h"

void GameObject3D::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == pos.x && lookAtPos.y == pos.y && lookAtPos.z == pos.z)
		return;

	lookAtPos.x = pos.x - lookAtPos.x;
	lookAtPos.y = pos.y - lookAtPos.y;
	lookAtPos.z = pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += DirectX::XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const DirectX::XMVECTOR& GameObject3D::GetForwardVector(bool omitY)
{
	if (omitY)
		return vec_forward_noY;
	else
		return vec_forward;
}

const DirectX::XMVECTOR& GameObject3D::GetRightVector(bool omitY)
{
	if (omitY)
		return vec_right_noY;
	else
		return vec_right;
}

const DirectX::XMVECTOR& GameObject3D::GetBackwardVector(bool omitY)
{
	if (omitY)
		return vec_backward_noY;
	else
		return vec_backward;
}

const DirectX::XMVECTOR& GameObject3D::GetLeftVector(bool omitY)
{
	if (omitY)
		return vec_left_noY;
	else
		return vec_left;
}

bool GameObject3D::LerpTurnRight(float ratio)
{
	//만약 주인공이 면의 중간지점에 들어와있다면(주인공이 중간에 있을때만 회전)
	//if ()
	{
		DirectX::XMFLOAT3 lerpRot = DirectX::XMFLOAT3(rot.x, rot.y - ratio, rot.z);
		DirectX::XMVECTOR lerpRotV = XMLoadFloat3(&lerpRot);

		DirectX::XMFLOAT3 turnRatio = DirectX::XMFLOAT3(0.0f, 0.0001f, 0.0f);
		DirectX::XMVECTOR turnRatioV = XMLoadFloat3(&turnRatio);

		DirectX::XMQuaternionSlerpV(rotVector, lerpRotV, turnRatioV);
		XMStoreFloat3(&this->rot, this->rotVector);

		//XMFLOAT3 turnRatio;
		//XMVectorLerpV()

		//this->gfx.land.AdjustRotation(0.0f, -DirectX::XM_PI / 2, 0.0f);
	}
	return false;
}

bool GameObject3D::LerpTurnLeft()
{
	return false;
}

void GameObject3D::UpdateMatrix()
{
	assert("UpdateMatrix must be overridden." && 0);
}

void GameObject3D::UpdateDirectionVectors()
{
	DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, 0.0f);
	vec_forward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	vec_backward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	vec_left = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	vec_right = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);

	DirectX::XMMATRIX vecRotationMatrixNoY = DirectX::XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f);
	vec_forward_noY = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrixNoY);
	vec_backward_noY = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrixNoY);
	vec_left_noY = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrixNoY);
	vec_right_noY = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrixNoY);
}