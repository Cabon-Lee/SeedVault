#pragma once
#include <queue>

class KeyboardEvent;

class KeyboardClass
{
public:
	KeyboardClass();
	~KeyboardClass();

	bool KeyIsPressed(const unsigned char keycode);			// ������ �Է¹�����
	bool KeyIsPressedFirst(const unsigned char keycode);	// ó�� �Է¹޾�����
	bool KeyIsReleased(const unsigned char keycode);		// �Է��� ����

	bool KeyBufferIsEmpty();
	bool CharBufferIsEmpty();
	KeyboardEvent* ReadKey();
	unsigned char ReadChar();
	void OnKeyPressed(const unsigned char key);
	void OnKeyReleased(const unsigned char key);

	void SetCurrentBeforeKey();

	void OnChar(const unsigned char key);
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();
private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	bool keyStates[256];			//���� Ű�Է� ����
	bool keyStatesBefore[256];		//��� ���� Ű�Է� ����
	std::queue<KeyboardEvent*> keyBuffer;
	std::queue<unsigned char> charBuffer;
};