#include "pch.h"
#include "JsonLoader_CL.h"

#include "PlayerController.h"
#include "CameraBehavior.h"
#include "CameraController.h"
#include "OrbitCamera.h"
#include "Zombie_Runner_AI.h"
#include "Zombie_Runner_Move.h"
#include "GameManager.h"
#include "Health.h"

JsonLoader_CL::JsonLoader_CL()
{

}

JsonLoader_CL::~JsonLoader_CL()
{

}

bool JsonLoader_CL::AddComponentToGameObject(GameObject* object, const char* compName, Json::Value* value)
{
	//GameEmgine Component가 아니면 Client Component이다.
	if (JsonLoader_GE::AddComponentToGameObject(object, compName, value)==true)
	{
		return true;
	}

	ComponentBase* _comp = nullptr;
	if (compName == "CameraController")
	{
		CameraController* _thisComp = new CameraController();
		_comp = _thisComp;
		object->AddComponent<CameraController>(_thisComp);
	}
	else if (compName == "PlayerController")
	{
		PlayerController* _thisComp = new PlayerController();
		_comp = _thisComp;
		object->AddComponent<PlayerController>(_thisComp);
	}

	else if (compName == "CameraBehavior")
	{
		CameraBehavior* _thisComp = new CameraBehavior();
		_comp = _thisComp;
		object->AddComponent<CameraBehavior>(_thisComp);
	}

	else if (compName == "CameraController")
	{
		CameraController* _thisComp = new CameraController();
		_comp = _thisComp;
		object->AddComponent<CameraController>(_thisComp);
	}

	else if (compName == "OrbitCamera")
	{
		OrbitCamera* _thisComp = new OrbitCamera();
		_comp = _thisComp;
		object->AddComponent<OrbitCamera>(_thisComp);
	}

	else if (compName == "Zombie_Runner_AI")
	{
		Zombie_Runner_AI* _thisComp = new Zombie_Runner_AI();
		_comp = _thisComp;
		object->AddComponent<Zombie_Runner_AI>(_thisComp);
	}

	else if (compName == "Zombie_Runner_Move")
	{
		Zombie_Runner_Move* _thisComp = new Zombie_Runner_Move();
		_comp = _thisComp;
		object->AddComponent<Zombie_Runner_Move>(_thisComp);
	}

	else if (compName == "GameManager")
	{
		GameManager* _thisComp = new GameManager();
		_comp = _thisComp;
		object->AddComponent<GameManager>(_thisComp);
	}

	else if (compName == "Health")
	{
		Health* _thisComp = new Health();
		_comp = _thisComp;
		object->AddComponent<Health>(_thisComp);
	}


	if (_comp == nullptr)
	{
		return false;
	}

	else
	{
		m_Component_M.insert(std::make_pair((*value)[compName]["m_ComponentId"].asUInt(), _comp));

		_comp->SetValue(value);
		_comp->Load();
		return true;
	}
}
