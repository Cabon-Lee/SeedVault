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

   // ��ȯ�̶� ����غ����Ұ�
   // Ŭ���̾�Ʈ���� scene���� objectManager ��������
   // IScene������ �ƴ� SceneBase�� �Ѱ��ִ°� ������?

   // �� ����ؾ�����
};

