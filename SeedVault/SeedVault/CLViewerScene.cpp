#include "pch.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "OrbitCamera.h"
#include "CLViewerScene.h"

#include "JsonLoader.h"

CLViewerScene::CLViewerScene() 
	: m_TestObject(nullptr)
	, m_pMainCamera(nullptr)
	, _bParent(false)
	, m_pDirectionalLight(nullptr)
{

}

CLViewerScene::~CLViewerScene()
{
}


void CLViewerScene::Initialize()
{

	/// Directional Light
	m_pDirectionalLight = DLLEngine::CreateObject(this);
	m_pDirectionalLight->SetObjectName("Directional Light");
	Light* _nowLight = new Light();
	_nowLight->m_CastShadow = true;
	//_nowLight->m_IsTextured = true;
	m_pDirectionalLight->AddComponent<Light>(_nowLight);

	m_PointLight = DLLEngine::CreateObject(this);
	m_PointLight->SetObjectName("point Light");
	Light* _newLight = new Light();
	_newLight->m_eLightType = eLIGHT_TYPE::SPOT;
	_newLight->SetColor(1.0f, 0.3f, 0.3f);
	_newLight->m_FalloffStart = 0.5f;
	_newLight->m_FalloffEnd = 2.2f;
	_newLight->m_SpotPower = 8.0f;
	_newLight->m_CastShadow = true;
	_newLight->m_IsTextured = true;
	_newLight->m_LightTextureName = "FlashLight_Vignette.png";
	m_PointLight->AddComponent<Light>(_newLight);
	



	/// 테스트 오브젝트1
	m_TestObject = DLLEngine::CreateObject(this);
	m_TestObject->SetObjectName("TestObject1");

	MeshFilter* _newMesh = new MeshFilter();
	_newMesh->SetMesh("Rifle Aiming Idle.bin");
	m_TestObject->AddComponent<MeshFilter>(_newMesh);
	Animation* _ani = new Animation();
	_ani->m_AnimationClipName = "Rifle Aiming Idle_mixamo.com.anim";
	m_TestObject->AddComponent<Animation>(_ani);
	
	PlayerController* _nowPC = new PlayerController();
	//m_TestObject->AddComponent(_nowPC);

	MaterialComponent* _newMat = new MaterialComponent();
	_newMat->SetVertexShaderName("vs_stnd.cso");
	_newMat->SetPixelShaderName("ps_stnd.cso");
	m_TestObject->AddComponent<MaterialComponent>(_newMat);
	m_TestObject->AddComponent<MeshRenderer>(new MeshRenderer);


	/// 테스트 오브젝트1
	m_pTestObject2 = DLLEngine::CreateObject(this);
	m_pTestObject2->SetObjectName("TestObject1");

	MeshFilter* _newMesh2 = new MeshFilter();
	m_pTestObject2->AddComponent<MeshFilter>(_newMesh2);
	m_pTestObject2->AddComponent<MeshRenderer>(new MeshRenderer);


	////////////////////////////////////////////////////////////////////

   /// 메인 카메라 오브젝트
	m_pMainCamera = DLLEngine::CreateObject(this);
	m_pMainCamera->SetObjectName("MainCamera");
	m_pMainCamera->m_Transform->SetPosition({ 0.0f, 0.0f, -30.0f });

	Camera* _newCamera = new Camera("testCam");

	//_newCamera->SetDrawSkyBox(true);
	_newCamera->SetSkyBoxTextureName("SkyBox.dds");

	m_pMainCamera->AddComponent<Camera>(_newCamera);
	DLLEngine::SetNowCamera("testCam");

	CameraController* _CameraController = new CameraController();
	m_pMainCamera->AddComponent<CameraController>(_CameraController);
	
	//m_GameObject_V.push_back(m_pDirectionalLight);
	//m_GameObject_V.push_back(m_PointLight);
	//m_GameObject_V.push_back(m_TestObject);
	//m_GameObject_V.push_back(m_pTestObject2);
	//m_GameObject_V.push_back(m_pMainCamera);

}

void CLViewerScene::Update(float dTime)
{
	//Transform* _tf = m_MainCamera->GetComponent<Transform>();
	//CA_TRACE("%f, %f, %f", _tf->GetPosition().x, _tf->GetPosition().y, _tf->GetPosition().z);
	//Vector3 pos = _tf->GetPosition();
	//Trace("%f, %f, %f", pos.x, pos.y, pos.z);

}


void CLViewerScene::Reset()
{
}
