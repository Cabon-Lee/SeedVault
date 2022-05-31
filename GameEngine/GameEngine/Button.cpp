#include "pch.h"
#include "Button.h"
#include "EngineDLL.h"

#include "IRenderer.h"
#include "IResourceManager.h"



Button::Button()
	: ComponentBase(ComponentType::Rendering)
	, m_SaveData(new Button_Save())
{
}

Button::~Button()
{
	Safe_Delete(m_SaveData);

	m_pEngine = nullptr;
	m_pCamera = nullptr;
}

void Button::Start()
{
	m_pEngine = DLLEngine::GetEngine();
	m_pCamera = Managers::GetInstance()->GetCameraManager()->GetNowCamera();
	m_pRenderer = m_pEngine->GetIRenderer();

	m_worldTM =
		XMMatrixScaling(m_pMyObject->m_Transform->m_Scale.x, m_pMyObject->m_Transform->m_Scale.y, 1.0f) *
		XMMatrixRotationRollPitchYaw(m_pMyObject->m_Transform->m_Rotation.x, m_pMyObject->m_Transform->m_Rotation.y + XM_PI, m_pMyObject->m_Transform->m_Rotation.z + XM_PI) *
		XMMatrixTranslation(m_pMyObject->m_Transform->m_Position.x + m_pMyObject->m_Transform->m_Scale.x / 2.0f,
			m_pMyObject->m_Transform->m_Position.y + m_pMyObject->m_Transform->m_Scale.y / 2.0f,
			m_pMyObject->m_Transform->m_Position.z);
}

void Button::Update(float dTime)
{
}

void Button::OnRender()
{
	
}

void Button::OnUIRender()
{
	m_worldTM =
		XMMatrixScaling(m_pMyObject->m_Transform->m_Scale.x, m_pMyObject->m_Transform->m_Scale.y, 1.0f) *
		XMMatrixRotationRollPitchYaw(m_pMyObject->m_Transform->m_Rotation.x, m_pMyObject->m_Transform->m_Rotation.y + XM_PI, m_pMyObject->m_Transform->m_Rotation.z + XM_PI) *
		XMMatrixTranslation(m_pMyObject->m_Transform->m_Position.x + m_pMyObject->m_Transform->m_Scale.x / 2.0f,
			m_pMyObject->m_Transform->m_Position.y + m_pMyObject->m_Transform->m_Scale.y / 2.0f,
			m_pMyObject->m_Transform->m_Position.z);

	m_NowState = eButtonState::OFF;
	m_IsPushedBefore = m_IsPushed;
	m_IsPushed = false;

	//CA_TRACE("%d  %d", m_pEngine->GetInput()->GetMouseClientPos().x, m_pEngine->GetInput()->GetMouseClientPos().y);
	if (Intersect(m_pEngine->GetInput()->GetMouseClientPos()))
	{
		m_NowState = eButtonState::OVER;

		if (m_pEngine->GetInput()->GetKey(VK_LBUTTON))
		{
			m_IsPushed = true;
			m_NowState = eButtonState::PUT;
		}
	}

	/// 클릭 상태에 따라 렌더링 변경 및 클릭 기능 실행
	// 눌렸는지 아닌지 판단이 되면, 필요한 것만 그려준다
	switch (m_NowState)
	{
	case eButtonState::PUT:
		m_pRenderer->DrawSprite(eResourceType::eSingleImage, m_OnSpriteIndex, m_worldTM);
		if (m_onClick && m_IsPushed == true && m_IsPushedBefore == false)
		{
			m_onClick();
		}
		break;

	case eButtonState::OVER:
		if (m_AbleOver)
		{
			m_pRenderer->DrawSprite(eResourceType::eSingleImage, m_OverSpriteIndex, m_worldTM);
		}
		else
		{
			m_pRenderer->DrawSprite(eResourceType::eSingleImage, m_OffSpriteIndex, m_worldTM);
		}
		break;

	case eButtonState::OFF:
	default:
		m_pRenderer->DrawSprite(eResourceType::eSingleImage, m_OffSpriteIndex, m_worldTM);
		break;
	}

}

void Button::SetFunc(std::function<void()> onClick)
{
	m_onClick = onClick;
}

void Button::SetProportion(float x = 1, float y = 1)
{
	m_ProportionX = x;
	m_ProportionY = y;
}

void Button::SetPivot(float pivotX = 0.5f, float pivotY = 0.5f)
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

void Button::SetOverSprite(bool ableOver)
{
	m_AbleOver = ableOver;
}


bool Button::Intersect(POINT mousePos)
{
	RECT _nowRect = GetSpriteRect();

	if ((mousePos.x > _nowRect.left && _nowRect.right > mousePos.x) &&
		(mousePos.y > _nowRect.top && _nowRect.bottom > mousePos.y))
	{
		return true;
	}

	return false;
}

RECT Button::GetSpriteRect()
{
	RECT _newRect;

	_newRect.left = (m_pMyObject->m_Transform->m_Position.x);
	_newRect.top = (m_pMyObject->m_Transform->m_Position.y);
	_newRect.right = (m_pMyObject->m_Transform->m_Position.x + m_Rect.right * m_ProportionX);
	_newRect.bottom = (m_pMyObject->m_Transform->m_Position.y + m_Rect.bottom * m_ProportionY);

	return _newRect;
}

void Button::ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_OffSpriteIndex = pResourceManager->GetSpriteDataIndex(m_OffSprite);
	m_OnSpriteIndex = pResourceManager->GetSpriteDataIndex(m_OnSprite);
	m_OnSpriteIndex = pResourceManager->GetSpriteDataIndex(m_OverSprite);

	// 버튼하나에 필요한 3가지 버튼의 이미지의 크기는 같다고 가정한다
	m_Rect = pResourceManager->GetTextureRect(m_OffSpriteIndex);

	// 텍스트의 크기정보
	DirectX::SimpleMath::Vector3 _scale = DirectX::SimpleMath::Vector3(0, 0, 0);
	pResourceManager->GetSpriteScale(m_OffSpriteIndex, _scale);

	_scale.x = _scale.x * m_ProportionX;
	_scale.y = _scale.y * m_ProportionY;

	// 트랜스폼에 크기를 저장한다
	m_pMyObject->m_Transform->SetScale(_scale);
}

void Button::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_OffSprite = m_OffSprite;
	m_SaveData->m_OnSprite = m_OnSprite;

	m_SaveData->m_ProportionX = m_ProportionX;
	m_SaveData->m_ProportionY = m_ProportionY;

	m_SaveData->m_PivotX = m_PivotX;
	m_SaveData->m_PivotY = m_PivotY;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Button::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);

	m_OffSprite = m_SaveData->m_OffSprite;
	m_OnSprite = m_SaveData->m_OnSprite;

	m_ProportionX = m_SaveData->m_ProportionX;
	m_ProportionY = m_SaveData->m_ProportionY;

	m_PivotX = m_SaveData->m_PivotX + 0.5f;
	m_PivotY = m_SaveData->m_PivotY + 0.5f;
}

