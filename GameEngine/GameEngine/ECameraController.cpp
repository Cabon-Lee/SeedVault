#include "pch.h"
#include "EngineDLL.h"
#include "Timer.h"

#include "ECameraController.h"

ECameraController::ECameraController()
	: ComponentBase(ComponentType::Etc)
	, m_eMovingMode(ECameraMovingMode::Normal)
	, m_DefaultSpeed(1.0f)
	, m_AccelerationSpeed(0.5f)
	, m_MaxSpeed(200.0f)
	, m_MoveSpeed(m_DefaultSpeed)
	, m_RotationAngles(Vector2::Zero)
	, m_SaveData(new ECameraController_Save())
{
	m_RotationAngles = { 0.0f, 0.0f, 0.0f };
	//m_Transform->SetRotationFromVec(m_RotationAngles);

}

ECameraController::~ECameraController()
{
}

void ECameraController::Start()
{
	// 초기 각도 세팅
	m_Transform->SetRotationFromVec(m_RotationAngles);

}

void ECameraController::Update(float dTime)
{
	
	/// 무빙 모드 체크
	UpdateMovingMode();

	/// 자유 시점 모드에서의 카메라 이동
	Move();
	/// <summary>
	/// 마우스 우클릭을 누르고 있는 동안
	/// 카메라의 회전을 한다.
	/// </summary>
	Rotate();
}

void ECameraController::OnRender()
{
}

_DLL void ECameraController::SetMovingVelocity(float speed)
{
		// 이동속도 초기화
	m_DefaultSpeed = speed;
}

_DLL const float ECameraController::GetMovingVelocity()
{
	return m_DefaultSpeed;
}

void ECameraController::UpdateMovingMode()
{
	// Shift 눌리면
	if (DLLInput::InputKey(VK_SHIFT))
	{
		// 가속 모드로 변경
		m_eMovingMode = ECameraMovingMode::Acceleration;

		m_MoveSpeed += m_AccelerationSpeed;

		// 최대 속도 제한
		if (m_MoveSpeed > m_MaxSpeed)
		{
			m_MoveSpeed = m_MaxSpeed;
		}
	}

	// 안눌리면
	else
	{
		// 노말모드로
		m_eMovingMode = ECameraMovingMode::Normal;

		m_MoveSpeed = m_DefaultSpeed;
	}

	//CA_TRACE("%f ", m_MoveSpeed);
}

void ECameraController::Move()
{
	/// 전후좌우 이동
	// 앞로 이동
	if (DLLInput::InputKey('W'))
	{
		m_Transform->MoveForward(m_MoveSpeed * DLLTime::DeltaTime());
	}

	// 뒤로 이동
	if (DLLInput::InputKey('S'))
	{
		// 이동속도 설정
		float _MoveSpeed = m_DefaultSpeed;

		// 가속 모드면 가속 배율 만큼 곱해준다
		if (m_eMovingMode == ECameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		m_Transform->MoveForward(-m_MoveSpeed * DLLTime::DeltaTime());
	}

	// 좌로 이동
	if (DLLInput::InputKey('A'))
	{
		// 이동속도 설정
		float _MoveSpeed = m_DefaultSpeed;

		// 가속 모드면 가속 배율 만큼 곱해준다
		if (m_eMovingMode == ECameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		m_Transform->MoveSide(-m_MoveSpeed * DLLTime::DeltaTime());
	}

	// 우로 이동
	if (DLLInput::InputKey('D'))
	{
		// 이동속도 설정
		float _MoveSpeed = m_DefaultSpeed;

		// 가속 모드면 가속 배율 만큼 곱해준다
		if (m_eMovingMode == ECameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		m_Transform->MoveSide(m_MoveSpeed * DLLTime::DeltaTime());
	}

	// 아래로 이동
	if (DLLInput::InputKey('Q'))
	{
		// 이동속도 설정
		float _MoveSpeed = m_DefaultSpeed;

		// 가속 모드면 가속 배율 만큼 곱해준다
		if (m_eMovingMode == ECameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		Vector3 _moveDir = { 0.0f, -_MoveSpeed, -0.0f };
		_moveDir *= DLLTime::DeltaTime();

		m_Transform->Translate(_moveDir);
	}

	// 위로 이동
	if (DLLInput::InputKey('E'))
	{
		// 이동속도 설정
		float _MoveSpeed = m_DefaultSpeed;

		// 가속 모드면 가속 배율 만큼 곱해준다
		if (m_eMovingMode == ECameraMovingMode::Acceleration)
		{
			_MoveSpeed *= m_AccelerationSpeed;
		}

		Vector3 _moveDir = { 0.0f, _MoveSpeed, -0.0f };
		_moveDir *= DLLTime::DeltaTime();

		m_Transform->Translate(_moveDir);
	}



}

void ECameraController::Rotate()
{
	if (DLLInput::InputKey(VK_RBUTTON))
	{
		// 마우스의 이전 프레임 pos와 현재 프레임 pos를 이용해 차이를 구함
		POINT _prevPos = DLLInput::GetPrevMouseClientPos();
		POINT _currentPos = DLLInput::GetMouseClientPos();

		//CA_TRACE("prev mouse pos [%d, %d]", _prevPos.x, _prevPos.y);
		//CA_TRACE("curr mouse pos [%d, %d]", _currentPos.x, _currentPos.y);

		float _deltaX = static_cast<float>(_currentPos.x - _prevPos.x);
		float _deltaY = static_cast<float>(_currentPos.y - _prevPos.y);

		// 회전각 조절
		// 마우스 입력 속도(민감도)가 빨라서 
		// 직접 조정하는 회전각도를 조절할 때 오프셋 만큼 곱해서 회전량을 조절
		m_RotationAngles +=
		{
			_deltaY* TEMPKEY::KeyMap::MouseSensitivity,
				_deltaX* TEMPKEY::KeyMap::MouseSensitivity
		};

		// 각도 0 ~ 360 제한
		m_RotationAngles = m_Transform->NormalizeAngles(m_RotationAngles);

		m_Transform->SetRotationFromVec(m_RotationAngles);
		//CA_TRACE("Camera EulerAngles = %f / %f / %f", m_RotationAngles.x, m_RotationAngles.y, 0.0f);
	}

	else
	{
		///이 주석이 풀리면 윈도우 창을 옮길 수 없다
		//ReleaseCapture();
	}
}

void ECameraController::SetRotationAngles(const Vector2 angles)
{
	SetRotationAngles({ angles.x, angles.y, 0.0f });
}

void ECameraController::SetRotationAngles(const Vector3 angles)
{
	m_RotationAngles.x = angles.x;
	m_RotationAngles.y = angles.y;

	m_Transform->SetRotationFromVec(m_RotationAngles);
}

void ECameraController::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_eMovingMode = static_cast<int>(m_eMovingMode);
	m_SaveData->m_RotationAngles = &m_RotationAngles;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	CA_TRACE("ECameraController : SaveData");
}

void ECameraController::LoadData()
{
	m_SaveData->m_RotationAngles = &m_RotationAngles;

	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_eMovingMode = static_cast<decltype(m_eMovingMode)>(m_SaveData->m_eMovingMode);
	CA_TRACE("ECameraController : LoadData");
}
