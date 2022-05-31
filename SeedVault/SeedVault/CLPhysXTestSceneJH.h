#pragma once



#include "SceneBase.h"

class PhysicsEngine;

/// PhysX를 테스트할 씬
/// 현재 동적으로 오브젝트를 만드는것은 안된다
/// 미리 만들어놓은것만 일단 시행하자

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

