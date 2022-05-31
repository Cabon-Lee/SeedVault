#include "pch.h"
#include "QtInput.h"

POINT QT::Input::s_MousePrevPos = { 0, 0 };
POINT QT::Input::s_MouseCurrentPos = { 0, 0 };

LONG QT::Input::s_MouseDeltaX = 0;
LONG QT::Input::s_MouseDeltaY = 0;

float QT::Input::s_MouseMoveOffset = 12.0f;

float QT::Input::s_AxisHorizontal = 0;
float QT::Input::s_AxisVertical = 0;
float QT::Input::s_AxisMouseX = 0;
float QT::Input::s_AxisMouseY = 0;

QT::Input::Input()
{
	//CA_TRACE("new Input()");
}

QT::Input::~Input()
{
	//CA_TRACE("delete Input()");
}

void QT::Input::Update()
{
	// Update Mouse Position 
	UpdateMousePosition();

	// Update Axis
	UpdateAxisVerticalAndHorizontal();	// 키 축(Horizontal / Vertical) 업데이트
	UpdateAxisMouseXAndMouseY();		// 마우스 축(MouseX / MouseY) 업데이트

}

void QT::Input::UpdateMousePosition()
{
	// 마우스 포지션 갱신
	s_MousePrevPos = DLLInput::GetPrevMouseClientPos();
	s_MouseCurrentPos = DLLInput::GetMouseClientPos();

	//CA_TRACE("s_MouseCurrentPos = %d | %d", s_MouseCurrentPos.x, s_MouseCurrentPos.y);

	// 마우스 이동량 계산
	s_MouseDeltaX = (s_MouseCurrentPos.x - s_MousePrevPos.x);	// 수평
	s_MouseDeltaY = -(s_MouseCurrentPos.y - s_MousePrevPos.y);	// 수직, 윈도우 좌표계가 Dx좌표계와 y축이 반대라서 부호를 반전시킨다.

	//CA_TRACE("%d", s_MouseDeltaX);
}

void QT::Input::UpdateAxisVerticalAndHorizontal()
{
	float _delta = DLLTime::DeltaTime();	// 최종적으로 Horizontal 이나 Vertical 값에 가감할 값
	
	// QT::Input::Update() 는 엔진과 별개로 처리되기 때문에
	// QT::Input::Update() 가 Component::Update() 보다 훨 씬 자주 업데이트 된다.
	// 일부러 작은 수를 곱해서 보정하지 않으면
	// AxisLaw() 의 값이 너무 빨리 -1, +1에 도달해버리게 되고 그 중간 소수점 부분이 눈깜짝할 새에 스킵 되어버린다..
	// 그래서 일부러 만든 애니메이션 트랜지션이 원하느 대로 작동하지 않았다..
	// _delta 를 임의로 엔진의 업데이트 주기와 맞추기 위해 앤잔의 목표 프레임인 1 / 60 의 시간만큼 곱해서 보정해주자.
	_delta *= 1.0f / 60.0f;



	/// 수직방향으로 입력이 있을 경우
	/// 해당 방향으로 Vertical 값을 증가(감소) 한다.
	if (IsVerticalInputed())
	{
		// Check Postive Vertical Input
		// 윗키 입력이 있는 경우
		if (IsUpInputed())
		{
			// Vertical 값 증가
			s_AxisVertical += _delta;

			// Max 값 제한
			if (s_AxisVertical > 1.0f)
			{
				s_AxisVertical = 1.0f;
			}
		}

		// Check Negative Vertical Input
		// 아랫키 입력이 있는 경우
		if (IsDownInputed())
		{
			// Vertical 값 감소
			s_AxisVertical -= _delta;

			// Min 값 제한
			if (s_AxisVertical < -1.0f)
			{
				s_AxisVertical = -1.0f;
			}
		}
	}

	/// 수직방향 입력이 없을 경우
	/// 현재 Vertical 값의 부호를 확인해서 더하거나 빼서 점점 0에 수렴하게 한다.
	else
	{
		// 현재 Vertical이 양수인 경우
		if (s_AxisVertical > 0.0f)
		{
			s_AxisVertical -= DLLTime::DeltaTime();

			if (s_AxisVertical < 0.0f)
			{
				s_AxisVertical = 0.0f;
			}
		}

		// 현재 Vertical이 음수인 경우
		else if (s_AxisVertical < 0.0f)
		{
			s_AxisVertical += DLLTime::DeltaTime();

			if (s_AxisVertical > 0.0f)
			{
				s_AxisVertical = 0.0f;
			}
		}
	}


	/// 수평방향으로 입력이 있을 경우
	/// 해당 방향으로 Horizontal 값을 증가(감소) 한다.
	if (IsHorizontalInputed())
	{
		// Check Negative Horizontal Input
		// 왼쪽 키 입력이 있는 경우
		if (IsLeftInputed())
		{
			// Horizontal 값 감소
			s_AxisHorizontal -= _delta;

			// Min 값 제한
			if (s_AxisHorizontal < -1.0f)
			{
				s_AxisHorizontal = -1.0f;
			}
		}

		// Check Postive Horizontal Input
		// 오른쪽 키 입력이 있는 경우
		if (IsRightInputed())
		{
			// Horizontal 값 증가
			s_AxisHorizontal += _delta;

			// Max 값 제한
			if (s_AxisHorizontal > 1.0f)
			{
				s_AxisHorizontal = 1.0f;
			}
		}
	}

	/// 수평방향 입력이 없을 경우
	/// 현재 Horizontal 값의 부호를 확인해서 더하거나 빼서 점점 0에 수렴하게 한다.
	else
	{
		// 현재 Horizontal 이 양수인 경우
		if (s_AxisHorizontal > 0.0f)
		{
			s_AxisHorizontal -= DLLTime::DeltaTime();

			if (s_AxisHorizontal < 0.0f)
			{
				s_AxisHorizontal = 0.0f;
			}
		}

		// 현재 Horizontal 이 음수인 경우
		else if (s_AxisHorizontal < 0.0f)
		{
			s_AxisHorizontal += DLLTime::DeltaTime();

			if (s_AxisHorizontal > 0.0f)
			{
				s_AxisHorizontal = 0.0f;
			}
		}
	}
}

