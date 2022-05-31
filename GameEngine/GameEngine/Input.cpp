#include "pch.h"
#include "Input.h"
#include "CATrace.h"


KeyState::KeyState()
   : curr(false), prev(false)
{
}

KeyState::~KeyState()
{
}

void Input::Initialize(HWND hWnd)
{
   m_CurrentHWND = hWnd;
}

void Input::KeyUpdate()
{
   for (int i = 0; i < 255; i++)
   {
      m_KeyStates[i].prev = m_KeyStates[i].curr;
      m_KeyStates[i].curr = HIWORD(GetAsyncKeyState(i));
   }
}

bool Input::GetKey(int vk)
{
   return m_KeyStates[vk].curr;
}

bool Input::GetKeyDown(int vk)
{
   return !m_KeyStates[vk].prev && m_KeyStates[vk].curr;
}

bool Input::GetKeyUp(int vk)
{
   return m_KeyStates[vk].prev && !m_KeyStates[vk].curr;
}

POINT Input::GetMouseClientPos()
{
   //m_MouseClientPos = m_MouseScreenPos;
   //ScreenToClient(m_CurrentHWND, &m_MouseClientPos);

   return m_MouseClientPos;
}

POINT Input::GetMouseScreenPos()
{
   return m_MouseScreenPos;
}

POINT Input::GetPrevMouseClientPos()
{
   return m_PrevMouseClientPos;
}

void Input::MouseUpdate()
{
   //if (m_MouseClientPos.x < 0.0f || m_MouseClientPos.y < 0.0f)
   //{
   //   Trace("out of boundary");
   //   return;
   //}


   // 현재 마우스 위치 값을 따로 저장 -> 과거 마우스 위치 값
   m_PrevMouseClientPos = m_MouseClientPos;

   GetCursorPos(&m_MouseClientPos);
   ScreenToClient(m_CurrentHWND, &m_MouseClientPos);
}
