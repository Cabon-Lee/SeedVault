#pragma once
#include "SceneBase.h"

class ObjectBase;

class TestScene1 : public SceneBase
{
public:
   TestScene1();
   virtual ~TestScene1();

   virtual void Initialize() override;
   virtual void Update(float dTime) override;

   virtual void Reset() override;

private:
   //ObjectBase* m_pTestObject;

   // 요환이랑 얘기해봐야할것
   // 클라이언트에서 scene마다 objectManager 접근할지
   // IScene제공이 아닌 SceneBase를 넘겨주는게 나을까?

   // 뭐 어떻게해야하지
};

