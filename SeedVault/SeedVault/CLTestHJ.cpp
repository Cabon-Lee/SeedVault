#include "pch.h"
#include "CLTestHJ.h"

CLTestHJ::CLTestHJ()
{

}

CLTestHJ::~CLTestHJ()
{

}

void CLTestHJ::Initialize()
{
	DLLEngine::CreateObject(this);
	DLLEngine::CreateObject(this , true)->SetObjectName("Continual Object");

}

void CLTestHJ::Update(float dTime)
{

}

void CLTestHJ::Reset()
{

}
