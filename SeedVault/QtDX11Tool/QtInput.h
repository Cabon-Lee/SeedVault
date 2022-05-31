#pragma once

/// <summary>
/// �������� �������ִ� Ű�Է� �Ǻ��� ���콺 ��ǥ�� �����Ͽ�
/// Ŭ���̾�Ʈ���� �߰� ������ �ϱ� ���� Ŭ����
/// 
/// ����/���� ������ �Է� ������ Ŭ���̾�Ʈ�� Ű���ο����� �̿��Ѵ�.
/// </summary>

#pragma region UpdateLog
// ������Ʈ �α�
// 2021-11-09 
// static�� �� ������ �����ѵ� �ϴ� ��ɵ��� �ؼ� ����.
// ��, �Ʒ�, ��, �� �Է� üũ�� ���� ������ �Լ��� �߰��ߴ�. IsUpInputed() IsDownInputed() ....
// GetAxisRaw() �Լ� �߰�
// Ű ��(Horizontal / Vertical)�� ���콺 ��(Mouse X / Mouse Y)�� �и��ߴ�.

// 2022-01-06
// virtual keycode F1 ~ F12 �߰�
#pragma endregion UpdateLog

// MSDN keycode list
// https://docs.microsoft.com/ko-kr/windows/win32/inputdev/virtual-key-codes

namespace QT
{
	enum class KeyCode : unsigned long
	{
		CLICK_LEFT = 0x01,
		CLICK_RIGHT = 0x02,

		KEY_LEFT  = 0x25,
		KEY_UP    = 0x26,
		KEY_RIGHT = 0x27,
		KEY_DOWN  = 0x28,

		KEY_F1  = 0x70,
		KEY_F2  = 0x71,
		KEY_F3  = 0x72,
		KEY_F4  = 0x73,
		KEY_F5  = 0x74,
		KEY_F6  = 0x75,
		KEY_F7  = 0x76,
		KEY_F8  = 0x77,
		KEY_F9  = 0x78,
		KEY_F10 = 0x79,
		KEY_F11 = 0x7A,
		KEY_F12 = 0x7b,

		KEY_0 = 0x30,
		KEY_1 = 0x31,
		KEY_2 = 0x32,
		KEY_3 = 0x33,
		KEY_4 = 0x34,
		KEY_5 = 0x35,
		KEY_6 = 0x36,
		KEY_7 = 0x37,
		KEY_8 = 0x38,
		KEY_9 = 0x39,

		KEY_A = 0x41,
		KEY_B = 0x42,
		KEY_C = 0x43,
		KEY_D = 0x44,
		KEY_E = 0x45,
		KEY_F = 0x46,
		KEY_G = 0x47,
		KEY_H = 0x48,
		KEY_I = 0x49,
		KEY_J = 0x4A,
		KEY_K = 0x4B,
		KEY_L = 0x4C,
		KEY_M = 0x4D,
		KEY_N = 0x4E,
		KEY_O = 0x4F,
		KEY_P = 0x50,
		KEY_Q = 0x51,
		KEY_R = 0x52,
		KEY_S = 0x53,
		KEY_T = 0x54,
		KEY_U = 0x55,
		KEY_V = 0x56,
		KEY_W = 0x57,
		KEY_X = 0x58,
		KEY_Y = 0x59,
		KEY_Z = 0x5A,

		KEY_LSHIFT = 0xA0,
		KEY_LCONTROL = 0xA2,
		KEY_SPACE = 0x20,

		KEY_ESCAPE = 0x1B,
	};

	class Input final
	{
	public:
		Input();
		~Input();

		void Update();
		void UpdateMousePosition();					// ���콺 pos ���Ű� �̵���(delta) ���
		void UpdateAxisVerticalAndHorizontal();		// Key   Axis ������Ʈ
		void UpdateAxisMouseXAndMouseY();			// Mouse Axis ������Ʈ
		

		/// Ű �Է� Axis ����
		static bool IsVerticalInputed();			// ���� Ű �Է��� �־����� Ȯ��
		static bool IsHorizontalInputed();			// ���� Ű �Է��� �־����� Ȯ��
		static bool IsUpInputed();					// ���� Ű �Է��� �־����� Ȯ��
		static bool IsDownInputed();				// �Ʒ��� Ű �Է��� �־����� Ȯ��
		static bool IsLeftInputed();				// ���� Ű �Է°� ���콺 ���� �̵��� �־����� Ȯ��
		static bool IsRightInputed();				// ������ Ű �Է°� ���콺 ���� �̵��� �־����� Ȯ��

		/// Mouse �̵� Axis ���� 
		static bool IsMouseYInputed();				// ���콺 ���� �̵��� �־����� Ȯ��
		static bool IsMouseXInputed();				// ���콺 ���� �̵��� �־����� Ȯ��
		static bool IsMouseMovedUp();				// ���콺 ���� �̵��� �־����� Ȯ��
		static bool IsMouseMovedDown();				// ���콺 �Ʒ��� �̵��� �־����� Ȯ��
		static bool IsMouseMovedLeft();				// ���콺 ���� �̵��� �־����� Ȯ��
		static bool IsMouseMovedRight();			// ���콺 ���� �̵��� �־����� Ȯ��

		// axisName�� �ش��ϴ� ���� ������ ���
		static float GetAxis(const std::string axisName);
		static float GetAxisRaw(const std::string axisName);

		// Mouse Position
		static POINT s_MousePrevPos;
		static POINT s_MouseCurrentPos;

		// Axis Input
		static float s_AxisHorizontal;		// KeyInput Horizontal
		static float s_AxisVertical;		// KeyInput Vertical
		static float s_AxisMouseX;			// Mouse Move X
		static float s_AxisMouseY;			// Mouse Move Y

	//private:
		// Mouse Pos �̵� �Ǻ�
		static LONG s_MouseDeltaX;
		static LONG s_MouseDeltaY;

		static float s_MouseMoveOffset;	// ���콺 �̵��� Ű���� ������ ���� üũ�� ���
										// ���콺 �̵����� ���ؼ� Axis ���� ���� �� ����ġ�� ���ؼ� 
										// ������ �ӵ��� �����̰� ������ش�.
	};
}