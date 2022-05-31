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
	UpdateAxisVerticalAndHorizontal();	// Ű ��(Horizontal / Vertical) ������Ʈ
	UpdateAxisMouseXAndMouseY();		// ���콺 ��(MouseX / MouseY) ������Ʈ

	//CA_TRACE("%f, %f", s_AxisMouseX, s_AxisMouseY);
}

void CL::Input::UpdateMousePosition()
{
	// ���콺 ������ ����
	s_MousePrevPos = DLLInput::GetPrevMouseClientPos();
	s_MouseCurrentPos = DLLInput::GetMouseClientPos();

	//CA_TRACE("s_MouseCurrentPos = %d | %d", s_MouseCurrentPos.x, s_MouseCurrentPos.y);

	// ���콺 �̵��� ���
	s_MouseDeltaX = (s_MouseCurrentPos.x - s_MousePrevPos.x);	// ����
	s_MouseDeltaY = -(s_MouseCurrentPos.y - s_MousePrevPos.y);	// ����, ������ ��ǥ�谡 Dx��ǥ��� y���� �ݴ�� ��ȣ�� ������Ų��.

	//CA_TRACE("%d", s_MouseDeltaX);
}

void CL::Input::UpdateAxisVerticalAndHorizontal()
{
	/// ������������ �Է��� ���� ���
	/// �ش� �������� Vertical ���� ����(����) �Ѵ�.
		// Check Postive Vertical Input
		// ��Ű �Է��� �ִ� ���
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_W)))
	{
		// Vertical �� ����
		s_AxisVertical += s_DeltaTime;

		// Max �� ����
		if (s_AxisVertical > 1.0f)
		{
			s_AxisVertical = 1.0f;
		}
	}

	// ��Ű �Է��� ���� ���
	else if (s_AxisVertical > 0.0f)
	{
		s_AxisVertical -= CL::Input::s_DeltaTime;

		if (s_AxisVertical < 0.0f)
		{
			s_AxisVertical = 0.0f;
		}
	}

	// Check Negative Vertical Input
	// �Ʒ�Ű �Է��� �ִ� ���
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_S)))
	{
		// Vertical �� ����
		s_AxisVertical -= s_DeltaTime;

		// Min �� ����
		if (s_AxisVertical < -1.0f)
		{
			s_AxisVertical = -1.0f;
		}
	}

	// �Ʒ�Ű �Է��� ���� ���
	else if (s_AxisVertical < 0.0f)
	{
		s_AxisVertical += CL::Input::s_DeltaTime;

		if (s_AxisVertical > 0.0f)
		{
			s_AxisVertical = 0.0f;
		}
	}


	/// ����������� �Է��� ���� ���
	/// �ش� �������� Horizontal ���� ����(����) �Ѵ�.

	// Check Negative Horizontal Input
	// ���� Ű �Է��� �ִ� ���
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_A)))
	{
		// Horizontal �� ����
		s_AxisHorizontal -= s_DeltaTime;

		// Min �� ����
		if (s_AxisHorizontal < -1.0f)
		{
			s_AxisHorizontal = -1.0f;
		}
	}

	// ���� Ű �Է��� ���� ���
	else if (s_AxisHorizontal < 0.0f)
	{
		s_AxisHorizontal += CL::Input::s_DeltaTime;

		if (s_AxisHorizontal > 0.0f)
		{
			s_AxisHorizontal = 0.0f;
		}
	}

	// Check Postive Horizontal Input
	// ������ Ű �Է��� �ִ� ���
	if (DLLInput::InputKey(static_cast<unsigned int>(KeyCode::KEY_D)))
	{
		// Horizontal �� ����
		s_AxisHorizontal += s_DeltaTime;

		// Max �� ����
		if (s_AxisHorizontal > 1.0f)
		{
			s_AxisHorizontal = 1.0f;
		}
	}

	// ������ Ű �Է��� ���� ���
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
	// ���콺 �������� ������ offset ��ŭ ���ؼ� �������ش�.
	float _delta = CL::Input::s_DeltaTime * s_MouseMoveOffset;

	/// ���콺 ���� �������� �̵��� ���� ���
	/// �ش� �������� MouseY ���� ����(����) �Ѵ�.

	// Check Postive MouseY Input
	// ���� �̵��� ���
	if (s_MouseDeltaY > 0)
	{
		// Vertical �� ����
		s_AxisMouseY += _delta;

		// Max �� ����
		if (s_AxisMouseY > 1.0f)
		{
			s_AxisMouseY = 1.0f;
		}
	}

	// Check Negative MouseY Input
	// �Ʒ��� �̵��� ���
	else if (s_MouseDeltaY < 0)
	{
		// Vertical �� ����
		s_AxisMouseY -= _delta;

		// Min �� ����
		if (s_AxisMouseY < -1.0f)
		{
			s_AxisMouseY = -1.0f;
		}
	}


	/// ���콺 ���� �������� �̵��� ���� ���
	/// ���� AxisMouseY ���� ��ȣ�� Ȯ���ؼ� ���ϰų� ���� ���� 0�� �����ϰ� �Ѵ�.
	else if (s_MouseDeltaY == 0)
	{
		// ���� MouseY �� ����� ���
		if (s_AxisMouseY > 0.0f)
		{
			s_AxisMouseY -= CL::Input::s_DeltaTime;

			if (s_AxisMouseY < 0.0f)
			{
				s_AxisMouseY = 0.0f;
			}
		}

		// ���� MouseY �� ������ ���
		else if (s_AxisMouseY < 0.0f)
		{
			s_AxisMouseY += CL::Input::s_DeltaTime;

			if (s_AxisMouseY > 0.0f)
			{
				s_AxisMouseY = 0.0f;
			}
		}
	}

	/// ���콺 ���� �������� �̵��� ���� ���
	/// �ش� �������� MouseX ���� ����(����) �Ѵ�.

	// Check Postive MouseX Input
	// ��� �̵��� ���
	if (s_MouseDeltaX > 0)
	{
		// MouseX �� ����
		s_AxisMouseX += _delta;

		// Max �� ����
		if (s_AxisMouseX > 1.0f)
		{
			s_AxisMouseX = 1.0f;
		}
	}

	// Check Negative MouseX Input
	// �·� �̵��� ���
	else if (s_MouseDeltaX < 0)
	{
		// Vertical �� ����
		s_AxisMouseX -= _delta;

		// Min �� ����
		if (s_AxisMouseX < -1.0f)
		{
			s_AxisMouseX = -1.0f;
		}
	}


	/// ���콺 ���� �������� �̵��� ���� ���
	/// ���� AxisMouseX ���� ��ȣ�� Ȯ���ؼ� ���ϰų� ���� ���� 0�� �����ϰ� �Ѵ�.
	else if (s_MouseDeltaX == 0)
	{
		// ���� MouseX �� ����� ���
		if (s_AxisMouseX > 0.0f)
		{
			s_AxisMouseX -= CL::Input::s_DeltaTime;

			if (s_AxisMouseX < 0.0f)
			{
				s_AxisMouseX = 0.0f;
			}
		}

		// ���� MouseX �� ������ ���
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

	// �� �� ����ó��
	assert(false);
	return 0.0f;
}

