#pragma once

/// Ű��ǲ�� ���õ� Ŭ����
/// �������� ������Ʈ ���ְ� ���콺 ��ġ���� bool���� ��ȯ

class KeyState
{
public:
   KeyState();
   ~KeyState();

   bool curr;
   bool prev;
};


class Input
{
public:
   void Initialize(HWND hWnd);
   void KeyUpdate();
   bool GetKey(int vk);
   bool GetKeyDown(int vk);
   bool GetKeyUp(int vk);

   POINT GetMouseClientPos();
   POINT GetMouseScreenPos();
   POINT GetPrevMouseClientPos();

   void MouseUpdate();

public: // �������� �ϴ� ���ּ� ���� ����
   POINT m_MouseClientPos;
   POINT m_MouseScreenPos;
   POINT m_PrevMouseClientPos;

private:
   HWND m_CurrentHWND;
   KeyState m_KeyStates[256];

};

