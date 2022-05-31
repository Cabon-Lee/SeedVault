#pragma once
#include "GameObject3D.h"

using namespace DirectX;

enum CameraKind
{
	Debug = 0,
	Ingame,
};

enum class CameraState
{
	Front = 0,
	Right,
	Left,
	Back,
};

class Camera3D : public GameObject3D
{
public:
	Camera3D();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;

private:
	void UpdateMatrix() override;	//���ǿ� ���Ʈ����

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

};


