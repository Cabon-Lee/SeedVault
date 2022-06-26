#include "pch.h"
#include "Billboard.h"
#include "PlaneAnimation.h"
#include "EngineDLL.h"
#include "SimpleMath.h"

#include "IRenderer.h"
#include "IResourceManager.h"

Billboard::Billboard()
	: ComponentBase(ComponentType::Rendering)
	, m_SaveData(new Billboard_Save())
	, m_pRenderer(nullptr)
	, m_pCamera(nullptr)
	, m_pPlaneAni(nullptr)
	, m_resourceType(eResourceType::eSingleImage)
	, m_RotationType(eRotationType::LookAt)
	, m_ProportionX(1.f)
	, m_ProportionY(1.f)
	, m_PivotX(0.5f)
	, m_PivotY(0.5f)
	, m_IsGizmoOn(true)
	, m_worldTM(DirectX::SimpleMath::Matrix())
{
}

Billboard::~Billboard()
{
	Safe_Delete(m_SaveData);

	m_pCamera = nullptr;
	m_pPlaneAni = nullptr;
}

/// <summary>
/// ������ �ʱ�ȭ
/// </summary>
void Billboard::Start()
{
	ResourceObserver* _test = static_cast<ResourceObserver*>(this);

	m_pRenderer = DLLEngine::GetEngine()->GetIRenderer();
	m_pCamera = Managers::GetInstance()->GetCameraManager()->GetNowCamera();
	if (m_resourceType == eResourceType::eSeveralAni)
	{
		m_pPlaneAni = m_pMyObject->GetComponent<PlaneAnimation>();
		m_pPlaneAni->SetAniIdx(m_SpriteIndex_V.size());
	}
	m_worldTM = DirectX::XMMatrixIdentity();
}

/// <summary>
/// ������ ������
/// </summary>
void Billboard::OnRender()
{	
	m_pCamera = Managers::GetInstance()->GetCameraManager()->GetNowCamera();

	switch (m_RotationType)
	{
	case eRotationType::YAxis:
		Y_FixRender();
		break;
	case eRotationType::LookAt:
		LookAtdRender();
		break;
	}
	
	switch (m_resourceType)
	{
	case eResourceType::eSingleImage:
		m_pRenderer->DrawBillboard(m_SpriteIndex_V[0], m_worldTM);
		break;
	case eResourceType::eSeveralAni:
		m_pRenderer->DrawBillboard(m_SpriteIndex_V[m_pPlaneAni->GetCurAniIndx()], m_worldTM);
		break;
	case eResourceType::eSingleAni:
		break;
	default:
		m_pRenderer->DrawBillboard(m_SpriteIndex_V[0], m_worldTM);
		break;
	}
}

void Billboard::OnUIRender()
{
	m_pRenderer->DrawBillboardUI();
}

/// <summary>
/// �̹����� Ÿ��(�����̹���, ���� �ִ�, ������ִ�)
/// </summary>
/// <param name="resourceType"></param>
void Billboard::SeteResourceType(eResourceType resourceType)
{
	m_resourceType = resourceType;
}

/// <summary>
/// ȸ�� Ÿ�� ����
/// </summary>
/// <param name="rotationType"></param>
void Billboard::SetRotationType(eRotationType rotationType)
{
	m_RotationType = rotationType;
}

/// <summary>
/// ���� ����
/// </summary>
/// <param name="x">X(����)</param>
/// <param name="y">Y(����)</param>
void Billboard::SetProportion(float x, float y)
{
	m_ProportionX = x;
	m_ProportionY = y;
}

