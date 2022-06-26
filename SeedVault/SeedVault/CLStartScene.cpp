#include "pch.h"
#include "CLStartScene.h"
#include "CameraController.h"

CLStartScene::CLStartScene()
{
}

CLStartScene::~CLStartScene()
{
}

void CLStartScene::Initialize()
{
	/// 바탕
	m_StartImage = DLLEngine::CreateObject(this);
	m_StartImage->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_StartImage->SetObjectName("fakeStartBack");

	Sprite2D* _StartImage = new Sprite2D();

	_StartImage->m_SpriteName_V.push_back("fakeStartScene.png");

	_StartImage->SetProportion(1.f, 1.f);
	_StartImage->SetPivot(0.5f, 0.5f);
	_StartImage->SetUIAxis(eUIAxis::Center);

	m_StartImage->AddComponent<Sprite2D>(_StartImage);

	/// --------------------------------------------------
	/// 버튼
	/// --------------------------------------------------
	
	/// 시작버튼 

	float _x = 330.f;
	float _y = 0.f;

	m_StartButton = DLLEngine::CreateObject(this);
	m_StartButton->m_Transform->SetPosition({ _x, _y, 1.0f });
	m_StartButton->SetObjectName("TitleStartButton");
	
	Button* _startButton = new Button();
	
	_startButton->m_OffSprite = "button_start_up.png";
	_startButton->m_OnSprite = "button_start_down.png";
	
	_startButton->SetProportion(0.34f, 0.34f);
	_startButton->SetPivot(1.f, 1.f);
	_startButton->SetAxis(eUIAxis::Center);
	_startButton->SetFunc([]() { DLLEngine::PickScene("TestSceneYH"); });
	
	m_StartButton->AddComponent<Button>(_startButton);

	/// 챕터선택버튼 
	m_ChapterChoiceButton = DLLEngine::CreateObject(this);
	m_ChapterChoiceButton->m_Transform->SetPosition({ _x, _y + 110, 1.0f });
	m_ChapterChoiceButton->SetObjectName("TitleChapterButton");

	Button* _chapterButton = new Button();

	_chapterButton->m_OffSprite = "button_chapter_up.png";
	_chapterButton->m_OnSprite = "button_chapter_down.png";

	_chapterButton->SetProportion(0.34f, 0.34f);
	_chapterButton->SetPivot(1.f, 1.f);
	_chapterButton->SetAxis(eUIAxis::Center);
	_chapterButton->SetFunc([]() { CA_TRACE("버튼이 눌린다뤼"); });

	m_ChapterChoiceButton->AddComponent<Button>(_chapterButton);

	/// 옵션버튼 
	m_OptionButton = DLLEngine::CreateObject(this);
	m_OptionButton->m_Transform->SetPosition({ _x, _y + 220, 0.0f });
	m_OptionButton->SetObjectName("TitleOptionButton");

	Button* _optionButton = new Button();

	_optionButton->m_OffSprite = "button_option_up.png";
	_optionButton->m_OnSprite = "button_option_down.png";

	_optionButton->SetProportion(0.34f, 0.34f);
	_optionButton->SetPivot(1.f, 1.f);
	_optionButton->SetAxis(eUIAxis::Center);
	_optionButton->SetFunc([]() { CA_TRACE("버튼이 눌린다뤼"); });

	m_OptionButton->AddComponent<Button>(_optionButton);

	/// 나가기버튼 
	m_ExitButton = DLLEngine::CreateObject(this);
	m_ExitButton->m_Transform->SetPosition({ _x, _y + 330, 0.0f });
	m_ExitButton->SetObjectName("TitleExitButton");

	Button* _exitButton = new Button();

	_exitButton->m_OffSprite = "button_exit_up.png";
	_exitButton->m_OnSprite = "button_exit_down.png";

	_exitButton->SetProportion(0.34f, 0.34f);
	_exitButton->SetPivot(1.f, 1.f);
	_exitButton->SetAxis(eUIAxis::Center);
	_exitButton->SetFunc([]() { CA_TRACE("버튼이 눌린다뤼"); });

	m_ExitButton->AddComponent<Button>(_exitButton);

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

void CLStartScene::Update(float dTime)
{
}

void CLStartScene::Reset()
{
}
