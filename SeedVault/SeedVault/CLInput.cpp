#include "pch.h"

POINT CL::Input::s_MousePrevPos = { 0, 0 };
POINT CL::Input::s_MouseCurrentPos = { 0, 0 };

LONG CL::Input::s_MouseDeltaX = 0;
LONG CL::Input::s_MouseDeltaY = 0;

float CL::Input::s_MouseMoveOffset = 12.0f;

float CL::Input::s_AxisHorizontal = 0;
float CL::Input::s_AxisVertical = 0;
float CL::Input::s_AxisMouseX = 0;
float CL::Input::s_AxisMouseY = 0;

float CL::Input::s_DeltaTime = 0.0f;



CL::Input::Input()
{
	CA_TRACE("new Input()");
}

CL::Input::~Input()
{
	CA_TRACE("delete Input()");
}

void CL::Input::Update()
{
	s_DeltaTime = DLLTime::DeltaTime();
	if (s_DeltaTime < 0.0f)
	{
		s_DeltaTime = 0.0f;
	}
	//CA_TRACE("%f", s_DeltaTime);

	// Update Mouse Position 
	UpdateMousePosition();

	// Update Axis
	UpdateAxisVerticalAndHorizontal();	// 키 축(Horizontal / Vertical) 업데이트
	UpdateAxisMouseXAndMouseY();		// 마우스 축(MouseX / MouseY) 업데이트

	//CA_TRACE("%f, %f", s_AxisMouseX, s_AxisMouseY);
}

void CL::Input::UpdateMousePosition()
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

void CL::Input::UpdateAxisVerticalAndHorizontal()
{
	/// 수직방향으로 입력이 있을 경우
	/// 해당 방향으로 Vertical 값을 증가(감소) 한다.
		// Check Postive Vertical Input
		// 윗키 입력이 있는 경우
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_W)))
	{
		// Vertical 값 증가
		s_AxisVertical += s_DeltaTime;

		// Max 값 제한
		if (s_AxisVertical > 1.0f)
		{
			s_AxisVertical = 1.0f;
		}
	}

	// 윗키 입력이 없는 경우
	else if (s_AxisVertical > 0.0f)
	{
		s_AxisVertical -= CL::Input::s_DeltaTime;

		if (s_AxisVertical < 0.0f)
		{
			s_AxisVertical = 0.0f;
		}
	}

	// Check Negative Vertical Input
	// 아랫키 입력이 있는 경우
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_S)))
	{
		// Vertical 값 감소
		s_AxisVertical -= s_DeltaTime;

		// Min 값 제한
		if (s_AxisVertical < -1.0f)
		{
			s_AxisVertical = -1.0f;
		}
	}

	// 아랫키 입력이 없는 경우
	else if (s_AxisVertical < 0.0f)
	{
		s_AxisVertical += CL::Input::s_DeltaTime;

		if (s_AxisVertical > 0.0f)
		{
			s_AxisVertical = 0.0f;
		}
	}


	/// 수평방향으로 입력이 있을 경우
	/// 해당 방향으로 Horizontal 값을 증가(감소) 한다.

	// Check Negative Horizontal Input
	// 왼쪽 키 입력이 있는 경우
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_A)))
	{
		// Horizontal 값 감소
		s_AxisHorizontal -= s_DeltaTime;

		// Min 값 제한
		if (s_AxisHorizontal < -1.0f)
		{
			s_AxisHorizontal = -1.0f;
		}
	}

	// 왼쪽 키 입력이 없는 경우
	else if (s_AxisHorizontal < 0.0f)
	{
		s_AxisHorizontal += CL::Input::s_DeltaTime;

		if (s_AxisHorizontal > 0.0f)
		{
			s_AxisHorizontal = 0.0f;
		}
	}

	// Check Postive Horizontal Input
	// 오른쪽 키 입력이 있는 경우
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_D)))
	{
		// Horizontal 값 증가
		s_AxisHorizontal += s_DeltaTime;

		// Max 값 제한
		if (s_AxisHorizontal > 1.0f)
		{
			s_AxisHorizontal = 1.0f;
		}
	}

	// 오른쪽 키 입력이 없는 경우
	else if (s_AxisHorizontal > 0.0f)
	{
		s_AxisHorizontal -= CL::Input::s_DeltaTime;

		if (s_AxisHorizontal < 0.0f)
		{
			s_AxisHorizontal = 0.0f;
		}
	}

}

