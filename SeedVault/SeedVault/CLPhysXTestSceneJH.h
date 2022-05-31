#pragma once



#include "SceneBase.h"

class PhysicsEngine;

/// PhysX�� �׽�Ʈ�� ��
/// ���� �������� ������Ʈ�� ����°��� �ȵȴ�
/// �̸� ���������͸� �ϴ� ��������

class CLPhysXTestSceneJH final : public SceneBase
{
public:
   CLPhysXTestSceneJH();
   virtual ~CLPhysXTestSceneJH();
   CLPhysXTestSceneJH(const CLPhysXTestSceneJH& other) = default;


   virtual void Initialize() override;
   virtual void Update(float dTime) override;
   virtual void Reset() override;

   void KeyPressEvent();
   //virtual void Render() override;



private:

   void CoverRenderFilter(GameObject* pObject);

   GameObject* m_TestObject0;
   GameObject* m_TestObject1;
   GameObject* m_pTestObject2;
   GameObject* m_pMainCamera;

   std::vector<GameObject*> m_Objects_V;

   // physx test
   //PhysicsEngine* m_pPhysics;
   GameObject* m_pPhysTestSphere;
   std::vector<GameObject*> m_PhysTestObject_V;
};

