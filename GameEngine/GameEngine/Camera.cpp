#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Managers.h"
#include "EngineDLL.h"

#include "IRenderer.h"
#include "IResourceManager.h"

Camera::Camera(std::string name)
	: ComponentBase(ComponentType::Camera)
	, m_Name(name)
	, m_IsSkyBox(false)
	, m_Right(1.0f, 0.0f, 0.0f)
	, m_Up(0.0f, 1.0f, 0.0f)
	, m_Look(0.0f, 0.0f, 1.0f)
	, m_SaveData(new Camera_Save())
{
	//Managers::GetInstance()->GetCameraManager()->AddCamera(this, m_Name);

	m_FovY = 0.25f * MathHelper::Pi;
	m_Aspect = static_cast<float>(DLLWindow::GetScreenWidth()) / static_cast<float>(DLLWindow::GetScreenHeight());
	m_NearZ = 0.1f;
	m_FarZ = 10000.0f;
}

Camera::~Camera()
{
	///카메라 컴포넌트가 소멸될때 카메라 매니저에서 자신을 등록해제시키지 않으면 카메라메니저는 씬이 넘어가도 남아있는다.
   // Managers::GetInstance()->GetCameraManager()->

}

void Camera::Start()
{
	// Modified by 최 요 환
	// 기존에 상수값으로 특정 값을 넘겨주던 방식에서
	// 트랜스폼 컴포넌트에 있는 값을 가져와서 세팅하는 방식으로 변경
	Vector3 _startPosition = m_Transform->GetLocalPosition();
	
	SetLens(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	
	Engine* engine = DLLEngine::GetEngine();
	m_pRenderer = engine->GetIRenderer();
}

void Camera::Update(float dTime)
{
	if (GetIsEnabled() == true)
	{
		UpdateViewMatrix();
	}

	//m_pRenderer->CameraUpdate(m_Transform->GetWorldPosition(), m_View, m_Proj);
}

void Camera::OnRender()
{

}

Vector3 Camera::GetLook() const
{
	return m_Look;
}

DirectX::SimpleMath::Vector3 Camera::GetPosition() const
{
	return m_Transform->GetLocalPosition();
}

float Camera::GetNearWndWidth() const
{
	return m_Aspect * m_NearWndHeight;
}

float Camera::GetNearWndHeight() const
{
	return m_NearWndHeight;
}

float Camera::GetFarWndWidth() const
{
	return m_Aspect * m_FarWndHeight;
}

float Camera::GetFarWndHeight() const
{
	return m_FarWndHeight;
}

float Camera::GetNearZ() const
{
	return m_NearZ;
}

float Camera::GetFarZ() const
{
	return m_FarZ;
}

float Camera::GetAspect() const
{
	return m_Aspect;
}

_DLL void Camera::SetAspect(float aspect)
{
	m_Aspect = aspect;

	SetLens(m_FovY, m_Aspect, m_NearZ, m_FarZ);
}

float Camera::GetFovY() const
{
	return m_FovY;
}

void Camera::SetFovY(float fovY)
{
	m_FovY = fovY;

	SetLens(m_FovY, m_Aspect, m_NearZ, m_FarZ);
}

float Camera::GetFovX() const
{
	return 2.0f * atan(0.5f * GetNearWndWidth() / m_NearZ);
}

/// need 수정

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	//m_FovY = fovY;
	//m_Aspect = aspect;
	//m_NearZ = zn;
	//m_FarZ = zf;

	m_NearWndHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWndHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	// XMMatrixOrthographicLH for orthogonal
	/// perspective view
	Matrix _p = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	//XMStoreFloat4x4(&m_Proj, _p);

	m_Proj = _p;
}

/// need 수정
void Camera::LookAtTarget(Vector3 target)
{
	Vector3 _l = XMVector3Normalize(target - m_Transform->m_Position);
	Vector3 _r = XMVector3Normalize(XMVector3Cross({ 0.0f, 1.0f, 0.0f }, _l));
	Vector3 _u = XMVector3Cross(_l, _r);

	float _x = -XMVectorGetX(XMVector3Dot(m_Transform->m_Position, _r));
	float _y = -XMVectorGetX(XMVector3Dot(m_Transform->m_Position, _u));
	float _z = -XMVectorGetX(XMVector3Dot(m_Transform->m_Position, _l));

	Matrix _resultTM = Matrix(
		_r.x, _u.x, _l.x, 0,
		_r.y, _u.y, _l.y, 0,
		_r.z, _u.z, _l.z, 0,
		0, 0, 0, 1);

	m_Transform->m_RotationTM = _resultTM.Transpose();
	m_Transform->m_Rotation = Quaternion::CreateFromRotationMatrix(m_Transform->m_RotationTM);
}