void CL::Input::UpdateAxisMouseXAndMouseY()
{
	// 마우스 움직임이 있으면 offset 만큼 곱해서 보정해준다.
	float _delta = CL::Input::s_DeltaTime * s_MouseMoveOffset;

	/// 마우스 수직 방향으로 이동이 있을 경우
	/// 해당 방향으로 MouseY 값을 증가(감소) 한다.

	// Check Postive MouseY Input
	// 위로 이동한 경우
	if (s_MouseDeltaY > 0)
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
	else if (s_MouseDeltaY < 0)
	{
		// Vertical 값 감소
		s_AxisMouseY -= _delta;

		// Min 값 제한
		if (s_AxisMouseY < -1.0f)
		{
			s_AxisMouseY = -1.0f;
		}
	}


	/// 마우스 수직 방향으로 이동이 없을 경우
	/// 현재 AxisMouseY 값의 부호를 확인해서 더하거나 빼서 점점 0에 수렴하게 한다.
	else if (s_MouseDeltaY == 0)
	{
		// 현재 MouseY 가 양수인 경우
		if (s_AxisMouseY > 0.0f)
		{
			s_AxisMouseY -= CL::Input::s_DeltaTime;

			if (s_AxisMouseY < 0.0f)
			{
				s_AxisMouseY = 0.0f;
			}
		}

		// 현재 MouseY 가 음수인 경우
		else if (s_AxisMouseY < 0.0f)
		{
			s_AxisMouseY += CL::Input::s_DeltaTime;

			if (s_AxisMouseY > 0.0f)
			{
				s_AxisMouseY = 0.0f;
			}
		}
	}

	/// 마우스 수평 방향으로 이동이 있을 경우
	/// 해당 방향으로 MouseX 값을 증가(감소) 한다.

	// Check Postive MouseX Input
	// 우로 이동한 경우
	if (s_MouseDeltaX > 0)
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
	else if (s_MouseDeltaX < 0)
	{
		// Vertical 값 감소
		s_AxisMouseX -= _delta;

		// Min 값 제한
		if (s_AxisMouseX < -1.0f)
		{
			s_AxisMouseX = -1.0f;
		}
	}


	/// 마우스 수평 방향으로 이동이 없을 경우
	/// 현재 AxisMouseX 값의 부호를 확인해서 더하거나 빼서 점점 0에 수렴하게 한다.
	else if (s_MouseDeltaX == 0)
	{
		// 현재 MouseX 가 양수인 경우
		if (s_AxisMouseX > 0.0f)
		{
			s_AxisMouseX -= CL::Input::s_DeltaTime;

			if (s_AxisMouseX < 0.0f)
			{
				s_AxisMouseX = 0.0f;
			}
		}

		// 현재 MouseX 가 음수인 경우
		else if (s_AxisMouseX < 0.0f)
		{
			s_AxisMouseX += CL::Input::s_DeltaTime;

			if (s_AxisMouseX > 0.0f)
			{
				s_AxisMouseX = 0.0f;
			}
		}
	}
}


bool CL::Input::IsVerticalInputed()
{
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_W))
		// || DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_UP)) 
		|| DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_S))
		// || DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_DOWN)
		)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool CL::Input::IsHorizontalInputed()
{
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_A))
		//|| DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_LEFT)) 
		|| DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_D))
		//|| DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_RIGHT))
		)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool CL::Input::IsUpInputed()
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

bool CL::Input::IsDownInputed()
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

bool CL::Input::IsLeftInputed()
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

bool CL::Input::IsRightInputed()
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

bool CL::Input::IsMouseYInputed()
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

bool CL::Input::IsMouseXInputed()
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

bool CL::Input::IsMouseMovedUp()
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

bool CL::Input::IsMouseMovedDown()
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

bool CL::Input::IsMouseMovedLeft()
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

bool CL::Input::IsMouseMovedRight()
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

float CL::Input::GetAxis(const std::string axisName)
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

	// 그 외 예외처리
	assert(false);
	return 0.0f;
}

float CL::Input::GetAxisRaw(const std::string axisName)
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
	CA_TRACE("Wrong Axis Name Inputed");

	return 999;
}
