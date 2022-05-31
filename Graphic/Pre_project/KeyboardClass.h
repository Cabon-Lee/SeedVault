#pragma once
#include <queue>

class KeyboardEvent;

class KeyboardClass
{
public:
	KeyboardClass();
	~KeyboardClass();

	bool KeyIsPressed(const unsigned char keycode);			// 연속적 입력받을시
	bool KeyIsPressedFirst(const unsigned char keycode);	// 처음 입력받았을시
	bool KeyIsReleased(const unsigned char keycode);		// 입력을 뗄시

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
	bool keyStates[256];			//현재 키입력 상태
	bool keyStatesBefore[256];		//방금 전의 키입력 상태
	std::queue<KeyboardEvent*> keyBuffer;
	std::queue<unsigned char> charBuffer;
};