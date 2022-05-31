#include "pch.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "OrbitCamera.h"
//#include "PhysicsEngine.h"

#include "CLPhysXTestSceneJH.h"

CLPhysXTestSceneJH::CLPhysXTestSceneJH() :
   m_TestObject0(nullptr),
   m_TestObject1(nullptr),
   m_pTestObject2(nullptr),
   m_pMainCamera(nullptr),
   //m_pPhysics(nullptr),
   m_pPhysTestSphere(nullptr)
{

}

CLPhysXTestSceneJH::~CLPhysXTestSceneJH()
{
}


void CLPhysXTestSceneJH::Initialize()
{
   m_PhysTestObject_V.clear();
   /*
   /// 테스트 오브젝트0
   m_TestObject0 = DLLEngine::CreateObject(this);
   m_TestObject0->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
   m_TestObject0->SetObjectName("TestObject0");
   CoverRenderFilter(m_TestObject0);

   //////////////////////////////////////////////////////////////////////////

   /// 테스트 오브젝트1
   m_TestObject1 = DLLEngine::CreateObject(this);
   m_TestObject1->m_Transform->SetPosition({ 20.0f, 0.0f, 0.0f });
   m_TestObject1->SetObjectName("TestObject2");
   CoverRenderFilter(m_TestObject1);

   //////////////////////////////////////////////////////////////////////////
   */

   /// 테스트 오브젝트2
   //m_TestObject2 = DLLEngine::CreateObject(this);
   //m_TestObject2->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
   //m_TestObject2->SetObjectName("TestObject3");
   //CoverRenderFilter(m_TestObject2);

   //m_TestObject2->AddComponent<PlayerController>(new PlayerController());

   //////////////////////////////////////////////////////////////////////////

   /// 메인 카메라 오브젝트
   m_pMainCamera = DLLEngine::CreateObject(this);
   m_pMainCamera->m_Transform->SetPosition({ 0.0f, 20.0f, -30.0f });
   m_pMainCamera->SetObjectName("MainCamera");
   //CoverRenderFilter(m_MainCamera);
   // m_TestObject 를 m_MainCamera 의 부모로 설정
   //m_MainCamera->m_Transform->SetParentTM(m_TestObject2);

   m_pMainCamera->AddComponent<Camera>(new Camera("CLTestScene_MainCamera"));
   DLLEngine::SetNowCamera("CLTestScene_MainCamera");
   m_pMainCamera->AddComponent<CameraController>(new CameraController());

   //////////////////////////////////////////////////////////////////////////

   // orbit 카메라
   //OrbitCamera* _OrbitCamera = new OrbitCamera();
   //_OrbitCamera->SetFocus(m_TestObject->m_Transform);
   //m_MainCamera->AddComponent<OrbitCamera>(_OrbitCamera);

   //OrbitCamera* _OrbitCamera = new OrbitCamera();
   //_OrbitCamera->SetFocusTrasform(m_TestObject0->m_Transform);
   //_OrbitCamera->m_Transform = m_MainCamera->m_Transform;
   //m_MainCamera->AddComponent<OrbitCamera>(_OrbitCamera);

   //m_TestObject2->m_Transform->SetParentTM(m_TestObject);

   //m_MainCamera->m_Transform->SetParentTM(m_TestObject2);
   //////////////////////////////////////////////////////////////////////////
   // PVD에는 연결이 된다 -> 물리 공간에서는 업데이트가 따로 된다, 렌더러 공간과 동기화돼서 업데이트 안됨...
   /// phyics
   //m_pPhysics = new PhysicsEngine();
   //m_pPhysics->Initialize(true);


   //m_pPhysics->CreateTestBox(&_trn, 5.0f);

   //
   //for (int i = 0; i < 5; i++)
   //{
   //   GameObject* _testBox = DLLEngine::CreateObject(this);
   //   _testBox->m_Transform->SetPosition({ 40.0f + i * 5.0f , 50.0f + i * 10.0f, -50.0f });
   //   _testBox->m_Transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
   //   string _name = "PhysicsBox";
   //   _testBox->SetObjectName(_name.append(to_string(i)));
   //   CoverRenderFilter(_testBox);

   //   PhysicsActor* _physActor = new PhysicsActor({ 3.0f, 3.0f, 3.0f });
   //   _testBox->AddComponent(_physActor);
   //   m_pPhysics->AddPhysicsActor(_physActor);

   //   m_PhysTestObject_V.push_back(_testBox);
   //}
   //
   //for (int i = 0; i < 5; i++)
   //{
     // GameObject* _testBox = DLLEngine::CreateObject(this);
     // _testBox->m_Transform->SetPosition({ 40.0f + i * 5.0f , 50.0f + i * 10.0f, -50.0f });
     // _testBox->m_Transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
     // string _name = "PhysicsBox";
     // _testBox->SetObjectName(_name.append(to_string(i)));
     // CoverRenderFilter(_testBox);

     // PhysicsActor* _physActor = new PhysicsActor({ 7.0f, 2.0f, 7.0f });
     // _testBox->AddComponent(_physActor);
     // m_pPhysics->AddPhysicsActor(_physActor);

     // m_PhysTestObject_V.push_back(_testBox);
   //}

   m_pPhysTestSphere = DLLEngine::CreateObject(this);
   //_testSphere->m_Transform->SetPosition({ 50.0f , 40.0f + i * 10.0f, -40.0f + i * 10.0f });
   m_pPhysTestSphere->m_Transform->SetPosition({ 0.0f, 5.0f, -30.0f });
   m_pPhysTestSphere->m_Transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
   std::string _name = "physxDynamicSphere";
   m_pPhysTestSphere->SetObjectName(_name.append(to_string(100)));
   CoverRenderFilter(m_pPhysTestSphere);

   PhysicsActor* _physActor = new PhysicsActor(5.0f, RigidType::Dynamic);
   m_pPhysTestSphere->AddComponent(_physActor);
   DLLEngine::AddPhysicsActorToPhysEngine(_physActor);

   //m_PhysTestObject_V.push_back(m_pPhysTestSphere);

   for (int i = 0; i < 10; i++)
   {
      GameObject* _testSphere = DLLEngine::CreateObject(this);
      //_testSphere->m_Transform->SetPosition({ 50.0f , 40.0f + i * 10.0f, -40.0f + i * 10.0f });
      _testSphere->m_Transform->SetPosition({ 0.0f , 5.0f + i * 10.0f, 10.0f + i * 10.0f });
      _testSphere->m_Transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
      std::string _name = "physxDynamic";
      _testSphere->SetObjectName(_name.append(to_string(i)));
      CoverRenderFilter(_testSphere);

      PhysicsActor* _physActor = new PhysicsActor({ 5.0f, 5.0f, 5.0f }, RigidType::Dynamic);
      _testSphere->AddComponent(_physActor);
      DLLEngine::AddPhysicsActorToPhysEngine(_physActor);

      m_PhysTestObject_V.push_back(_testSphere);
   }

   //for (int i = 0; i < 10; i++)
   //{
   //   GameObject* _testSphere = DLLEngine::CreateObject(this);
   //   //_testSphere->m_Transform->SetPosition({ 50.0f , 40.0f + i * 10.0f, -40.0f + i * 10.0f });
   //   _testSphere->m_Transform->SetPosition({ 0.0f , 5.0f + i * 10.0f, 10.0f + i * 10.0f });
   //   _testSphere->m_Transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
   //   string _name = "physxStatic";
   //   _testSphere->SetObjectName(_name.append(to_string(i)));
   //   CoverRenderFilter(_testSphere);

   //   PhysicsActor* _physActor = new PhysicsActor({ 5.0f, 5.0f, 5.0f }, 2);
   //   _testSphere->AddComponent(_physActor);
   //   DLLEngine::AddPhysicsActor(_physActor);

   //   m_PhysTestObject_V.push_back(_testSphere);
   //}


   //for (int i = 0; i < 2; i++)
   //{
     // GameObject* _testSphere = DLLEngine::CreateObject(this);
     // _testSphere->m_Transform->SetPosition({ 50.0f , 40.0f + i * 10.0f, -40.0f + i * 5.0f });
     // _testSphere->m_Transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
     // string _name = "PhysicsSphere";
     // _testSphere->SetObjectName(_name.append(to_string(i)));
     // CoverRenderFilter(_testSphere);

     // PhysicsActor* _physActor = new PhysicsActor(3.0f);
     // _testSphere->AddComponent(_physActor);
     // m_pPhysics->AddPhysicsActor(_physActor);

     // m_PhysTestObject_V.push_back(_testSphere);
   //}

   //for (int i = 0; i < 5; i++)
   //{
     // GameObject* _testSphere = DLLEngine::CreateObject(this);
     // _testSphere->m_Transform->SetPosition({ 50.0f , 40.0f + i * 10.0f, -40.0f + i * 10.0f });
     // _testSphere->m_Transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
     // string _name = "PhysicsSphere";
     // _testSphere->SetObjectName(_name.append(to_string(i)));
     // CoverRenderFilter(_testSphere);

     // PhysicsActor* _physActor = new PhysicsActor({ 5.0f, 5.0f, 5.0f });
     // _testSphere->AddComponent(_physActor);
     // DLLEngine::AddPhysicsActor(_physActor);

     // m_PhysTestObject_V.push_back(_testSphere);
   //}


   /// 테스트 오브젝트1
   //m_pPhysTestBox = DLLEngine::CreateObject(this);
   //m_pPhysTestBox->m_Transform->SetPosition({ 50.0f, 50.0f, -50.0f });
   //m_pPhysTestBox->m_Transform->SetScale(Vector3(1.0f, 1.0f, 1.0f));
   //m_pPhysTestBox->SetObjectName("PhysicsBox");
   //CoverRenderFilter(m_pPhysTestBox);
   ///// physx test
   //PhysicsActor* _physActor = new PhysicsActor(5.0f);
   //m_pPhysTestBox->AddComponent(_physActor);
   //m_pPhysics->AddPhysicsActor(_physActor);



   //m_pPhysics->CreateDynamicObject(m_pPhysTestBox->m_Transform, 1, (Vector3)m_pPhysTestBox->m_Transform->GetRotation());

}

