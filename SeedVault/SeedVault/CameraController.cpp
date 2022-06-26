#include "pch.h"
#include "CameraController.h"

CameraController::CameraController()
	: ComponentBase(ComponentType::GameLogic)
	, m_eMovingMode(CameraMovingMode::Normal)
	, m_DefaultSpeed(10.0f)
	, m_AccelerationSpeed(0.5f)
	, m_MaxSpeed(200.0f)
	, m_MoveSpeed(10.0f)
	, m_RotationAngles(Vector2::Zero)
	, m_SaveData(new CameraController_Save())
{
	m_RotationAngles = { 0.0f, 0.0f, 0.0f };
	//m_Transform->SetRotationFromVec(m_RotationAngles);

}

CameraController::~CameraController()
{
}

void CameraController::Start()
{
	// �ʱ� ���� ����
	m_Transform->SetRotationFromVec(m_RotationAngles);

	// �̵��ӵ� �ʱ�ȭ
	m_MoveSpeed = m_DefaultSpeed;
}

void CameraController::Update(float dTime)
{
	/// ��� üũ
	UpdateMovingMode();

	/// ���� ���� ��忡���� ī�޶� �̵�
	Move();

	/// <summary>
	/// ���콺 ��Ŭ���� ������ �ִ� ����
	/// ī�޶��� ȸ���� �Ѵ�.
	/// </summary>
	Rotate();
}

void CameraController::OnRender()
{
}

void CameraController::UpdateMovingMode()
{
	// Shift ������
	if (DLLInput::InputKey(VK_SHIFT))
	{
		// ���� ���� ����
		m_eMovingMode = CameraMovingMode::Acceleration;

		m_MoveSpeed += m_AccelerationSpeed;

		// �ִ� �ӵ� ����
		if (m_MoveSpeed > m_MaxSpeed)
		{
			m_MoveSpeed = m_MaxSpeed;
		}
	}

	// �ȴ�����
	else
	{
		// �븻����
		m_eMovingMode = CameraMovingMode::Normal;

		m_MoveSpeed = m_DefaultSpeed;
	}

	//CA_TRACE("%f ", m_MoveSpeed);
}

void CameraController::Move()
{
	/// �����¿� �̵�
	// �շ� �̵�
	if (DLLInput::InputKey('W'))
	{
		m_Transform->MoveForward(m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	// �ڷ� �̵�
	if (DLLInput::InputKey('S'))
	{
		// �̵��ӵ� ����
		float _MoveSpeed = m_DefaultSpeed;

		// ���� ���� ���� ���� ��ŭ �����ش�
		if (m_eMovingMode == CameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		m_Transform->MoveForward(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	// �·� �̵�
	if (DLLInput::InputKey('A'))
	{
		// �̵��ӵ� ����
		float _MoveSpeed = m_DefaultSpeed;

		// ���� ���� ���� ���� ��ŭ �����ش�
		if (m_eMovingMode == CameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		m_Transform->MoveSide(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	// ��� �̵�
	if (DLLInput::InputKey('D'))
	{
		// �̵��ӵ� ����
		float _MoveSpeed = m_DefaultSpeed;

		// ���� ���� ���� ���� ��ŭ �����ش�
		if (m_eMovingMode == CameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		m_Transform->MoveSide(m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	// �Ʒ��� �̵�
	if (DLLInput::InputKey('Q'))
	{
		// �̵��ӵ� ����
		float _MoveSpeed = m_DefaultSpeed;

		// ���� ���� ���� ���� ��ŭ �����ش�
		if (m_eMovingMode == CameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		Vector3 _moveDir = { 0.0f, -_MoveSpeed, -0.0f };
		_moveDir *= CL::Input::s_DeltaTime;

		m_Transform->Translate(_moveDir);
	}

	// ���� �̵�
	if (DLLInput::InputKey('E'))
	{
		// �̵��ӵ� ����
		float _MoveSpeed = m_DefaultSpeed;

		// ���� ���� ���� ���� ��ŭ �����ش�
		if (m_eMovingMode == CameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		Vector3 _moveDir = { 0.0f, _MoveSpeed, -0.0f };
		_moveDir *= CL::Input::s_DeltaTime;

		m_Transform->Translate(_moveDir);
	}



}

void CameraController::Rotate()
{
	if (DLLInput::InputKey(VK_RBUTTON))
	{
		// ���콺�� ���� ������ pos�� ���� ������ pos�� �̿��� ���̸� ����
		POINT _prevPos = DLLInput::GetPrevMouseClientPos();
		POINT _currentPos = DLLInput::GetMouseClientPos();

		//CA_TRACE("prev mouse pos [%d, %d]", _prevPos.x, _prevPos.y);
		//CA_TRACE("curr mouse pos [%d, %d]", _currentPos.x, _currentPos.y);

		float _deltaX = static_cast<float>(_currentPos.x - _prevPos.x);
		float _deltaY = static_cast<float>(_currentPos.y - _prevPos.y);

		// ȸ���� ����
		// ���콺 �Է� �ӵ�(�ΰ���)�� ���� 
		// ���� �����ϴ� ȸ�������� ������ �� ������ ��ŭ ���ؼ� ȸ������ ����
		m_RotationAngles +=
		{
			_deltaY* CL::KeyMap::MouseSensitivity,
				_deltaX* CL::KeyMap::MouseSensitivity
		};

		// ���� 0 ~ 360 ����
		m_RotationAngles = m_Transform->NormalizeAngles(m_RotationAngles);

		m_Transform->SetRotationFromVec(m_RotationAngles);
		//CA_TRACE("Camera EulerAngles = %f / %f / %f", m_RotationAngles.x, m_RotationAngles.y, 0.0f);
	}

	else
	{
		ReleaseCapture();
	}
}

void CameraController::SetRotationAngles(const Vector2 angles)
{
	SetRotationAngles({ angles.x, angles.y, 0.0f });
}

void CameraController::SetRotationAngles(const Vector3 angles)
{
	m_RotationAngles.x = angles.x;
	m_RotationAngles.y = angles.y;

	m_Transform->SetRotationFromVec(m_RotationAngles);
}

void CameraController::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_eMovingMode = static_cast<int>(m_eMovingMode);
	m_SaveData->m_RotationAngles = &m_RotationAngles;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void CameraController::LoadData()
{
	m_SaveData->m_RotationAngles = &m_RotationAngles;

	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_eMovingMode = static_cast<decltype(m_eMovingMode)>(m_SaveData->m_eMovingMode);
}
