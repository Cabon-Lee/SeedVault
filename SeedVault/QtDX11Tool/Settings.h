#pragma once

/// Ŭ���̾�Ʈ���� �����ϴ� Input
#include "QtInput.h"

/// Ű ���� ����
namespace QT
{
	namespace KeyMap
	{
		/// <summary>
		/// ���ӿ��� ����� Ű ����
		/// ���� �������� ���� ���ǵ� ���� ����ϹǷ� static���� �����ߴ�.
		/// �Դٰ� static�� �ƴϸ� ���� ���Ͽ��� include �Ǹ鼭 
		/// ������ ���� �Ǿ��ٴ� ������ �Բ� �߰Եȴ�.
		/// 
		/// Setting ��ü �ϳ� �����ϰ� ������Ʈ, ���� �ϴ°͵� �������̱��ѵ� �ϴ� �Ǵ¼����� �Ѿ��...
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