void QT::Input::UpdateAxisMouseXAndMouseY()
{
	// 마우스 움직임이 있으면 offset 만큼 곱해서 보정해준다.
	float _delta = DLLTime::DeltaTime() * s_MouseMoveOffset;

	/// 마우스 수직 방향으로 이동이 있을 경우
	/// 해당 방향으로 MouseY 값을 증가(감소) 한다.
	if (IsMouseYInputed())
	{
		// Check Postive MouseY Input
		// 위로 이동한 경우
		if (IsMouseMovedUp())
		{
			// Vertical 값 증가
			s_AxisMouseY += _delta;

			// Max 값 제한
			if (s_AxisMouseY > 1.0f)
			{
				s_AxisMouseY = 1.0f;
			}
		}

		// Check Negative MouseY Input
		// 아래로 이동한 경우
		if (IsMouseMovedDown())
		{
			// Vertical 값 감소
			s_AxisMouseY -= _delta;

			// Min 값 제한
			if (s_AxisMouseY < -1.0f)
			{
				s_AxisMouseY = -1.0f;
			}
		}
	}

	/// 마우스 수직 방향으로 이동이 없을 경우
	/// 현재 AxisMouseY 값의 부호를 확인해서 더하거나 빼서 점점 0에 수렴하게 한다.
	else
	{
		// 현재 MouseY 가 양수인 경우
		if (s_AxisMouseY > 0.0f)
		{
			s_AxisMouseY -= DLLTime::DeltaTime();

			if (s_AxisMouseY < 0.0f)
			{
				s_AxisMouseY = 0.0f;
			}
		}

		// 현재 MouseY 가 음수인 경우
		else if (s_AxisMouseY < 0.0f)
		{
			s_AxisMouseY += DLLTime::DeltaTime();

			if (s_AxisMouseY > 0.0f)
			{
				s_AxisMouseY = 0.0f;
			}
		}
	}

	/// 마우스 수평 방향으로 이동이 있을 경우
	/// 해당 방향으로 MouseX 값을 증가(감소) 한다.
	if (IsMouseXInputed())
	{
		// Check Postive MouseX Input
		// 우로 이동한 경우
		if (IsMouseMovedRight())
		{
			// MouseX 값 증가
			s_AxisMouseX += _delta;

			// Max 값 제한
			if (s_AxisMouseX > 1.0f)
			{
				s_AxisMouseX = 1.0f;
			}
		}

		// Check Negative MouseX Input
		// 좌로 이동한 경우
		if (IsMouseMovedLeft())
		{
			// Vertical 값 감소
			s_AxisMouseX -= _delta;

			// Min 값 제한
			if (s_AxisMouseX < -1.0f)
			{
				s_AxisMouseX = -1.0f;
			}
		}
	}

	/// 마우스 수평 방향으로 이동이 없을 경우
	/// 현재 AxisMouseX 값의 부호를 확인해서 더하거나 빼서 점점 0에 수렴하게 한다.
	else
	{
		// 현재 MouseX 가 양수인 경우
		if (s_AxisMouseX > 0.0f)
		{
			s_AxisMouseX -= DLLTime::DeltaTime();

			if (s_AxisMouseX < 0.0f)
			{
				s_AxisMouseX = 0.0f;
			}
		}

		// 현재 MouseX 가 음수인 경우
		else if (s_AxisMouseX < 0.0f)
		{
			s_AxisMouseX += DLLTime::DeltaTime();

			if (s_AxisMouseX > 0.0f)
			{
				s_AxisMouseX = 0.0f;
			}
		}
	}
}


