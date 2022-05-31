#pragma once

/// 클라이언트에서 관리하는 Input
#include "QtInput.h"

/// 키 세팅 매핑
namespace QT
{
	namespace KeyMap
	{
		/// <summary>
		/// 게임에서 사용할 키 맵핑
		/// 여러 군데에서 여기 정의된 값을 사용하므로 static으로 선언했다.
		/// 게다가 static이 아니면 여러 파일에서 include 되면서 
		/// 여러번 정의 되었다는 오류도 함께 뜨게된다.
		/// 
		/// Setting 객체 하나 생성하고 업데이트, 관리 하는것도 생각중이긴한데 일단 되는선에서 넘어가자...
		/// </summary>
		static unsigned long FORWARD = static_cast<unsigned long>(KeyCode::KEY_W);
		static unsigned long BACKWARDS = static_cast<unsigned long>(KeyCode::KEY_S);
		static unsigned long LEFT = static_cast<unsigned long>(KeyCode::KEY_A);
		static unsigned long RIGHT = static_cast<unsigned long>(KeyCode::KEY_D);
		static unsigned long SPRINT = static_cast<unsigned long>(KeyCode::KEY_LSHIFT);
		static unsigned long CROUCH = static_cast<unsigned long>(KeyCode::KEY_C);
		static unsigned long INTERATION = static_cast<unsigned long>(KeyCode::KEY_F);
		static unsigned long HANDSIGN = static_cast<unsigned long>(KeyCode::KEY_C);
		static unsigned long ATTACK = static_cast<unsigned long>(KeyCode::CLICK_LEFT);
		static unsigned long AIM = static_cast<unsigned long>(KeyCode::CLICK_RIGHT);

		static unsigned long DEBUG = static_cast<unsigned long>(KeyCode::KEY_F1);

		static float MouseSensitivity = 0.15f;
	}

	namespace Window
	{
	}
}

