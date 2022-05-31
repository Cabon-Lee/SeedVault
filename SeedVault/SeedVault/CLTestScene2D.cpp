#include "pch.h"
#include "CLTestScene2D.h"
#include "CameraController.h"
#include "DirectXColors.h"


CLTestScene2D::CLTestScene2D()
	: m_Sprite(nullptr), m_Button(nullptr), m_D2DText(nullptr), m_TestObject(nullptr), m_pMainCamera(nullptr)
{
}

CLTestScene2D::~CLTestScene2D()
{
}

void CLTestScene2D::Initialize()
{

	/// Directional Light
	m_pDirectionalLight = DLLEngine::CreateObject(this);
	m_pDirectionalLight->SetObjectName("Directional Light");
	Light* _nowLight = new Light();
	m_pDirectionalLight->AddComponent<Light>(_nowLight);

	//GameObject* _newBill = DLLEngine::CreateObject(this);
	//_newBill->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
	//Billboard* _billboard = new Billboard();
	//_billboard->m_SpriteName_V.push_back("RifleFront10.png");
	//_billboard->SeteResourceType(eResourceType::eSingleImage);
	//_billboard->SetRotationType(eRotationType::YAxis);
	//_billboard->SetProportion(0.01f, 0.01f);
	//_billboard->SetPivot(0.5f, 0.5f);
	//_newBill->AddComponent<Billboard>(_billboard);
	//
	//GameObject* _newBill3 = DLLEngine::CreateObject(this);
	//_newBill3->m_Transform->SetPosition({ -3.0f, 0.0f, 0.0f });
	//Billboard* _billboard3 = new Billboard();
	//_billboard3->m_SpriteName_V.push_back("RifleFront10.png");
	//_billboard3->SeteResourceType(eResourceType::eSingleImage);
	//_billboard3->SetRotationType(eRotationType::YAxis);
	//_billboard3->SetProportion(0.01f, 0.01f);
	//_billboard3->SetPivot(0.5f, 0.5f);
	//_newBill3->AddComponent<Billboard>(_billboard3);


	/// 빌보드
	//m_Billboard2 = DLLEngine::CreateObject(this);
	//m_Billboard2->SetObjectName("Billboard2");
	//m_Billboard2->m_Transform->SetPosition({ 3.0f, 0.0f, 0.0f });
	//
	//Billboard* _billboard2 = new Billboard();
	//_billboard2->m_SpriteName_V.push_back("RifleFront10.png");
	//_billboard2->SeteResourceType(eResourceType::eSingleImage);
	//_billboard2->SetRotationType(eRotationType::YAxis);
	//_billboard2->SetProportion(0.01f, 0.01f);
	//_billboard2->SetPivot(0.5f, 0.5f);
	//m_Billboard2->AddComponent<Billboard>(_billboard2);
	//
	//GameObject* _newObj = DLLEngine::CreateObject(this);
	//_newObj->SetObjectName("TestObject1");
	//MeshFilter* _newMesh = new MeshFilter();
	//_newMesh->SetMesh("BG_GR_BD_Ctrl_03.bin");
	//_newObj->AddComponent<MeshFilter>(_newMesh);
	//_newObj->AddComponent<MeshRenderer>(new MeshRenderer);
	//
	//
	//GameObject* _newObj2 = DLLEngine::CreateObject(this);
	//_newObj2->SetObjectName("TestObject1");
	//MeshFilter* _newMesh2 = new MeshFilter();
	//_newMesh2->SetMesh("BG_GR_BD_Ctrl_03.bin");
	//_newObj2->AddComponent<MeshFilter>(_newMesh2);
	//_newObj2->AddComponent<MeshRenderer>(new MeshRenderer);

	

	/*
	/// 빌보드 벡터
	m_Billboard_V.resize(100);
	for (int i = 0; i < m_Billboard_V.size(); ++i )
	{
		m_Billboard_V[i] = DLLEngine::CreateObject(this);
		std::string _name = "Billboard";
		std::string _num = to_string(i + 1);
		_name.append(_num);
		m_Billboard_V[i]->SetObjectName(_name);
	
		int j = 0;
		if (i < 10)
		{ j = 0;}
		else if (i < 20)
		{ j = 2;}
		else if (i < 30)
		{ j = 4;}
		else if (i < 40)
		{ j = 6;}
		else if (i < 50)
		{ j = 8;}
		else if (i < 60)
		{ j = 10;}
		else if (i < 70)
		{ j = 12;}
		else if (i < 80)
		{ j = 14;}
		else if (i < 90)
		{ j = 16;}
		else if (i < 100)
		{ j = 18;}
	
		m_Billboard_V[i]->m_Transform->SetPosition({ (i % 10) * 1.5f, 0.0f, j * 1.0f });
	
		Billboard* _billboard2 = new Billboard();
		//_billboard2->m_SpriteName_V.push_back("../Data/Image/RifleFront10.png");
		_billboard2->m_SpriteName_V.push_back("RifleFront10.png");
		_billboard2->SetRotationType(eRotationType::YAxis);
		_billboard2->SetProportion(0.01f, 0.01f);
		m_Billboard_V[i]->AddComponent<Billboard>(_billboard2);
	}
	*/
	/// 빌보드
	//m_Billboard = DLLEngine::CreateObject(this);
	//m_Billboard->SetObjectName("Billboard");
	//m_Billboard->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
	//
	//PlaneAnimation* _newBillboardAni = new PlaneAnimation();
	//_newBillboardAni->SetFPS(eFrameRate::FPS24, 2);
	//m_Billboard->AddComponent<PlaneAnimation>(_newBillboardAni);
	//
	//Billboard* _billboard = new Billboard();
	//for (int i = 0; i < 18; ++i)
	//{
	//	std::string first = "../Data/Image/burningbunnyAniFile/fireEffect_";
	//	std::string middle = to_string(i + 1);
	//
	//	if (middle.size() < 2)
	//	{
	//		middle.insert(0, "0");
	//	}
	//	std::string last = ".png";
	//
	//	first.append(middle);
	//	first.append(last);
	//
	//	_billboard->m_SpriteName_V.push_back(first);
	//}
	//
	//_billboard->SeteResourceType(eResourceType::eSeveralAni);
	//_billboard->SetRotationType(eRotationType::YAxis);
	//_billboard->SetProportion(0.001f, 0.001f);
	//_billboard->SetPivot(0.5f, 0.5f);
	//m_Billboard->AddComponent<Billboard>(_billboard);

	/// 빌보드 벡터
	//m_Billboard_V2.resize(100);
	//for (int i = 0; i < m_Billboard_V2.size(); ++i)
	//{
	//	m_Billboard_V2[i] = DLLEngine::CreateObject(this);
	//	std::string _name = "Billboard";
	//	std::string _num = to_string(i + 1);
	//	_name.append(_num);
	//	m_Billboard_V2[i]->SetObjectName(_name);
	//
	//	int j = 0;
	//	if (i < 10)
	//	{
	//		j = 0;
	//	}
	//	else if (i < 20)
	//	{
	//		j = 2;
	//	}
	//	else if (i < 30)
	//	{
	//		j = 4;
	//	}
	//	else if (i < 40)
	//	{
	//		j = 6;
	//	}
	//	else if (i < 50)
	//	{
	//		j = 8;
	//	}
	//	else if (i < 60)
	//	{
	//		j = 10;
	//	}
	//	else if (i < 70)
	//	{
	//		j = 12;
	//	}
	//	else if (i < 80)
	//	{
	//		j = 14;
	//	}
	//	else if (i < 90)
	//	{
	//		j = 16;
	//	}
	//	else if (i < 100)
	//	{
	//		j = 18;
	//	}
	//
	//	m_Billboard_V2[i]->m_Transform->SetPosition({ 20.f + (i % 10) * 1.5f, 0.0f, j * 1.0f });
	//
	//	Billboard* _billboard2 = new Billboard();
	//	_billboard2->m_SpriteName = "../Data/Image/Tree.png";
	//	_billboard2->SetRotationType(eRotationType::YAxis);
	//	m_Billboard_V2[i]->AddComponent<Billboard>(_billboard2);
	//}

	/// 테스트 오브젝트1
	//m_TestObject = DLLEngine::CreateObject(this);
	//m_TestObject->SetObjectName("TestObject1");
	//
	//MeshFilter* _newMesh = new MeshFilter();
	//m_TestObject->AddComponent<MeshFilter>(_newMesh);
	//
	//MaterialComponent* _newMat = new MaterialComponent(); 
	//_newMat->SetVertexShaderName("vs_stnd.cso");
	//_newMat->SetPixelShaderName("ps_stnd.cso");
	//m_TestObject->AddComponent<MaterialComponent>(_newMat);
	//m_TestObject->AddComponent<MeshRenderer>(new MeshRenderer);

	/// D2D 텍스트
	//#define WHITE 1.0f, 1.0f, 1.0f, 1.0f
	//
	//m_D2DText = DLLEngine::CreateObject(this);
	//m_D2DText->SetObjectName("D2DTextExample");
	//
	//MeshFilter* _SpriteTextMesh = new MeshFilter();
	//m_D2DText->AddComponent<MeshFilter>(_SpriteTextMesh);
	//Text* _D2DText = new Text();
	//
	//int _i = 100;
	//float _f = 200.f;
	//char _c = 'A';
	//const wchar_t* _tcahrStr = L"TcharStr";
	//const wchar_t* _tcahrStr2 = L"const wchar_t*를 사용해야 텍스트를 출력할수있다";
	//const wchar_t* _Error = L" ->Error";
	//
	//_D2DText->SetSpriteTextInfo(
	//	L"../Data/Fonts/HYgothic.ttf", 
	//	50.f, 320.f, 
	//	1.0f, 0.f, 0.f, 1.0f,
	//	100.f, 13.f, 12.f);
	//_D2DText->PrintSpriteText((TCHAR*)L"텍스트 실험");
	//_D2DText->PrintSpriteText((TCHAR*)L"I can print English");
	//
	//_D2DText->PrintSpriteText((TCHAR*)L"Print int : %d", _i);
	//_D2DText->PrintSpriteText((TCHAR*)L"Print float : %f", _f);
	//_D2DText->PrintSpriteText((TCHAR*)L"Print char : %c", _c);
	//_D2DText->PrintSpriteText((TCHAR*)L"Print string : %s", _tcahrStr);
	//_D2DText->PrintSpriteText((TCHAR*)L"Print string : %s", _tcahrStr2);
	//
	//m_D2DText->AddComponent<Text>(_D2DText);

	/// D2D 텍스트2
	//m_D2DText2 = DLLEngine::CreateObject(this);
	//m_D2DText2->SetObjectName("D2DTextExample2");
	//
	//MeshFilter* _SpriteTextMesh2 = new MeshFilter();
	//m_D2DText2->AddComponent<MeshFilter>(_SpriteTextMesh2);
	//Text* _D2DText2 = new Text();
	//
	//_D2DText2->SetSpriteTextInfo(
	//	L"../Data/Fonts/Font1.ttf",
	//	200.f, 320.f,
	//	0.f, 1.f, 1.f, 1.0f,
	//	1000.f, 30.f, 60.f);
	//_D2DText2->PrintSpriteText((TCHAR*)L"Text 출력", _i);
	//_D2DText2->PrintSpriteText((TCHAR*)L"Text 출력", _i);
	//m_D2DText2->AddComponent<Text>(_D2DText2);

	/// 버튼
	m_Button = DLLEngine::CreateObject(this);
	m_Button->m_Transform->SetPosition({ 20.0f, 20.0f, 0.0f });
	m_Button->SetObjectName("ButtonOld");
	
	Button* _button = new Button();
	
	_button->m_OffSprite = "button_start_up.png";
	_button->m_OnSprite = "button_start_down.png";

	//_button->m_OffSprite = "../Data/Image/button_start_up.png";
	//_button->m_OnSprite = "../Data/Image/button_start_down.png";

	_button->SetProportion(0.34f, 0.34f);
	_button->SetPivot(1.f, 1.f);
	_button->SetFunc([]() { CA_TRACE("버튼이 눌린다뤼"); });
	
	m_Button->AddComponent<Button>(_button);

	/// 스프라이트 애니메이션
	//m_SpriteAni = DLLEngine::CreateObject(this);
	//m_SpriteAni->m_Transform->SetPosition({ 30.0f, 620.0f, 0.0f });
	//m_SpriteAni->SetObjectName("spriteAni");
	//
	//PlaneAnimation* _newSpriteAni = new PlaneAnimation();
	//_newSpriteAni->SetFPS(eFrameRate::FPS24, 1);
	//m_SpriteAni->AddComponent<PlaneAnimation>(_newSpriteAni);
	//
	//Sprite2D* _ani = new Sprite2D;	
	//_ani->SetType(eResourceType::eSeveralAni);
	//
	////for (int i = 0; i < 18; ++i)
	////{
	////	std::string first = "../Data/Image/burningbunnyAniFile/fireEffect_"; 
	////	std::string middle = to_string(i + 1);
	////
	////	if (middle.size() < 2)
	////	{
	////		middle.insert(0,"0");
	////	}
	////	std::string last = ".png";
	////
	////	first.append(middle);
	////	first.append(last);
	////
	////	_ani->m_SpriteName_V.push_back(first);
	////}
	//
	//for (int i = 0; i < 8; ++i)
	//{
	//	std::string first = "../Data/Image/walking_";
	//	std::string middle = to_string(i + 1);
	//
	//	if (middle.size() < 2)
	//	{
	//		middle.insert(0, "00");
	//	}
	//	std::string last = ".png";
	//
	//	first.append(middle);
	//	first.append(last);
	//
	//	_ani->m_SpriteName_V.push_back(first);
	//}	
	//_ani->SetProportion(1.f, 1.f);
	//_ani->SetPivot(1.f, 0.3f);	
	//
	//m_SpriteAni->AddComponent<Sprite2D>(_ani);

	/// 스프라이트
	//m_Sprite = DLLEngine::CreateObject(this);
	//m_Sprite->m_Transform->SetPosition({ 100.0f, 400.0f, 0.0f });
	//m_Sprite->SetObjectName("giraffa_sprite");
	//
	//Sprite2D* _sptire = new Sprite2D;
	//_sptire->SetType(eResourceType::eSingleImage);
	//_sptire->m_SpriteName_V.push_back("../Data/Image/whiteBunny.png");
	//_sptire->SetProportion(0.5f, 0.5f);
	//_sptire->SetPivot(1.f, 0.3f);
	//
	//m_Sprite->AddComponent<Sprite2D>(_sptire);

	/// 파티클
	//m_Particle = DLLEngine::CreateObject(this);
	//m_Particle->m_Transform->SetPosition({ 0.0f, 20.0f, 0.0f });
	//m_Particle->SetObjectName("particle");
	//
	//Particle* _particle = new Particle;
	//_particle->m_SpriteName = "../Data/Image/star.dds";
	//
	//ParticleAdmin* _particleAdmin = new ParticleAdmin;
	//
	//m_Particle->AddComponent<Particle>(_particle);
	//m_Particle->AddComponent<ParticleAdmin>(_particleAdmin);
	
	/// Directional Light
	//m_pDirectionalLight = DLLEngine::CreateObject(this);
	//m_pDirectionalLight->SetObjectName("Directional Light");
	//Light* _nowLight = new Light();
	//m_pDirectionalLight->AddComponent<Light>(_nowLight);

	/// 트랜스폼 애니메이션
	//m_TransformAnimation = DLLEngine::CreateObject(this);
	//m_TransformAnimation->SetObjectFileName("TransAni");
	//
	//// DoorFrame
	//m_DoorFrame = DLLEngine::CreateObject(this);
	//m_DoorFrame->SetObjectName("DoorFrame");
	//m_DoorFrame->SetParent(m_TransformAnimation);
	//
	//MeshFilter* _DoorFrameMesh = new MeshFilter();
	//m_DoorFrame->AddComponent<MeshFilter>(_DoorFrameMesh);
	//
	//MaterialComponent* _DoorFrameMat = new MaterialComponent();
	//_DoorFrameMat->SetVertexShaderName("vs_stnd.cso");
	//_DoorFrameMat->SetPixelShaderName("ps_stnd.cso");
	//m_DoorFrame->AddComponent<MaterialComponent>(_DoorFrameMat);
	//m_DoorFrame->AddComponent<MeshRenderer>(new MeshRenderer);
	//
	//// Door1
	//m_Door1 = DLLEngine::CreateObject(this);
	//m_Door1->SetObjectName("Door1");
	//m_Door1->SetParent(m_TransformAnimation);
	//
	//MeshFilter* _actor1Mesh = new MeshFilter();
	//m_Door1->AddComponent<MeshFilter>(_actor1Mesh);
	//
	//MaterialComponent* _actor1Mat = new MaterialComponent();
	//_actor1Mat->SetVertexShaderName("vs_stnd.cso");
	//_actor1Mat->SetPixelShaderName("ps_stnd.cso");
	//m_Door1->AddComponent<MaterialComponent>(_actor1Mat);
	//m_Door1->AddComponent<MeshRenderer>(new MeshRenderer);
	//
	//// Door2
	//m_Door2 = DLLEngine::CreateObject(this);
	//m_Door2->SetObjectName("Door2");
	//m_Door2->SetParent(m_TransformAnimation);
	//
	//MeshFilter* _actor2Mesh = new MeshFilter();
	//m_Door2->AddComponent<MeshFilter>(_actor2Mesh);
	//
	//MaterialComponent* _actor2Mat = new MaterialComponent();
	//_actor2Mat->SetVertexShaderName("vs_stnd.cso");
	//_actor2Mat->SetPixelShaderName("ps_stnd.cso");
	//m_Door2->AddComponent<MaterialComponent>(_actor2Mat);
	//m_Door2->AddComponent<MeshRenderer>(new MeshRenderer);
	//
	//
	//TransformAnimation* _transformAnimation = new TransformAnimation;
	//_transformAnimation->SetPlayType(ePlayType::Loop);
	//_transformAnimation->SetFrameRate(eFrameRate::FPS60);
	//_transformAnimation->AddActor(m_Door1);
	//_transformAnimation->AddActor(m_Door2);
	//
	//_transformAnimation->GetActor("Door1")->AddKeyFrame(3.f, DirectX::SimpleMath::Vector3(3.f, 0.0f, 0.0f));
	//_transformAnimation->GetActor("Door1")->AddKeyFrame(3.f, DirectX::SimpleMath::Vector3(5.f, 0.0f, 0.0f));
	//
	//_transformAnimation->GetActor("Door2")->AddKeyFrame(3.f, DirectX::SimpleMath::Vector3(-3.f, 0.0f, 0.0f));
	//_transformAnimation->GetActor("Door2")->AddKeyFrame(3.f, DirectX::SimpleMath::Vector3(-5.f, 0.0f, 0.0f));
	//
	//_transformAnimation->Play();
	//
	//m_TransformAnimation->AddComponent<TransformAnimation>(_transformAnimation);

/*
	/// OIT
	m_OIT = DLLEngine::CreateObject(this);
	m_OIT->SetObjectName("OITTest");
	m_OIT->AddComponent<TempOit>(new TempOit);

	/// OIT
	m_OIT2 = DLLEngine::CreateObject(this);
	m_OIT2->SetObjectName("OITTest2");
	m_OIT2->AddComponent<TempOit2>(new TempOit2);
	*/
	/// 카메라
	m_pMainCamera = DLLEngine::CreateObject(this);
	m_pMainCamera->m_Transform->SetPosition({ -40.0f, 50.0f, -40.0f });
	m_pMainCamera->SetObjectName("SpriteMainCamera");
	//CoverRenderFilter(m_MainCamera);
	// m_TestObject 를 m_MainCamera 의 부모로 설정
	//m_MainCamera->m_Transform->SetParentTM(m_TestObject2);

	m_pMainCamera->AddComponent<Camera>(new Camera("CLTestScene2D_MainCamera"));
	DLLEngine::SetNowCamera("CLTestScene2D_MainCamera");
	m_pMainCamera->AddComponent<CameraController>(new CameraController());

}

void CLTestScene2D::Update(float dTime)
{
	// onResize를 위해 위치값 update
	float width = static_cast<float>(DLLWindow::GetScreenWidth()) / 2;
	float Hight = static_cast<float>(DLLWindow::GetScreenHeight()) / 2;
	//m_Sprite->m_Transform->SetPosition({ width, Hight , 0.0f });
	//m_spriteAni->m_Transform->SetPosition({ width, Hight , 0.0f });

}

void CLTestScene2D::Reset()
{
}
