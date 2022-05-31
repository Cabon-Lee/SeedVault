#include "pch.h"
#include "ResourcesPathDef.h"

#include "CLMaterialTest.h"

#include "CameraController.h"
#include "MuzzleFlash.h"
#include "HitPoint.h"


CLMaterialTest::CLMaterialTest()
	: m_TestObject(nullptr)
	, m_pMainCamera(nullptr)
	, m_pDirectionalLight(nullptr)
{

}

CLMaterialTest::~CLMaterialTest()
{

}


void CLMaterialTest::Initialize()
{
	/// Directional Light

	/*
	{
		const int n = 5;

		float width = 50.0f;
		float height = 50.0f;
		float depth = 50.0f;

		float x = -0.5f * width;
		float y = -0.5f * height;
		float z = -0.5f * depth;
		float dx = width / (n - 1);
		float dy = height / (n - 1);
		float dz = depth / (n - 1);
		for (int k = 0; k < n; ++k)
		{
			for (int i = 0; i < n; ++i)
			{
				for (int j = 0; j < n; ++j)
				{

					GameObject* _newObj = DLLEngine::CreateObject(this);
					_newObj->SetObjectName("TestObject1");
					MeshFilter* _newMesh = new MeshFilter();
					_newMesh->SetMesh("box.bin");
					_newObj->AddComponent<MeshFilter>(_newMesh);
					MeshRenderer* _newRenderer = new MeshRenderer;
					_newObj->AddComponent<MeshRenderer>(_newRenderer);

					auto* _nowTransform = _newObj->GetComponent<Transform>();

					DirectX::XMMATRIX _nowWorld = DirectX::XMMATRIX(
						0.001f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.001f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.001f, 0.0f,
						x + j * dx, y + i * dy, z + k * dz, 1.0f);

					_nowTransform->SetPosition(DirectX::SimpleMath::Vector3(x + j * dx, y + i * dy, z + k * dz));

				}
			}
		}
	}
	*/


	GameObject* _newObj = DLLEngine::CreateObject(this);
	_newObj->SetObjectName("TestObject1");

	MeshFilter* _newMesh = new MeshFilter();
	_newObj->AddComponent<MeshFilter>(_newMesh);
	_newObj->AddComponent<MeshRenderer>(new MeshRenderer);
	/*
	
	//_newObj->AddComponent<MeshFilter>(new MeshFilter);
	//_newObj->AddComponent<MeshRenderer>(new MeshRenderer);
	_newObj->AddComponent<Light>(new Light{});

	ParticleSpawner* _newSpawner = new ParticleSpawner{};
	_newSpawner->AddSpriteName("RifleFront10.png");
	_newObj->AddComponent<ParticleSpawner>(_newSpawner);
	m_MuzzleFlash = new MuzzleFlash;
	_newObj->AddComponent<MuzzleFlash>(m_MuzzleFlash);
	

	*/
	GameObject* _hitPoint = DLLEngine::CreateObject(this);
	_hitPoint->SetObjectName("HitPoint");
	ParticleSpawner* _newSpawner2 = new ParticleSpawner{};
	_newSpawner2->AddSpriteName("dia.png");
	_hitPoint->AddComponent<ParticleSpawner>(_newSpawner2);
	m_HitPoint = new class HitPoint();
	_hitPoint->AddComponent<HitPoint>(m_HitPoint);


	/*
	Animation* _newAnim = new Animation();
	_newAnim->m_AnimationClipName = "Rifle_Aim_mixamo.com.001.anim";
	_newObj->AddComponent< Animation>(_newAnim);

	_newAnim->SetTagetBone(5);
	_newObj->GetTransform()->SetParentTM(_newAnim->GetTargetBoneTM());

	DirectX::SimpleMath::Matrix _local;
	_local = DirectX::SimpleMath::Matrix::CreateTranslation(10, 10, 10);
	_newObj->GetTransform()->SetLocalTM(_local);
	*/







	/*
	GameObject* _newObj = DLLEngine::CreateObject(this);
	//_newObj->GetTransform()->SetScale({0.001f, 0.001f ,0.001f });

	_newObj->SetObjectName("TestObject1");
	MeshFilter* _newMesh = new MeshFilter();
	_newMesh->SetMesh("BG_GR_BD_Ctrl_03.bin");
	_newObj->AddComponent<MeshFilter>(_newMesh);
	_newObj->AddComponent<MeshRenderer>(new MeshRenderer);

	GameObject* _newObj2 = DLLEngine::CreateObject(this);
	//_newObj2->GetTransform()->SetScale({ 0.02f, 0.02f ,0.02f });
	//_newObj2->GetTransform()->SetPosition({ 0.0f, 1.0f ,0.0f });
	_newObj2->SetObjectName("TestObject1");
	MeshFilter* _newMesh2 = new MeshFilter();
	_newMesh2->SetMesh("BG_GR_BD_Ctrl_03.bin");
	_newObj2->AddComponent<MeshFilter>(_newMesh2);
	MeshRenderer* _newRenderer = new MeshRenderer;
	//_newRenderer->SetTransparent(true);

	_newObj2->AddComponent<MeshRenderer>(_newRenderer);
	*/

	/// 테스트 오브젝트1
	//m_TestObject = DLLEngine::CreateObject(this);
	//m_TestObject->SetObjectName("TestObject1");
	//MeshFilter* _newMesh = new MeshFilter();
	//_newMesh->SetMesh("Dancing.bin");
	//m_TestObject->AddComponent<MeshFilter>(_newMesh);
	//m_TestObject->AddComponent<MeshRenderer>(new MeshRenderer);
	//m_TestObject->AddComponent<NaviMeshController>(new NaviMeshController);
	//
	//Animation* _newAnim = new Animation();
	//_newAnim->m_AnimationClipName = "Dancing_mixamo.com.anim";
	//m_TestObject->AddComponent< Animation>(_newAnim);

	/*
	GameObject* _temp = DLLEngine::CreateObject(this);
	MeshFilter* _newMesh2 = new MeshFilter();
	_newMesh2->SetMesh("box.bin");
	_temp->AddComponent<MeshFilter>(_newMesh2);
	_temp->AddComponent<MeshRenderer>(new MeshRenderer);
	*/



	//Animator* _playerAnimator = new Animator();
	//const std::string Movement = "Movement";
	//const std::string Stand_Idle = "Stand_Idle";
	//
	//_playerAnimator->AddAnimLayer(Movement);
	//
	//_playerAnimator->GetAnimLayer(Movement)->m_MaskingType = eMASKING_TYPE::NONE;
	//
	//// Stand
	//_playerAnimator->GetAnimLayer(Movement)->AddStateMap(Stand_Idle, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_IDLE);
	//m_TestObject->AddComponent<Animator>(_playerAnimator);


	/// 테스트 오브젝트2
	//m_TestObject2 = DLLEngine::CreateObject(this);
	//m_TestObject2->SetObjectName("TestObject2");
	//m_TestObject2->AddComponent<MeshFilter>(new MeshFilter);
	//m_TestObject2->GetComponent<MeshFilter>()->SetMesh("Sphere.bin");
	//m_TestObject2->AddComponent<MeshRenderer>(new MeshRenderer);

	///// 테스트 오브젝트3
	//m_TestObject3 = DLLEngine::CreateObject(this);
	//m_TestObject3->SetObjectName("TestObject3");
	//m_TestObject3->AddComponent<MeshFilter>(new MeshFilter);
	//m_TestObject3->GetComponent<MeshFilter>()->SetMesh("Sphere.bin");
	//m_TestObject3->AddComponent<MeshRenderer>(new MeshRenderer);
	//
	///// 테스트 오브젝트4
	//m_TestObject4 = DLLEngine::CreateObject(this);
	//m_TestObject4->SetObjectName("TestObject4");
	//m_TestObject4->AddComponent<MeshFilter>(new MeshFilter);
	//m_TestObject4->GetComponent<MeshFilter>()->SetMesh("Sphere.bin");
	//m_TestObject4->AddComponent<MeshRenderer>(new MeshRenderer);
	//
	///// 테스트 오브젝트5
	//m_TestObject5 = DLLEngine::CreateObject(this);
	//m_TestObject5->SetObjectName("TestObject5");
	//m_TestObject5->AddComponent<MeshFilter>(new MeshFilter);
	//m_TestObject5->GetComponent<MeshFilter>()->SetMesh("Sphere.bin");
	//m_TestObject5->AddComponent<MeshRenderer>(new MeshRenderer);
	//
	//m_ReflectionProbe = DLLEngine::CreateObject(this);
	//m_ReflectionProbe->SetObjectName("ReflectionProbe");
	//m_ReflectionProbe->AddComponent<ReflectionProbeCamera>(new ReflectionProbeCamera);
	//
	//m_ReflectionProbe2 = DLLEngine::CreateObject(this);
	//m_ReflectionProbe2->SetObjectName("ReflectionProbe2");
	//m_ReflectionProbe2->AddComponent<ReflectionProbeCamera>(new ReflectionProbeCamera);

	//m_ReflectionProbe3 = DLLEngine::CreateObject(this);
	//m_ReflectionProbe3->SetObjectName("ReflectionProbe2");
	//m_ReflectionProbe3->AddComponent<ReflectionProbeCamera>(new ReflectionProbeCamera);


	/// 메인 카메라 오브젝트
	m_pMainCamera = DLLEngine::CreateObject(this);
	m_pMainCamera->SetObjectName("MainCamera");
	m_pMainCamera->m_Transform->SetPosition({ 0.0f, 0.0f, -5.0f });

	Camera* _newCamera = new Camera("testCam");
	_newCamera->SetDrawSkyBox(true);
	_newCamera->SetSkyBoxTextureName("satara_night.png");
	m_pMainCamera->AddComponent<Camera>(_newCamera);
	DLLEngine::SetNowCamera("testCam");

	CameraController* _CameraController = new CameraController();
	m_pMainCamera->AddComponent<CameraController>(_CameraController);



}

void CLMaterialTest::Update(float dTime)
{

	if (DLLInput::InputKeyUp(static_cast<int>(0x01)) == true)
	{
		//m_MuzzleFlash->Fire();
		m_HitPoint->GetHitPoint();
	}

}

void CLMaterialTest::Reset()
{

}