void Camera::LookAtVector(Vector3 vector)
{
	// Matrix::CreateLookAt({ 0.0f,0.0f,0.0f }, vector, {0.0f, 1.0f, 0.0f});
	//Matrix _look = Matrix::CreateLookAt({ 0.0f,0.0f,0.0f }, XMVector3Normalize(vector), { 0.0f, 1.0f, 0.0f });


	//Matrix _rotTM = Matrix::CreateFromQuaternion(XMVector3Normalize(vector));

	m_Transform->m_RotationTM = Matrix::CreateLookAt(
		m_Transform->m_Position,
		XMVector3Normalize(vector),
		{ 0.0f, 1.0f, 0.0f });

	//m_View *= m_Transform->m_RotationTM;
	m_Transform->m_Rotation = Quaternion::CreateFromRotationMatrix(m_Transform->m_RotationTM);
}

Matrix Camera::View() const
{
	return m_View; // XMLoadFloat4x4(&m_View);
}

Matrix Camera::Proj() const
{
	return m_Proj; // XMLoadFloat4x4(&m_Proj);
}

Matrix Camera::ViewProj() const
{
	return XMMatrixMultiply(View(), Proj());
}

void Camera::UpdateViewMatrix()
{
	Vector3 _r = m_Transform->m_WorldTM.Right();
	Vector3 _u = m_Transform->m_WorldTM.Up();
	Vector3 _l = m_Transform->m_WorldTM.Forward();
	Vector3 _p = m_Transform->GetWorldPosition();// { m_Transform->m_WorldTM._41, m_Transform->m_WorldTM._42, m_Transform->m_WorldTM._43 };

	_l = XMVector3Normalize(_l);
	_u = XMVector3Normalize(XMVector3Cross(_l, _r));
	_r = XMVector3Cross(_u, _l);

	float _x = -XMVectorGetX(XMVector3Dot(_p, _r));
	float _y = -XMVectorGetX(XMVector3Dot(_p, _u));
	float _z = -XMVectorGetX(XMVector3Dot(_p, _l));

	m_Right = _r;
	m_Up = _u;
	m_Look = _l;

	m_View = Matrix(
		m_Right.x, m_Up.x, m_Look.x, 0.0f,
		m_Right.y, m_Up.y, m_Look.y, 0.0f,
		m_Right.z, m_Up.z, m_Look.z, 0.0f,
		_x, _y, _z, 1.0f);
}

const std::string& Camera::GetName()
{
	return m_Name;
}

unsigned int Camera::GetSkyBoxTextrueIndex()
{
	return m_SkyBoxTextureIndex;
}

bool Camera::GetIsSkyBox()
{
	return m_IsSkyBox;
}

void Camera::SetSkyBoxTextureName(std::string name)
{
	m_SkyBoxTextureName = name.c_str();
}

void Camera::SetDrawSkyBox(bool val)
{
	m_IsSkyBox = val;
}

_DLL const DirectX::SimpleMath::Matrix& Camera::GetWorldTM()
{
	return m_Transform->GetWorld();
}

void Camera::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	m_SkyBoxTextureIndex = pReosureManager->GetTextureIndex(m_SkyBoxTextureName);
}

void Camera::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_Name = m_Name;
	m_SaveData->m_Look= &m_Look;
	m_SaveData->m_NearZ = m_NearZ;
	m_SaveData->m_FarZ = m_FarZ;
	m_SaveData->m_Aspect = m_Aspect;
	m_SaveData->m_FovY = m_FovY;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Camera::LoadData()
{
	m_SaveData->m_Look= &m_Look;
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
	m_Name = m_SaveData->m_Name;
	m_NearZ = m_SaveData->m_NearZ;
	m_FarZ = m_SaveData->m_FarZ;
	m_Aspect = m_SaveData->m_Aspect;
	m_FovY = m_SaveData->m_FovY;
}

