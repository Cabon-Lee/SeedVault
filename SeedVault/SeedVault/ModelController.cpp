#include "pch.h"
#include "ModelController.h"

ModelController::ModelController()
	: ComponentBase(ComponentType::Input)
{

}

ModelController::~ModelController()
{

}

void ModelController::Start()
{

}

void ModelController::Update(float dTime)
{
	UserInputUpdate();
}

void ModelController::OnRender()
{

}

void ModelController::UserInputUpdate()
{
	if (DLLInput::InputKeyUp(static_cast<int>(CL::KeyCode::KEY_T)))
	{

	}
}
