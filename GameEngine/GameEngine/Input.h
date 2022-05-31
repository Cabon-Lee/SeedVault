#pragma once

/// 키인풋에 관련된 클래스
/// 엔진에서 업데이트 해주고 마우스 위치값과 bool값을 반환

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

public: // 숏컷으로 일단 빼둬서 정보 쓰자
   POINT m_MouseClientPos;
   POINT m_MouseScreenPos;
   POINT m_PrevMouseClientPos;

private:
   HWND m_CurrentHWND;
   KeyState m_KeyStates[256];

};

