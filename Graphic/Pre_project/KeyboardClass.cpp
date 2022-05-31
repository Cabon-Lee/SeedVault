#include "pch.h"
#include "KeyboardClass.h"
#include "KeyboardEvent.h"

KeyboardClass::KeyboardClass()
{	
	for (int i = 0; i < 256; i++)
		this->keyStates[i] = false; 
		
	for (int i = 0; i < 256; i++)
		this->keyStatesBefore[i] = false; 
}

KeyboardClass::~KeyboardClass()
{

}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}

bool KeyboardClass::KeyIsPressedFirst(const unsigned char keycode)
{
	if (keyStatesBefore[keycode] == false && keyStates[keycode] == true)
	{
		return true;
	}
	return false;
}

bool KeyboardClass::KeyIsReleased(const unsigned char keycode)
{
	if (keyStates[keycode] == false && keyStatesBefore[keycode] == true)
	{
		keyStatesBefore[keycode] = keyStates[keycode];
		return true;
	}
	return false;
}

bool KeyboardClass::KeyBufferIsEmpty()
{
	return this->keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty()
{
	return this->charBuffer.empty();
}

KeyboardEvent* KeyboardClass::ReadKey()
{
	if (this->keyBuffer.empty()) //If no keys to be read?
	{
		KeyboardEvent* e = new KeyboardEvent();
		return e; //return empty keyboard event
	}
	else
	{
		KeyboardEvent* e = keyBuffer.front(); //Get first Keyboard Event from queue
		this->keyBuffer.pop(); //Remove first item from queue
		return e; //Returns keyboard event
	}
}

unsigned char KeyboardClass::ReadChar()
{
	if (this->charBuffer.empty()) //If no keys to be read?
	{
		return 0u; //return 0 (NULL char)
	}
	else
	{
		unsigned char e = this->charBuffer.front(); //Get first char from queue
		this->charBuffer.pop(); //Remove first char from queue
		return e; //Returns char
	}
}

void KeyboardClass::OnKeyPressed(const unsigned char key)
{
	this->keyStatesBefore[key] = this->keyStates[key];
	this->keyStates[key] = true;
	KeyboardEvent* e = new KeyboardEvent(KeyboardEvent::EventType::Press, key);
	this->keyBuffer.push(e);
}

void KeyboardClass::OnKeyReleased(const unsigned char key)
{
	this->keyStatesBefore[key] = this->keyStates[key];
	this->keyStates[key] = false;
	KeyboardEvent* e = new KeyboardEvent(KeyboardEvent::EventType::Release, key);
	this->keyBuffer.push(e);
}

void KeyboardClass::SetCurrentBeforeKey()
{
	for (int i = 0; i < 256; i++)
		this->keyStatesBefore[i] = this->keyStates[i];
}

void KeyboardClass::OnChar(const unsigned char key)
{
	this->charBuffer.push(key);
}

void KeyboardClass::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

void KeyboardClass::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

void KeyboardClass::EnableAutoRepeatChars()
{
	this->autoRepeatChars = true;
}

void KeyboardClass::DisableAutoRepeatChars()
{
	this->autoRepeatChars = false;
}

bool KeyboardClass::IsKeysAutoRepeat()
{
	return this->autoRepeatKeys;
}

bool KeyboardClass::IsCharsAutoRepeat()
{
	return this->autoRepeatChars;
}