bool QT::Input::IsVerticalInputed()
{
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_W)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_UP)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_S)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_DOWN))
		)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QT::Input::IsHorizontalInputed()
{
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_A)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_LEFT)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_D)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_RIGHT))
		)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QT::Input::IsUpInputed()
{
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_W)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_UP))
		)
	{
		return true;
	}

	else
	{
		return false;

	}
}

bool QT::Input::IsDownInputed()
{
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_S)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_DOWN))
		)
	{
		return true;
	}

	else
	{
		return false;

	}
}

bool QT::Input::IsLeftInputed()
{
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_A)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_LEFT))
		)
	{
		return true;
	}

	else
	{
		return false;

	}
}

bool QT::Input::IsRightInputed()
{
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_D)) ||
		DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_RIGHT))
		)
	{
		return true;
	}

	else
	{
		return false;

	}
}

bool QT::Input::IsMouseYInputed()
{
	if (s_MouseDeltaY != 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QT::Input::IsMouseXInputed()
{
	if (s_MouseDeltaX != 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QT::Input::IsMouseMovedUp()
{
	if (s_MouseDeltaY > 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QT::Input::IsMouseMovedDown()
{
	if (s_MouseDeltaY < 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QT::Input::IsMouseMovedLeft()
{
	if (s_MouseDeltaX < 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QT::Input::IsMouseMovedRight()
{
	if (s_MouseDeltaX > 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}

float QT::Input::GetAxis(const std::string axisName)
{
	if (axisName == "Horizontal")
	{
		return s_AxisHorizontal;
	}

	else if (axisName == "Vertical")
	{
		return s_AxisVertical;
	}

	else if (axisName == "Mouse X")
	{
		return s_AxisMouseX;
	}

	else if (axisName == "Mouse Y")
	{
		return s_AxisMouseY;
	}

}

float QT::Input::GetAxisRaw(const std::string axisName)
{

	if (axisName == "Vertical")
	{
		// 키 수직 입력이 있는 경우
		if (IsVerticalInputed())
		{
			// 윗키 입력이면
			if (IsUpInputed())
			{
				return 1.0f;
			}

			// 아랫키 입력이면
			else if (IsDownInputed())
			{
				return -1.0f;
			}
		}

		// 수직입력이 없으면
		else
		{
			return 0.0f;
		}
	}

	else if (axisName == "Horizontal")
	{
		// 키 수평 입력이 있는 경우
		if (IsHorizontalInputed())
		{
			// 오른쪽 키 입력이면
			if (IsRightInputed())
			{
				return 1.0f;
			}

			// 왼쪽 키 입력이면
			else if (IsLeftInputed())
			{
				return -1.0f;
			}
		}

		// 수평 입력이 없으면
		else
		{
			return 0.0f;
		}
	}

	else if (axisName == "Mouse X")
	{
		// 마우스 수평 이동이 있는 경우
		if (IsMouseXInputed())
		{
			// 양의 이동이면
			if (IsMouseMovedRight())
			{
				return 1.0f;
			}

			// 음의 이동이면
			else if (IsMouseMovedLeft())
			{
				return -1.0f;
			}
		}

		// 수평이동이 없으면
		else
		{
			return 0.0f;
		}
	}

	else if (axisName == "Mouse Y")
	{
		// 마우스 수직 이동이 있는 경우
		if (IsMouseYInputed())
		{
			// 위로 이동이면
			if (IsMouseMovedUp())
			{
				return 1.0f;
			}

			// 아래로 이동이면
			else if (IsMouseMovedDown())
			{
				return -1.0f;
			}
		}

		// 수직 이동이 없으면
		else
		{
			return 0.0f;
		}
	}

	// 잘못된 값이 들어온 경우
	//CA_TRACE("Wrong Axis Name Inputed");

	return 999;
}
