#include "pch.h"
#include "CLTestInGameUI.h"
#include "CameraController.h"

CLTestInGameUI::CLTestInGameUI()
	:m_pDirectionalLight(nullptr)
{
}

CLTestInGameUI::~CLTestInGameUI()
{
}

void CLTestInGameUI::Initialize()
{
	///// Directional Light
	//m_pDirectionalLight = DLLEngine::CreateObject(this);
	//m_pDirectionalLight->SetObjectName("Directional Light");
	//Light* _nowLight = new Light();
	//m_pDirectionalLight->AddComponent<Light>(_nowLight);

	///// 카메라
	//m_pMainCamera = DLLEngine::CreateObject(this);
	//m_pMainCamera->m_Transform->SetPosition({ -40.0f, 50.0f, -40.0f });
	//m_pMainCamera->SetObjectName("SpriteMainCamera");

	//m_pMainCamera->AddComponent<Camera>(new Camera("CLTestScene2D_MainCamera"));
	//DLLEngine::SetNowCamera("CLTestScene2D_MainCamera");
	//m_pMainCamera->AddComponent<CameraController>(new CameraController());

	/////
	///// 조준점
	/////
	/////(스프라이트, 키입력)
	//m_aimPoint = DLLEngine::CreateObject(this);
	//m_aimPoint->SetObjectName("Aim Point");

	//Sprite2D* _aimSptire = new Sprite2D;
	//_aimSptire->SetType(eResourceType::eSingleImage);
	//_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_Off.png");
	//_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_On.png");
	//_aimSptire->SetProportion(1.f, 1.f);
	//_aimSptire->SetPivot(0.5f, 0.5f);
	//_aimSptire->SetUIAxis(eUIAxis::Center);

	//m_aimPoint->AddComponent<Sprite2D>(_aimSptire);

	///// 퀘스트 텍스트(텍스트 X2, sprite) 
	/////퀘스트 제목
	////(텍스트 , sprite)
	//m_questText = DLLEngine::CreateObject(this);
	//m_questText->SetObjectName("Quest Text");

	//Sprite2D* _QuestSptire = new Sprite2D;
	//_QuestSptire->SetType(eResourceType::eSingleImage);
	//_QuestSptire->m_SpriteName_V.push_back("Quest_Text_Line.png");
	//_QuestSptire->SetProportion(1.0f, 1.0f);
	//_QuestSptire->SetPivot(0.5f, 0.5f);
	//_QuestSptire->SetUIAxis(eUIAxis::RightUp);

	//// (( "발전기 수리" ))
	//Text* _QuestText1 = new Text(); 
	//_QuestText1->SetSpriteTextInfo(
	//	L"../Data/Fonts/Font1.ttf",
	//	350.f, 20.f,
	//	1.f, 1.f, 1.f, 1.0f,
	//	1000.f, 0.f, 45.f, eUIAxis::RightUp);

	//m_questText->AddComponent<Sprite2D>(_QuestSptire);
	//m_questText->AddComponent<Text>(_QuestText1);

	/////	퀘스트 내용
	////(텍스트)
	//m_questText_Sub = DLLEngine::CreateObject(this);
	//m_questText_Sub->SetObjectName("Quest Text Sub");

	//Text* _QuestText2 = new Text();
	//_QuestText2->SetSpriteTextInfo(
	//	L"../Data/Fonts/Font1.ttf",
	//	350.f, 80.f,
	//	1.f, 1.f, 1.f, 1.0f,
	//	1000.f, 0.f, 23.f, eUIAxis::RightUp);
	//
	//m_questText_Sub->AddComponent<Text>(_QuestText2);

	//float _battleStatePosX = 500.f;
	//float _battleStatePosY = 120.f;

	/////
	///// 전투 스탯
	/////
	////(스프라이트 X6, 텍스트 X6, 스프라이트(life) X6)
	//m_combetStat = DLLEngine::CreateObject(this);
	//m_combetStat->SetObjectName("Combet Stat");

	///// 무기
	//m_combetStat_Weapon = DLLEngine::CreateObject(this);
	//m_combetStat_Weapon->SetObjectName("Combet Weapon Stat");

	//Sprite2D* _WeaponSptire = new Sprite2D;
	//_WeaponSptire->SetType(eResourceType::eSingleImage);
	//_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Pistol.png");
	//_WeaponSptire->SetProportion(1.f, 1.f);
	//_WeaponSptire->SetPivot(1.0f, 1.0f);
	//_WeaponSptire->SetUIAxis(eUIAxis::RightDown);


	//Text* _WeaponText1 = new Text();
	//_WeaponText1->SetSpriteTextInfo(
	//	L"../Data/Fonts/Font1.ttf",
	//	_battleStatePosX - 15.f, _battleStatePosY - 10.f,
	//	1.f, 1.f, 1.f, 1.0f,
	//	1000.f, 0.f, 51.f, eUIAxis::RightDown);

	//m_combetStat_Weapon->AddComponent<Sprite2D>(_WeaponSptire);
	//m_combetStat_Weapon->AddComponent<Text>(_WeaponText1);

	///// 플라스크
	//m_combetStat_Flask = DLLEngine::CreateObject(this);
	//m_combetStat_Flask->SetObjectName("Combet Flask Stat");

	//Sprite2D* _FlaskSptire = new Sprite2D;
	//_FlaskSptire->SetType(eResourceType::eSingleImage);
	//_FlaskSptire->m_SpriteName_V.push_back("UI_Item_Flask_On.png");
	//_FlaskSptire->SetProportion(1.f, 1.f);
	//_FlaskSptire->SetPivot(1.0f, 1.0f);
	//_FlaskSptire->SetUIAxis(eUIAxis::RightDown);

	//m_combetStat_Flask->AddComponent<Sprite2D>(_FlaskSptire);

	///// 화염병
	//m_combetStat_Fire = DLLEngine::CreateObject(this);
	//m_combetStat_Fire->SetObjectName("Combet Fire Stat");

	//Sprite2D* _FireSptire = new Sprite2D;
	//_FireSptire->SetType(eResourceType::eSingleImage);
	//_FireSptire->m_SpriteName_V.push_back("UI_Item_Fire_On.png");
	//_FireSptire->SetProportion(1.f, 1.f);
	//_FireSptire->SetPivot(1.0f, 1.0f);
	//_FireSptire->SetUIAxis(eUIAxis::RightDown);

	//m_combetStat_Fire->AddComponent<Sprite2D>(_FireSptire);

	///// 힐
	//m_combetStat_Heal = DLLEngine::CreateObject(this);
	//m_combetStat_Heal->SetObjectName("Combet Fire Stat");

	//Sprite2D* _HealSptire = new Sprite2D;
	//_HealSptire->SetType(eResourceType::eSingleImage);
	//_HealSptire->m_SpriteName_V.push_back("UI_Item_Heal_On.png");
	//_HealSptire->SetProportion(1.f, 1.f);
	//_HealSptire->SetPivot(1.0f, 1.0f);
	//_HealSptire->SetUIAxis(eUIAxis::RightDown);

	//m_combetStat_Heal->AddComponent<Sprite2D>(_HealSptire);

	///// 알코올
	//m_combetStat_Alcohol = DLLEngine::CreateObject(this);
	//m_combetStat_Alcohol->SetObjectName("Combet Fire Stat");

	//Sprite2D* _AlcoholSptire = new Sprite2D;
	//_AlcoholSptire->SetType(eResourceType::eSingleImage);
	//_AlcoholSptire->m_SpriteName_V.push_back("UI_Item_Alcohol.png");
	//_AlcoholSptire->SetProportion(1.0f, 1.0f);
	//_AlcoholSptire->SetPivot(1.0f, 1.0f);
	//_AlcoholSptire->SetUIAxis(eUIAxis::RightDown);

	//m_combetStat_Alcohol->AddComponent<Sprite2D>(_AlcoholSptire);

	///// 허브
	//m_combetStat_Herb = DLLEngine::CreateObject(this);
	//m_combetStat_Herb->SetObjectName("Combet Fire Stat");

	//Sprite2D* _HerbSptire = new Sprite2D;
	//_HerbSptire->SetType(eResourceType::eSingleImage);
	//_HerbSptire->m_SpriteName_V.push_back("UI_Item_Herb_On.png");
	//_HerbSptire->SetProportion(1.0f, 1.0f);
	//_HerbSptire->SetPivot(1.0f, 1.0f);
	//_HerbSptire->SetUIAxis(eUIAxis::RightDown);

	//m_combetStat_Herb->AddComponent<Sprite2D>(_HerbSptire);

	/////
	///// 플레이어 Life
	/////
	//m_Life = DLLEngine::CreateObject(this);
	//m_Life->SetObjectName("Player Life Stat");

	//Sprite2D* _LifeSptire = new Sprite2D;
	//_LifeSptire->SetType(eResourceType::eSingleImage);
	//_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_6.png");
	//_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_5.png");
	//_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_4.png");
	//_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_3.png");
	//_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_2.png");
	//_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_1.png");
	//_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_0.png");
	//_LifeSptire->SetProportion(1.0f, 1.0f);
	//_LifeSptire->SetPivot(1.0f, 1.0f);
	//_LifeSptire->SetUIAxis(eUIAxis::RightDown);

	//m_Life->AddComponent<Sprite2D>(_LifeSptire);

	///// <summary>
	///// (임시) 조수 Life
	///// </summary>
	//m_TmpAssistLifeBar = DLLEngine::CreateObject(this);
	//m_TmpAssistLifeBar->SetObjectName("Assist Life Bar");
	//m_TmpAssistLifeBar->m_Transform->SetPosition({ 100.f, 100.f, 0.f });

	//Sprite2D* _TmpAssistLifeBarSptire = new Sprite2D;
	//_TmpAssistLifeBarSptire->SetType(eResourceType::eSingleImage);
	//_TmpAssistLifeBarSptire->m_SpriteName_V.push_back("UI_HP_Partner_Off.png");
	//_TmpAssistLifeBarSptire->SetProportion(1.0f, 1.0f);
	//_TmpAssistLifeBarSptire->SetPivot(1.0f, 1.0f);

	//m_TmpAssistLifeBar->AddComponent<Sprite2D>(_TmpAssistLifeBarSptire);


	//m_TmpAssistLife = DLLEngine::CreateObject(this);
	//m_TmpAssistLife->SetObjectName("Assist Life");
	//m_TmpAssistLife->m_Transform->SetPosition({ 100.f, 100.f, 0.f });
	//
	//Sprite2D* _TmpAssistLifeSptire = new Sprite2D;
	//_TmpAssistLifeSptire->SetType(eResourceType::eSingleImage);
	//_TmpAssistLifeSptire->m_SpriteName_V.push_back("UI_HP_Partner_On.png");
	//_TmpAssistLifeSptire->SetProportion(0.5f, 1.f);
	//_TmpAssistLifeSptire->SetPivot(1.0f, 1.0f);

	//m_TmpAssistLife->AddComponent<Sprite2D>(_TmpAssistLifeSptire);


	////Sprite2DBundle* _LifeSptires = new Sprite2DBundle;
	////_LifeSptires->SetType(eResourceType::eSingleImage);
	////_LifeSptires->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });
	////_LifeSptires->SetUIAxis(eUIAxis::RightDown);
	////_LifeSptires->SetProportion(0.9f, 0.9f);
	////
	////_LifeSptires->m_SpriteName_V.push_back("UI_Item_Herb_On.png", );
	////_LifeSptires->SetPivot(0.5f, 0.5f);
	////
	////m_Life->AddComponent<Sprite2DBundle>(_LifeSptires);

	///// -------------------------------------------------------------
	///// UI sprite 포지션
	///// -------------------------------------------------------------
	//	
	//m_questText->m_Transform->SetPosition({ 150.f, 100.f, 0.0f });

	//m_combetStat_Weapon->m_Transform->SetPosition({ _battleStatePosX, _battleStatePosY + 22.f, 0.0f });
	//m_combetStat_Flask->m_Transform->SetPosition({ _battleStatePosX - 185.f, _battleStatePosY, 0.0f });
	//m_combetStat_Fire->m_Transform->SetPosition({ _battleStatePosX - 260.f, _battleStatePosY, 0.0f });
	//m_combetStat_Heal->m_Transform->SetPosition({ _battleStatePosX - 335.f, _battleStatePosY, 0.0f });
	//m_combetStat_Alcohol->m_Transform->SetPosition({ _battleStatePosX - 410.f, _battleStatePosY , 0.0f });
	//m_combetStat_Herb->m_Transform->SetPosition({ _battleStatePosX - 410.f, _battleStatePosY - 35.f, 0.0f });
	//m_Life->m_Transform->SetPosition({ _battleStatePosX + 20, _battleStatePosY - 75.f, 0.0f });
}

void CLTestInGameUI::Update(float dTime)
{
	//m_questText->GetComponent<Text>()->PrintSpriteText((TCHAR*)L"발전기 수리");
	//m_questText_Sub->GetComponent<Text>()->PrintSpriteText((TCHAR*)L"발전기 수리 부품을 찾는다");
	//
	//m_combetStat_Weapon->GetComponent<Text>()->PrintSpriteText((TCHAR*)L"39");


}

void CLTestInGameUI::Reset()
{
}
