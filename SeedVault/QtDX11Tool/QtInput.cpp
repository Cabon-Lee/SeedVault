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
	UpdateAxisVerticalAndHorizontal();	// Ű ��(Horizontal / Vertical) ������Ʈ
	UpdateAxisMouseXAndMouseY();		// ���콺 ��(MouseX / MouseY) ������Ʈ

}

void QT::Input::UpdateMousePosition()
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

void QT::Input::UpdateAxisVerticalAndHorizontal()
{
	float _delta = DLLTime::DeltaTime();	// ���������� Horizontal �̳� Vertical ���� ������ ��
	
	// QT::Input::Update() �� ������ ������ ó���Ǳ� ������
	// QT::Input::Update() �� Component::Update() ���� �� �� ���� ������Ʈ �ȴ�.
	// �Ϻη� ���� ���� ���ؼ� �������� ������
	// AxisLaw() �� ���� �ʹ� ���� -1, +1�� �����ع����� �ǰ� �� �߰� �Ҽ��� �κ��� ����¦�� ���� ��ŵ �Ǿ������..
	// �׷��� �Ϻη� ���� �ִϸ��̼� Ʈ�������� ���ϴ� ��� �۵����� �ʾҴ�..
	// _delta �� ���Ƿ� ������ ������Ʈ �ֱ�� ���߱� ���� ������ ��ǥ �������� 1 / 60 �� �ð���ŭ ���ؼ� ����������.
	_delta *= 1.0f / 60.0f;



	/// ������������ �Է��� ���� ���
	/// �ش� �������� Vertical ���� ����(����) �Ѵ�.
	if (IsVerticalInputed())
	{
		// Check Postive Vertical Input
		// ��Ű �Է��� �ִ� ���
		if (IsUpInputed())
		{
			// Vertical �� ����
			s_AxisVertical += _delta;

			// Max �� ����
			if (s_AxisVertical > 1.0f)
			{
				s_AxisVertical = 1.0f;
			}
		}

		// Check Negative Vertical Input
		// �Ʒ�Ű �Է��� �ִ� ���
		if (IsDownInputed())
		{
			// Vertical �� ����
			s_AxisVertical -= _delta;

			// Min �� ����
			if (s_AxisVertical < -1.0f)
			{
				s_AxisVertical = -1.0f;
			}
		}
	}

	/// �������� �Է��� ���� ���
	/// ���� Vertical ���� ��ȣ�� Ȯ���ؼ� ���ϰų� ���� ���� 0�� �����ϰ� �Ѵ�.
	else
	{
		// ���� Vertical�� ����� ���
		if (s_AxisVertical > 0.0f)
		{
			s_AxisVertical -= DLLTime::DeltaTime();

			if (s_AxisVertical < 0.0f)
			{
				s_AxisVertical = 0.0f;
			}
		}

		// ���� Vertical�� ������ ���
		else if (s_AxisVertical < 0.0f)
		{
			s_AxisVertical += DLLTime::DeltaTime();

			if (s_AxisVertical > 0.0f)
			{
				s_AxisVertical = 0.0f;
			}
		}
	}


	/// ����������� �Է��� ���� ���
	/// �ش� �������� Horizontal ���� ����(����) �Ѵ�.
	if (IsHorizontalInputed())
	{
		// Check Negative Horizontal Input
		// ���� Ű �Է��� �ִ� ���
		if (IsLeftInputed())
		{
			// Horizontal �� ����
			s_AxisHorizontal -= _delta;

			// Min �� ����
			if (s_AxisHorizontal < -1.0f)
			{
				s_AxisHorizontal = -1.0f;
			}
		}

		// Check Postive Horizontal Input
		// ������ Ű �Է��� �ִ� ���
		if (IsRightInputed())
		{
			// Horizontal �� ����
			s_AxisHorizontal += _delta;

			// Max �� ����
			if (s_AxisHorizontal > 1.0f)
			{
				s_AxisHorizontal = 1.0f;
			}
		}
	}

	/// ������� �Է��� ���� ���
	/// ���� Horizontal ���� ��ȣ�� Ȯ���ؼ� ���ϰų� ���� ���� 0�� �����ϰ� �Ѵ�.
	else
	{
		// ���� Horizontal �� ����� ���
		if (s_AxisHorizontal > 0.0f)
		{
			s_AxisHorizontal -= DLLTime::DeltaTime();

			if (s_AxisHorizontal < 0.0f)
			{
				s_AxisHorizontal = 0.0f;
			}
		}

		// ���� Horizontal �� ������ ���
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
	// ���콺 �������� ������ offset ��ŭ ���ؼ� �������ش�.
	float _delta = DLLTime::DeltaTime() * s_MouseMoveOffset;

	/// ���콺 ���� �������� �̵��� ���� ���
	/// �ش� �������� MouseY ���� ����(����) �Ѵ�.
	if (IsMouseYInputed())
	{
		// Check Postive MouseY Input
		// ���� �̵��� ���
		if (IsMouseMovedUp())
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
		if (IsMouseMovedDown())
		{
			// Vertical �� ����
			s_AxisMouseY -= _delta;

			// Min �� ����
			if (s_AxisMouseY < -1.0f)
			{
				s_AxisMouseY = -1.0f;
			}
		}
	}

	/// ���콺 ���� �������� �̵��� ���� ���
	/// ���� AxisMouseY ���� ��ȣ�� Ȯ���ؼ� ���ϰų� ���� ���� 0�� �����ϰ� �Ѵ�.
	else
	{
		// ���� MouseY �� ����� ���
		if (s_AxisMouseY > 0.0f)
		{
			s_AxisMouseY -= DLLTime::DeltaTime();

			if (s_AxisMouseY < 0.0f)
			{
				s_AxisMouseY = 0.0f;
			}
		}

		// ���� MouseY �� ������ ���
		else if (s_AxisMouseY < 0.0f)
		{
			s_AxisMouseY += DLLTime::DeltaTime();

			if (s_AxisMouseY > 0.0f)
			{
				s_AxisMouseY = 0.0f;
			}
		}
	}

	/// ���콺 ���� �������� �̵��� ���� ���
	/// �ش� �������� MouseX ���� ����(����) �Ѵ�.
	if (IsMouseXInputed())
	{
		// Check Postive MouseX Input
		// ��� �̵��� ���
		if (IsMouseMovedRight())
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
		if (IsMouseMovedLeft())
		{
			// Vertical �� ����
			s_AxisMouseX -= _delta;

			// Min �� ����
			if (s_AxisMouseX < -1.0f)
			{
				s_AxisMouseX = -1.0f;
			}
		}
	}

	/// ���콺 ���� �������� �̵��� ���� ���
	/// ���� AxisMouseX ���� ��ȣ�� Ȯ���ؼ� ���ϰų� ���� ���� 0�� �����ϰ� �Ѵ�.
	else
	{
		// ���� MouseX �� ����� ���
		if (s_AxisMouseX > 0.0f)
		{
			s_AxisMouseX -= DLLTime::DeltaTime();

			if (s_AxisMouseX < 0.0f)
			{
				s_AxisMouseX = 0.0f;
			}
		}

		// ���� MouseX �� ������ ���
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
	//CA_TRACE("Wrong Axis Name Inputed");

	return 999;
}