void CLPhysXTestSceneJH::Update(float dTime)
{
   //Trace("%f, %f, %f", m_MainCamera->m_Transform->m_WorldTM._41, m_MainCamera->m_Transform->m_WorldTM._42, m_MainCamera->m_Transform->m_WorldTM._43);

   static float _angle = 1.0f;
   //m_MainCamera->GetComponent<Camera>()->LookAtTarget(m_TestObject2->m_Transform->GetWorldPosition());

   //m_TestObject2->GetTransform()->RotateY(_angle*dTime);
   std::vector<CollisionHit> _colHit;
   //DLLEngine::CheckCollision(&_colHit, m_pPhysTestSphere->GetComponent<PhysicsActor>(), m_pPhysTestSphere->m_Transform);

   /* Vector3 vec = -(m_MainCamera->m_Transform->m_RotationTM.Forward());
    if (DLLEngine::CheckRaycastHit(m_MainCamera->m_Transform->m_Position, vec, 1000.0f).actor != nullptr)
    {
       CA_TRACE("Ray hits object!, %s", DLLEngine::CheckRaycastHit(m_MainCamera->m_Transform->m_Position, vec, 1000.0f).actor->GetMyObject()->GetObjectName().c_str());
    }
    */
   KeyPressEvent();

}


void CLPhysXTestSceneJH::Reset()
{
   //for (GameObject* _object : m_PhysTestObject_V)
   //{
   //   delete _object;
   //   _object = nullptr;
   //}
   ////m_PhysTestObject_V.clear();
}

void CLPhysXTestSceneJH::KeyPressEvent()
{
   if (DLLInput::InputKeyUp(VK_SPACE))
   {
      for (int i = 0; i < m_PhysTestObject_V.size(); i++)
      {
         m_PhysTestObject_V[i]->m_Transform->SetPosition({ 30.0f + i * 10.0f, 40.0f + i * 10.0f, -30.0f + i * 10.0f });

         DLLEngine::SetPhysicsTransform(m_PhysTestObject_V[i]->m_Transform);
      }
   }

   static float pos = 0.0f;
   Transform _tempT;
   _tempT.SetPosition({ 0.0f, 0.0f, pos });

}

void CLPhysXTestSceneJH::CoverRenderFilter(GameObject* pObject)
{
   pObject->AddComponent<MeshFilter>(new MeshFilter);

   MaterialComponent* _newMat = new MaterialComponent();
   _newMat->SetVertexShaderName("vs_stnd.cso");
   _newMat->SetPixelShaderName("ps_stnd.cso");
   pObject->AddComponent<MaterialComponent>(_newMat);

   pObject->AddComponent<MeshRenderer>(new MeshRenderer);
}
