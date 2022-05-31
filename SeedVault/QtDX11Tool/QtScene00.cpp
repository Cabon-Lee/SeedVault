#include "pch.h"
#include "QtScene00.h"
#include "Light.h"

QtScene00::QtScene00()
{

}

QtScene00::~QtScene00()
{

}

void QtScene00::Initialize()
{
	GameObject* _cam = DLLEngine::CreateObject(this);
	DLLGameObject::SetName(_cam, "main");
	DLLGameObject::AddCamera(_cam, "main");
	DLLEngine::SetNowCamera("main");
	//DLLGameObject::AddCamraController(DLLEngine::FindGameObjectByName("EngineCamera"));


	GameObject* _light = DLLEngine::CreateObject(this);
	DLLGameObject::SetName(_light, "light");
	DLLGameObject::AddLight(_light);
	_light->m_Transform->SetRotationFromVec({ 270.f, 0.0f, 0.0f });

	DLLGameObject::GetLight(_light)->m_eLightType = eLIGHT_TYPE::DIRECTIONAL;
	DLLGameObject::GetLight(_light)->SetIntensity(0.5);
	DLLGameObject::GetLight(_light)->m_CastShadow = true;


	CA_TRACE("QtScene : Initialize");
}

void QtScene00::Update(float dTime)
{

}

void QtScene00::Reset()
{

}


void QtScene00::Load()
{

}