/// <summary>
/// �ǹ� ����(0~1������ ����)
/// </summary>
/// <param name="pivotX">X(����)</param>
/// <param name="pivotY">Y(����)</param>
void Billboard::SetPivot(float pivotX, float pivotY)
{
	if (pivotX <= 1 || pivotX >= 0)
	{
		m_PivotX = pivotX;
	}
	else if (pivotX > 1.f)
	{
		m_PivotX = 1.f;
	}
	else if (pivotX < 0)
	{
		m_PivotX = 0.f;
	}

	if (pivotY <= 1 || pivotY >= 0)
	{
		m_PivotY = pivotY;
	}
	else if (pivotY > 1.f)
	{
		m_PivotY = 1.f;
	}
	else if (pivotY < 0)
	{
		m_PivotY = 0.f;
	}
}

/// <summary>
/// Y�� �������� ȸ��
/// </summary>
void Billboard::Y_FixRender()
{
	// ��ũ ź��Ʈ �Լ��� ����Ͽ� ���� ī�޶� ��ġ�� ���ϵ��� ������ �𵨿� �����ؾ��ϴ� ȸ���� ����մϴ�.
	double angley = atan2(m_pMyObject->m_Transform->m_Position.x - m_pCamera->GetPosition().x, m_pMyObject->m_Transform->m_Position.z - m_pCamera->GetPosition().z) * (180.0 / XM_PI);

	// ȸ���� �������� ��ȯ�մϴ�.
	float rotationy = (float)angley * (XM_PI / 180);

	// ���� ����� ����Ͽ� �������� ������ ȸ���� �����մϴ�.
	XMMATRIX RotationMatrix = XMMatrixRotationY(rotationy);
	
	// ������ �𵨿��� ���� ����� �����մϴ�.
	XMMATRIX translateMatrix = XMMatrixTranslation(m_pMyObject->m_Transform->m_Position.x, m_pMyObject->m_Transform->m_Position.y, m_pMyObject->m_Transform->m_Position.z);

	// ���������� ũ��, ȸ�� �� ��ȯ ����� �����Ͽ� ������ ���� ���� ����� ����ϴ�.
	m_worldTM = XMMatrixScaling(m_pMyObject->m_Transform->m_Scale.x, m_pMyObject->m_Transform->m_Scale.y, 1.0f) * RotationMatrix * translateMatrix;
}