float CL::Input::GetAxisRaw(const std::string axisName)
{

	if (axisName == "Vertical")
	{
		// Ű ���� �Է��� �ִ� ���
		if (IsVerticalInputed())
		{
			// ��Ű �Է��̸�
			if (IsUpInputed())
			{
				return 1.0f;
			}

			// �Ʒ�Ű �Է��̸�
			else if (IsDownInputed())
			{
				return -1.0f;
			}
		}

		// �����Է��� ������
		else
		{
			return 0.0f;
		}
	}

	else if (axisName == "Horizontal")
	{
		// Ű ���� �Է��� �ִ� ���
		if (IsHorizontalInputed())
		{
			// ������ Ű �Է��̸�
			if (IsRightInputed())
			{
				return 1.0f;
			}

			// ���� Ű �Է��̸�
			else if (IsLeftInputed())
			{
				return -1.0f;
			}
		}

		// ���� �Է��� ������
		else
		{
			return 0.0f;
		}
	}

	else if (axisName == "Mouse X")
	{
		// ���콺 ���� �̵��� �ִ� ���
		if (IsMouseXInputed())
		{
			// ���� �̵��̸�
			if (IsMouseMovedRight())
			{
				return 1.0f;
			}

			// ���� �̵��̸�
			else if (IsMouseMovedLeft())
			{
				return -1.0f;
			}
		}

		// �����̵��� ������
		else
		{
			return 0.0f;
		}
	}

	else if (axisName == "Mouse Y")
	{
		// ���콺 ���� �̵��� �ִ� ���
		if (IsMouseYInputed())
		{
			// ���� �̵��̸�
			if (IsMouseMovedUp())
			{
				return 1.0f;
			}

			// �Ʒ��� �̵��̸�
			else if (IsMouseMovedDown())
			{
				return -1.0f;
			}
		}

		// ���� �̵��� ������
		else
		{
			return 0.0f;
		}
	}

	// �߸��� ���� ���� ���
	CA_TRACE("Wrong Axis Name Inputed");

	return 999;
}
