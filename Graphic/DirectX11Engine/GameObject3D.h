#pragma once

//#include "GameObject.h"

class GameObject3D : public GameObject
{
public:
	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);
	const DirectX::XMVECTOR& GetForwardVector(bool omitY = false);
	const DirectX::XMVECTOR& GetRightVector(bool omitY = false);
	const DirectX::XMVECTOR& GetBackwardVector(bool omitY = false);
	const DirectX::XMVECTOR& GetLeftVector(bool omitY = false);

	bool LerpTurnRight(float a);
	bool LerpTurnLeft();

protected:
	virtual void UpdateMatrix();

	void UpdateDirectionVectors();

	//float rotAccumulate = 0.f;	// 90도 회전 누적값

	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR vec_forward;
	DirectX::XMVECTOR vec_left;
	DirectX::XMVECTOR vec_right;
	DirectX::XMVECTOR vec_backward;

	DirectX::XMVECTOR vec_forward_noY;
	DirectX::XMVECTOR vec_left_noY;
	DirectX::XMVECTOR vec_right_noY;
	DirectX::XMVECTOR vec_backward_noY;
};
