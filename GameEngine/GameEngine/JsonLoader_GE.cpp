#include "pch.h"
#include "JsonLoader_GE.h"

#include "EngineDLL.h"

#include "Transform.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"

#include "Sprite2D.h"
#include "Button.h"
#include "Billboard.h"

#include <fstream>
#include "Scene.h"

_DLL JsonLoader_GE::JsonLoader_GE()
	:m_ReaderBuilder(new Json::CharReaderBuilder())
	, m_Reader(m_ReaderBuilder->newCharReader())
	, m_LoadingScene(nullptr)
	, m_Component_M()
	, m_GameObject_M()
{

}

_DLL JsonLoader_GE::~JsonLoader_GE()
{

}

Scene* JsonLoader_GE::SceneLoad(const std::string& filePath, const std::string& fileName)
{
	m_Component_M.clear();
	m_GameObject_M.clear();

	m_LoadingScene = nullptr;

	std::string _fullFileName = filePath + "/" + fileName + ".json";
	/// Json file�� ���� �о���� �����Ͱ� �� Value
	Json::Value* _root = new Json::Value();

	std::ifstream _sceneData;
	_sceneData.open(_fullFileName.c_str());
	JSONCPP_STRING _err;

	Json::parseFromStream(*m_ReaderBuilder, _sceneData, _root, &_err);

	if (!(*_root).isNull())
	{
		if ((*_root).isMember("Version"))
		{
			Trace("Json Loader : %s �д���", _fullFileName.c_str());

			//version check
			SceneLoad((*_root)["Version"].asFloat(), fileName, _root);

			return m_LoadingScene;
		}
	}

	Trace("Json Loader : Scene File�� ���� ���� ���Ͽ����ϴ�.");
	m_LoadingScene = nullptr;
	return DLLEngine::CreateScene(this, fileName.c_str(), "FailedScene");
}

void JsonLoader_GE::SceneLoad(Scene* scene, const std::string& filePath, const std::string& fileName)
{
	m_Component_M.clear();
	m_GameObject_M.clear();

	m_LoadingScene = scene;
	std::string _fullFileName = filePath + "/" + fileName + ".json";
	/// Json file�� ���� �о���� �����Ͱ� �� Value
	Json::Value* _root = new Json::Value();

	std::ifstream _sceneData;
	_sceneData.open(_fullFileName.c_str());
	JSONCPP_STRING _err;

	bool is_success = Json::parseFromStream(*m_ReaderBuilder, _sceneData, _root, &_err);

	if (!(*_root).isNull() || is_success == false)
	{
		if ((*_root).isMember("Version"))
		{
			Trace("Json Loader : %s �д���", _fullFileName.c_str());

			//version check
			SceneLoad((*_root)["Version"].asFloat(), fileName, _root);
			Trace("Json Loader : Scene File�� �д� ���Դϴ�.");
			return;
		}
	}

	Trace("Json Loader : Scene File�� ���� ���� ���Ͽ����ϴ�.");
	m_LoadingScene = nullptr;
}

void JsonLoader_GE::SceneLoad(float version, const std::string& fileName, Json::Value* value)
{
	//������  ���� �׳� �д�
	Json::Value _root = *value;

	/// <summary>
	/// null�̸� �����ϰ� �ƴϸ� ������ ���� �ε��Ѵ�.
	/// </summary>
	if (m_LoadingScene == nullptr)
	{
		if (_root.isMember("Scene"))
		{
			if (_root["Scene"].isMember("m_SceneName"))
			{
				m_LoadingScene = DLLEngine::CreateScene(this, fileName.c_str(), _root["Scene"]["m_SceneName"].asString().c_str());
			}
			else if (_root["Scene"].isMember("m_SceneId"))
			{
				m_LoadingScene = DLLEngine::CreateScene(this, fileName.c_str(), std::string("NoName" + std::to_string(_root["Scene"]["m_SceneId"].asUInt())).c_str());
			}
			else
			{
				m_LoadingScene = DLLEngine::CreateScene(this, fileName.c_str(), "NoName");
			}
		}
	}


	DLLEngine::SetValue(m_LoadingScene, value);
	DLLEngine::SetParsingData(m_LoadingScene, &m_GameObject_M, &m_Component_M);

	m_LoadingScene->Load();

	if (_root["Scene"].isMember("m_GameObject_V"))
	{
		if (_root["Scene"]["m_GameObject_V"].isArray())
		{
			Json::ValueIterator _goIt = _root["Scene"]["m_GameObject_V"].begin();
			for (; _goIt != _root["Scene"]["m_GameObject_V"].end(); _goIt++)
			{
				GameObject* _go = DLLEngine::CreateObject_NoTransform(m_LoadingScene);
				Json::Value* _goValue = new Json::Value();
				_goValue->copy((*_goIt));
				_go->SetValue(_goValue);
				_go->Load();


				m_GameObject_M.insert(std::make_pair((*_goIt)["GameObject"]["m_GameObjectId"].asUInt(), _go));

				Json::ValueIterator _compIt = (*_goIt)["GameObject"]["m_Component_V"].begin();
				//�ӽ÷� key���� ���� �����̴�. ���� ���Ͱ� �ƴϾ �� �� ������ �ٲپ ����.
				std::vector<std::string> _comp_V = (*_compIt).getMemberNames();


				for (; _compIt != (*_goIt)["GameObject"]["m_Component_V"].end(); _compIt++)
				{
					std::vector<std::string> _allComp_V = (*_compIt).getMemberNames();

					/// <summary>
					/// �ʹ� ���� ���ſ�
					/// </summary>
					if (_allComp_V.empty())
					{
						continue;
					}

					if (!AddComponentToGameObject(_go, _allComp_V.front().c_str(), &((*_compIt))))
					{
						Trace("Json Loader %s Component Fail", _allComp_V.front().c_str());
					}
				}

			}
		}

	}
}

