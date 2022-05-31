#pragma once

/// <summary>
/// 엔진에서 제공해주는 키입력 판별과 마우스 좌표를 가공하여
/// 클라이언트에서 추가 조작을 하기 위한 클래스
/// 
/// 수직/수평 방향의 입력 감지나 클라이언트이 키맵핑에서도 이용한다.
/// </summary>

#pragma region UpdateLog
// 업데이트 로그
// 2021-11-09 
// static이 좀 많은거 같긴한데 일단 기능동작 해서 보자.
// 위, 아래, 좌, 우 입력 체크에 대한 각각의 함수를 추가했다. IsUpInputed() IsDownInputed() ....
// GetAxisRaw() 함수 추가
// 키 축(Horizontal / Vertical)과 마우스 축(Mouse X / Mouse Y)를 분리했다.

// 2022-01-06
// virtual keycode F1 ~ F12 추가
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
		void UpdateMousePosition();					// 마우스 pos 갱신과 이동량(delta) 계산
		void UpdateAxisVerticalAndHorizontal();		// Key   Axis 업데이트
		void UpdateAxisMouseXAndMouseY();			// Mouse Axis 업데이트
		

		/// 키 입력 Axis 관련
		static bool IsVerticalInputed();			// 수직 키 입력이 있었는지 확인
		static bool IsHorizontalInputed();			// 수평 키 입력이 있었는지 확인
		static bool IsUpInputed();					// 위쪽 키 입력이 있었는지 확인
		static bool IsDownInputed();				// 아래쪽 키 입력이 있었는지 확인
		static bool IsLeftInputed();				// 왼쪽 키 입력과 마우스 좌측 이동이 있었는지 확인
		static bool IsRightInputed();				// 오른쪽 키 입력과 마우스 우측 이동이 있었는지 확인

		/// Mouse 이동 Axis 관련 
		static bool IsMouseYInputed();				// 마우스 수직 이동이 있었는지 확인
		static bool IsMouseXInputed();				// 마우스 수평 이동이 있었는지 확인
		static bool IsMouseMovedUp();				// 마우스 위측 이동이 있었는지 확인
		static bool IsMouseMovedDown();				// 마우스 아래측 이동이 있었는지 확인
		static bool IsMouseMovedLeft();				// 마우스 좌측 이동이 있었는지 확인
		static bool IsMouseMovedRight();			// 마우스 우측 이동이 있었는지 확인

		// axisName에 해당하는 축의 데이터 얻기
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
		// Mouse Pos 이동 판별
		static LONG s_MouseDeltaX;
		static LONG s_MouseDeltaY;

		static float s_MouseMoveOffset;	// 마우스 이동은 키보드 눌림에 비해 체크가 적어서
										// 마우스 이동으로 인해서 Axis 값을 구할 때 보정치를 곱해서 
										// 적당한 속도로 움직이게 만들어준다.
	};
}