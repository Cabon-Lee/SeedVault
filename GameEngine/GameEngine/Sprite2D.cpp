#include "pch.h"
#include "Sprite2D.h"
#include "PlaneAnimation.h"
#include "EngineDLL.h"

#include "IRenderer.h"
#include "IResourceManager.h"

Sprite2D::Sprite2D()
	: ComponentBase(ComponentType::Rendering)
	, m_SaveData(new SpriteRender_Save())
	, m_pRenderer(nullptr)
	, m_pPlaneAni(nullptr)
	, m_resourceType(eResourceType::eSingleImage)
	, m_ProportionX(1.f)
	, m_ProportionY(1.f)
	, m_PivotX(0.5f)
	, m_PivotY(0.5f)
	, m_worldTM(DirectX::SimpleMath::Matrix())
	, m_NowSpriteIndex(0)
{
}

Sprite2D::~Sprite2D()
{
	Safe_Delete(m_SaveData);

	m_pCamera = nullptr;
	m_pPlaneAni = nullptr;
}

/// <summary>
/// ��������Ʈ �ʱ�ȭ
/// </summary>
void Sprite2D::Start()
{
	m_pRenderer = DLLEngine::GetEngine()->GetIRenderer();
	m_pEngine = DLLEngine::GetEngine();

	m_pCamera = Managers::GetInstance()->GetCameraManager()->GetNowCamera();

	// �ִϸ��̼��� ��� �̹����� �������� �ִϸ��̼� Ŭ������ �����ش�
	if (m_resourceType == eResourceType::eSeveralAni)
	{
		m_pPlaneAni = m_pMyObject->GetComponent<PlaneAnimation>();
		m_pPlaneAni->SetAniIdx(m_SpriteIndex_V.size());
	}

	m_width = static_cast<float>(DLLWindow::GetScreenWidth());
	m_height = static_cast<float>(DLLWindow::GetScreenHeight());

	float _Xpos = 0;
	float _Ypos = 0;

	switch (m_UIAxis)
	{
	case eUIAxis::None:
	case eUIAxis::LeftUp:
		_Xpos = m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_pMyObject->m_Transform->m_Position.y;
		break;
	case eUIAxis::RightUp:
		_Xpos = m_width - m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_pMyObject->m_Transform->m_Position.y;
		break;
	case eUIAxis::RightDown:
		_Xpos = m_width - m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_height - m_pMyObject->m_Transform->m_Position.y;
		break;
	case eUIAxis::LeftDown:
		_Xpos = m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_height - m_pMyObject->m_Transform->m_Position.y;
		break;
	case eUIAxis::Center:
		_Xpos = m_width / 2 + m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_height / 2 + m_pMyObject->m_Transform->m_Position.y;
		break;
	}

	float _ScaleX = m_pMyObject->m_Transform->m_Scale.x;
	float _ScaleY = m_pMyObject->m_Transform->m_Scale.y;

	// -0.5 ~ 0.5 ������ ���� �������� 0.5�� ���ش�
	m_PivotX -= 0.5f;
	m_PivotY -= 0.5f;

	m_worldTM =
		DirectX::XMMatrixScaling(m_pMyObject->m_Transform->m_Scale.x, m_pMyObject->m_Transform->m_Scale.y, 1.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(m_pMyObject->m_Transform->m_Rotation.x, m_pMyObject->m_Transform->m_Rotation.y + XM_PI, m_pMyObject->m_Transform->m_Rotation.z + XM_PI) *
		DirectX::XMMatrixTranslation(_Xpos + (_ScaleX * m_PivotX), _Ypos + (_ScaleY * m_PivotY), m_pMyObject->m_Transform->m_Position.z);

}

/// <summary>
/// ��������Ʈ ������
/// </summary>
void Sprite2D::OnRender()
{
}

void Sprite2D::OnUIRender()
{
	bool _isResize = m_pEngine->GetIsResize();

	if (_isResize == true)
	{
		m_width = static_cast<float>(DLLWindow::GetScreenWidth());
		m_height = static_cast<float>(DLLWindow::GetScreenHeight());
	}

	float _Xpos = 0;
	float _Ypos = 0;

	switch (m_UIAxis)
	{
	case eUIAxis::None:
	case eUIAxis::LeftUp:
		_Xpos = m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_pMyObject->m_Transform->m_Position.y;
		break;
	case eUIAxis::RightUp:
		_Xpos = m_width - m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_pMyObject->m_Transform->m_Position.y;
		break;
	case eUIAxis::RightDown:
		_Xpos = m_width - m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_height - m_pMyObject->m_Transform->m_Position.y;
		break;
	case eUIAxis::LeftDown:
		_Xpos = m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_height - m_pMyObject->m_Transform->m_Position.y;
		break;
	case eUIAxis::Center:
		_Xpos = m_width/2 + m_pMyObject->m_Transform->m_Position.x;
		_Ypos = m_height/2 + m_pMyObject->m_Transform->m_Position.y;
		break;
	}

	float _ScaleX = m_pMyObject->m_Transform->m_Scale.x;
	float _ScaleY = m_pMyObject->m_Transform->m_Scale.y;

	m_worldTM =
		DirectX::XMMatrixScaling(m_pMyObject->m_Transform->m_Scale.x, m_pMyObject->m_Transform->m_Scale.y, 1.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(m_pMyObject->m_Transform->m_Rotation.x, m_pMyObject->m_Transform->m_Rotation.y + XM_PI, m_pMyObject->m_Transform->m_Rotation.z + XM_PI) *
		DirectX::XMMatrixTranslation(_Xpos + (_ScaleX * m_PivotX), _Ypos + (_ScaleY * m_PivotY),	m_pMyObject->m_Transform->m_Position.z);

	switch (m_resourceType)
	{
	case eResourceType::eSingleImage:
		m_pRenderer->DrawSprite(m_resourceType, m_SpriteIndex_V[m_NowSpriteIndex], m_worldTM);
		break;
	case eResourceType::eSeveralAni:
		m_pRenderer->DrawSprite(m_resourceType, m_SpriteIndex_V[m_pPlaneAni->GetCurAniIndx()], m_worldTM);
		break;
	case eResourceType::eSingleAni:
		break;
	default:
		break;
	}
}

/// <summary>
/// �̹����� Ÿ��(�����̹���, ���� �ִ�, ������ִ�)
/// </summary>
/// <param name="resourceType"></param>
void Sprite2D::SetType(eResourceType resourceType)
{
	m_resourceType = resourceType;
}

/// <summary>
/// ���� ����
/// </summary>
/// <param name="x">X(����)</param>
/// <param name="y">Y(����)</param>
void Sprite2D::SetProportion(float x, float y)
{
	m_ProportionX = x;
	m_ProportionY = y;
}

/// <summary>
/// �ǹ� ����(0~1������ ����)
/// </summary>
/// <param name="pivotX">X(����)</param>
/// <param name="pivotY">Y(����)</param>
void Sprite2D::SetPivot(float pivotX, float pivotY)
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
/// ȭ���� 4���� �𼭸�, �߽� �� ��� �������� �׸��� 
/// </summary>
/// <param name="pivotX"></param>
/// <param name="pivotY"></param>
void Sprite2D::SetUIAxis(eUIAxis uiAxis)
{
	m_UIAxis = uiAxis;
}

/// <summary>
/// 
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
_DLL void Sprite2D::SetspriteIndex(unsigned int index)
{
	m_NowSpriteIndex = index;
}

/// <summary>
/// start�� �Ǳ��� ���ҽ� �Ŵ����� �����ؼ� �ε����� ���� �˷��ִ��Լ� 
/// </summary>
/// <param name="pResourceManager"></param>
void Sprite2D::ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_SpriteIndex_V.resize(m_SpriteName_V.size());

	for (int i = 0; i < m_SpriteName_V.size(); ++i)
	{
		m_SpriteIndex_V[i] = pResourceManager->GetSpriteDataIndex(m_SpriteName_V[i]);
	}

	m_NowSpriteIndex = 0;

	// �ؽ�Ʈ�� ũ������
	DirectX::SimpleMath::Vector3 _scale = DirectX::SimpleMath::Vector3(0, 0, 0);
	pResourceManager->GetSpriteScale(m_SpriteIndex_V[m_NowSpriteIndex], _scale);

	_scale.x = _scale.x * m_ProportionX;
	_scale.y = _scale.y * m_ProportionY;

	m_pMyObject->m_Transform->SetScale(_scale);
}

void Sprite2D::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	//save::WriteVectorValue(&(*m_Value)["m_SpriteName_V"], m_SpriteName_V);

	m_SaveData->m_ProportionX = m_ProportionX;
	m_SaveData->m_ProportionY = m_ProportionY;

	m_SaveData->m_PivotX = m_PivotX;
	m_SaveData->m_PivotY = m_PivotY;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Sprite2D::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);

	//save::ReadVectorValue(&(*m_Value)["m_SpriteName_V"], m_SpriteName_V);

	m_ProportionX = m_SaveData->m_ProportionX;
	m_ProportionY = m_SaveData->m_ProportionY;

	m_PivotX = m_SaveData->m_PivotX + 0.5f;
	m_PivotY = m_SaveData->m_PivotY + 0.5f;
}