//��ο� ���ϸ��� ������ ������.
void JsonLoader_GE::SceneLoad(Scene* scene, const std::string& fullFilePath)
{
	m_Component_M.clear();
	m_GameObject_M.clear();

	m_LoadingScene = scene;
	std::string _fullFileName = fullFilePath + ".json";
	/// Json file�� ���� �о���� �����Ͱ� �� Value
	Json::Value* _root = new Json::Value();

	std::ifstream _sceneData;
	_sceneData.open(_fullFileName.c_str());
	JSONCPP_STRING _err;
	std::string _fileName;

	Json::parseFromStream(*m_ReaderBuilder, _sceneData, _root, &_err);

	if (!(*_root).isNull())
	{
		if ((*_root).isMember("Version"))
		{
			Trace("Json Loader : %s �д���", _fullFileName.c_str());
			//������ "/"������ ���ڿ��� �� ���ϸ��̴�.
			_fileName = fullFilePath.substr(fullFilePath.find_last_of("/"));
			//version check
			SceneLoad((*_root)["Version"].asFloat(), _fileName, _root);

			return;
		}
	}

	Trace("Json Loader : Scene File�� ���� ���� ���Ͽ����ϴ�.");
	m_LoadingScene = nullptr;
}

bool JsonLoader_GE::AddComponentToGameObject(GameObject* object, const char* compName, Json::Value* value)
{
	ComponentBase* _comp = nullptr;

	//�� ���ڿ��� �����ϸ� 0�� ��ȯ�Ѵ�.
	if (strcmp(compName, "Transform") == 0)
	{
		Transform* _thisComp = new Transform();
		_comp = _thisComp;
		object->m_Transform = _thisComp;
		object->AddComponent<Transform>(_thisComp);
	}
	else if (strcmp(compName, "MeshRenderer") == 0)
	{
		MeshRenderer* _thisComp = new MeshRenderer();
		_comp = _thisComp;
		object->AddComponent<MeshRenderer>(_thisComp);
	}
	else if (strcmp(compName, "MeshFilter") == 0)
	{
		MeshFilter* _thisComp = new MeshFilter();
		_comp = _thisComp;
		object->AddComponent<MeshFilter>(_thisComp);
	}
	else if (strcmp(compName, "Camera") == 0)
	{
		Camera* _thisComp = new Camera((*value)[compName]["m_Name"].asString().c_str());
		_comp = _thisComp;
		object->AddComponent<Camera>(_thisComp);
	}
	else if (strcmp(compName, "Light") == 0)
	{
		Light* _thisComp = new Light();
		_comp = _thisComp;
		object->AddComponent<Light>(_thisComp);
	}
	else if (strcmp(compName, "Sprite") == 0)
	{
		Sprite2D* _thisComp = new Sprite2D();
		_comp = _thisComp;
		object->AddComponent<Sprite2D>(_thisComp);
	}
	else if (strcmp(compName, "Button") == 0)
	{
		Button* _thisComp = new Button();
		_comp = _thisComp;
		object->AddComponent<Button>(_thisComp);
	}
	else if (strcmp(compName, "Billboard") == 0)
	{
		Billboard* _thisComp = new Billboard();
		_comp = _thisComp;
		object->AddComponent<Billboard>(_thisComp);
	}
	else if (strcmp(compName, "PhysicsActor") == 0)
	{
		PhysicsActor* _thisComp = new PhysicsActor();
		_comp = _thisComp;
		object->AddComponent<PhysicsActor>(_thisComp);
	}
	else if (strcmp(compName, "ReflectionProbeCamera") == 0)
	{
		ReflectionProbeCamera* _thisComp = new ReflectionProbeCamera();
		_comp = _thisComp;
		object->AddComponent<ReflectionProbeCamera>(_thisComp);
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

 std::map<unsigned int, GameObject*>* JsonLoader_GE::GetGameObject_Map()
{
	return &m_GameObject_M;
}

 std::map<unsigned int, ComponentBase*>* JsonLoader_GE::GetComponent_Map()
{
	return &m_Component_M;
}

GameObject* JsonLoader_GE::GameObjectPrefabLoad(const std::string& filePath, const std::string& fileName)
{
	return nullptr;
}