/// <summary>
/// ī�޶� �ٶ󺸴� ȸ��
/// </summary>
void Billboard::LookAtdRender()
{
	DirectX::SimpleMath::Matrix _CameraView = m_pCamera->View();
	XMFLOAT3X3 _CameraView3X3 = XMFLOAT3X3();
	XMFLOAT3X3 _CameraView3X3Inverse = XMFLOAT3X3();
	DirectX::SimpleMath::Matrix _rotationMatrix = DirectX::SimpleMath::Matrix();

	// ī�޶� ������� Ʈ�������̼��� ������ 3x3��Ŀ� �ִ´�
	_CameraView3X3._11 = _CameraView._11;
	_CameraView3X3._12 = _CameraView._12;
	_CameraView3X3._13 = _CameraView._13;

	_CameraView3X3._21 = _CameraView._21;
	_CameraView3X3._22 = _CameraView._22;
	_CameraView3X3._23 = _CameraView._23;

	_CameraView3X3._31 = _CameraView._31;
	_CameraView3X3._32 = _CameraView._32;
	_CameraView3X3._33 = _CameraView._33;

	//�� 3x3����� ����Ľ�
	double det = _CameraView3X3._11 * (_CameraView3X3._22 * _CameraView3X3._33 - _CameraView3X3._32 * _CameraView3X3._23) -
				 _CameraView3X3._12 * (_CameraView3X3._21 * _CameraView3X3._33 - _CameraView3X3._23 * _CameraView3X3._31) +
				 _CameraView3X3._13 * (_CameraView3X3._13 * _CameraView3X3._32 - _CameraView3X3._22 * _CameraView3X3._31);

	double idet = 1 / det;

	_CameraView3X3Inverse._11 = (_CameraView3X3._22 * _CameraView3X3._33 - _CameraView3X3._32 * _CameraView3X3._23) * idet;
	_CameraView3X3Inverse._12 = (_CameraView3X3._13 * _CameraView3X3._32 - _CameraView3X3._12 * _CameraView3X3._33) * idet;
	_CameraView3X3Inverse._13 = (_CameraView3X3._12 * _CameraView3X3._23 - _CameraView3X3._13 * _CameraView3X3._22) * idet;

	_CameraView3X3Inverse._21 = (_CameraView3X3._23 * _CameraView3X3._31 - _CameraView3X3._21 * _CameraView3X3._33) * idet;
	_CameraView3X3Inverse._22 = (_CameraView3X3._11 * _CameraView3X3._33 - _CameraView3X3._13 * _CameraView3X3._31) * idet;
	_CameraView3X3Inverse._23 = (_CameraView3X3._21 * _CameraView3X3._13 - _CameraView3X3._11 * _CameraView3X3._23) * idet;

	_CameraView3X3Inverse._31 = (_CameraView3X3._21 * _CameraView3X3._32 - _CameraView3X3._31 * _CameraView3X3._22) * idet;
	_CameraView3X3Inverse._32 = (_CameraView3X3._31 * _CameraView3X3._12 - _CameraView3X3._11 * _CameraView3X3._32) * idet;
	_CameraView3X3Inverse._33 = (_CameraView3X3._11 * _CameraView3X3._22 - _CameraView3X3._21 * _CameraView3X3._12) * idet;

	// 3x3 ������� 4X4��Ŀ� ����, Ʈ��������Ʈ�� 0
	_rotationMatrix._11 = _CameraView3X3Inverse._11;
	_rotationMatrix._12 = _CameraView3X3Inverse._12;
	_rotationMatrix._13 = _CameraView3X3Inverse._13;
	_rotationMatrix._14 = 0;

	_rotationMatrix._21 = _CameraView3X3Inverse._21;
	_rotationMatrix._22 = _CameraView3X3Inverse._22;
	_rotationMatrix._23 = _CameraView3X3Inverse._23;
	_rotationMatrix._24 = 0;

	_rotationMatrix._31 = _CameraView3X3Inverse._31;
	_rotationMatrix._32 = _CameraView3X3Inverse._32;
	_rotationMatrix._33 = _CameraView3X3Inverse._33;
	_rotationMatrix._34 = 0;

	_rotationMatrix._41 = 0;
	_rotationMatrix._42 = 0;
	_rotationMatrix._43 = 0;
	_rotationMatrix._44 = 1;

	// ������ �𵨿��� ���� ����� �����մϴ�.
	XMMATRIX translateMatrix = XMMatrixTranslation(m_pMyObject->m_Transform->m_Position.x, m_pMyObject->m_Transform->m_Position.y, m_pMyObject->m_Transform->m_Position.z);
	DirectX::SimpleMath::Matrix _ScaleMatirx = XMMatrixScaling(m_pMyObject->m_Transform->m_Scale.x, m_pMyObject->m_Transform->m_Scale.y, 1.0f);

	m_worldTM = _ScaleMatirx * _rotationMatrix * translateMatrix;

}

void Billboard::ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_SpriteIndex_V.resize(m_SpriteName_V.size());

	for (int i = 0; i < m_SpriteName_V.size(); ++i)
	{
		m_SpriteIndex_V[i] = pResourceManager->GetSpriteDataIndex(m_SpriteName_V[i]);
	}

	// �̹����� ũ������
	DirectX::SimpleMath::Vector3 _scale = DirectX::SimpleMath::Vector3(0, 0, 0);
	pResourceManager->GetSpriteScale(m_SpriteIndex_V[0], _scale);

	_scale.x = _scale.x * m_ProportionX;
	_scale.y = _scale.y * m_ProportionY;

	m_pMyObject->m_Transform->SetScale(_scale);

}

void Billboard::SaveData()
{	
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	//m_SaveData->m_SpriteName = m_SpriteName;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Billboard::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);

	//m_SpriteName = m_SaveData->m_SpriteName;
}