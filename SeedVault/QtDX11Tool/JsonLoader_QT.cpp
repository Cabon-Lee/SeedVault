#include "pch.h"
#include "JsonLoader_QT.h"



JsonLoader_QT::JsonLoader_QT()
{

}

JsonLoader_QT::~JsonLoader_QT()
{

}

bool JsonLoader_QT::AddComponentToGameObject(GameObject* object, const char* compName, Json::Value* value)
{
	//GameEmgine Component가 아니면 Client Component이다.
	if (JsonLoader_GE::AddComponentToGameObject(object, compName, value)==true)
	{
		return true;
	}

	ComponentBase* _comp = nullptr;
	if (strcmp(compName, "CameraController") == 0)
	{
		//CameraController* _thisComp = new CameraController();
		//_comp = _thisComp;

		//DLLGameObject::AddComponentBase(object, _comp, typeid(_thisComp));
		//object->AddComponent<CameraController>(_thisComp);
	}
	
	if (_comp == nullptr)
	{
		return false;
	}

	else
	{
		//JsonLoader_GE::GetComponent_Map()->insert(std::make_pair((value*)[_comp]["m_ComponentId"].asUInt(), _comp));

		//DLLGameObject::JsonLoaderInsertComponentMap(this, (*value)[compName]["m_ComponentId"].asUInt(), _comp);
		//_comp->SetValue(value);
		//_comp->Load();
		return true;
	}
}
