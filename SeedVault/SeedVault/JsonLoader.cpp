#include "pch.h"
#include <fstream>
#include "Scene.h"

#include "Transform.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "PlayerController.h"
#include "CameraBehavior.h"
#include "CameraController.h"
#include "OrbitCamera.h"
#include "Enemy_AI.h"
#include "EnemyMove.h"

#include "JsonLoader.h"

JsonLoader::JsonLoader()
	:m_ReaderBuilder(new Json::CharReaderBuilder())
	, m_Reader(m_ReaderBuilder->newCharReader())
	, m_LoadingScene(nullptr)
{

}

JsonLoader::~JsonLoader()
{

}

Scene* JsonLoader::SceneLoad(const std::string& filePath, const std::string& fileName)
{
	m_LoadingScene = nullptr;
	std::string _fullFileName = filePath + "/" + fileName + ".json";
	/// <summary>
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="fileName"></param>
	/// Json file로 부터 읽어들인 데이터가 들어갈 Value
	Json::Value* _root = new Json::Value();

	std::ifstream _sceneData;
	_sceneData.open(_fullFileName.c_str());
	JSONCPP_STRING _err;

	Json::parseFromStream(*m_ReaderBuilder, _sceneData, _root, &_err);

	if (!(*_root).isNull())
	{
		if ((*_root).isMember("Version"))
		{
			Trace("Json Loader : %s 읽는중", _fullFileName.c_str());

			//version check
			SceneLoad((*_root)["Version"].asFloat(), _root);

			return m_LoadingScene;
		}
	}

	Trace("Json Loader : Scene File을 열고 읽지 못하였습니다.");
	m_LoadingScene = nullptr;
	return DLLEngine::CreateScene(this,"FailedScene");
}

void JsonLoader::SceneLoad(float version, Json::Value* value)
{
	//지금은  버젼 그냥 둔다
	Json::Value _root = *value;

	/// <summary>
	/// null이면 생성하고 아니면 제공된 신을 로딩한다.
	/// </summary>
	if (m_LoadingScene == nullptr)
	{
		if (_root.isMember("Scene"))
		{
			if (_root["Scene"].isMember("m_SceneName"))
			{
				m_LoadingScene = DLLEngine::CreateScene(this,_root["Scene"]["m_SceneName"].asString());
			}
			else if (_root["Scene"].isMember("m_SceneId"))
			{
				m_LoadingScene = DLLEngine::CreateScene(this,"NoName" + std::to_string(_root["Scene"]["m_SceneId"].asUInt()));
			}
			else
			{
				m_LoadingScene = DLLEngine::CreateScene(this,"NoName");
			}
		}
	}


	//m_LoadingScene->SetValue(value);

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
				//임시로 key값을 얻어내기 위함이다. 굳이 벡터가 아니어도 할 수 있으면 바꾸어도 좋다.
				std::vector<std::string> _comp_V = (*_compIt).getMemberNames();


				for (; _compIt != (*_goIt)["GameObject"]["m_Component_V"].end(); _compIt++)
				{
					std::vector<std::string> _allComp_V = (*_compIt).getMemberNames();

					/// <summary>
					/// 초반 오류 제거용
					/// </summary>
					if (_allComp_V.empty())
					{
						continue;
					}

					if (!AddComponentToGameObject(_go, _allComp_V.front(), &((*_compIt))))
					{
						Trace("Json Loader %s Component Fail", _allComp_V.front().c_str());
					}
				}

			}
		}

	}
}

void JsonLoader::SceneLoad(Scene* scene, const std::string& filePath, const std::string& fileName)
{
	m_LoadingScene = scene;

	std::string _fullFileName = filePath + "/" + fileName + ".json";
	/// <summary>
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="fileName"></param>
	/// Json file로 부터 읽어들인 데이터가 들어갈 Value
	Json::Value* _root = new Json::Value();

	std::ifstream _sceneData;
	_sceneData.open(_fullFileName.c_str());
	JSONCPP_STRING _err;

	Json::parseFromStream(*m_ReaderBuilder, _sceneData, _root, &_err);

	if (!(*_root).isNull())
	{
		if ((*_root).isMember("Version"))
		{
			Trace("Json Loader : %s 읽는중", _fullFileName.c_str());

			//version check
			SceneLoad((*_root)["Version"].asFloat(), _root);

			return;
		}
	}

	Trace("Json Loader : Scene File을 열고 읽지 못하였습니다.");
	m_LoadingScene = nullptr;
}

GameObject* JsonLoader::GameObjectPrefabLoad(const std::string& filePath, const std::string& fileName)
{
	return nullptr;
}

bool JsonLoader::AddComponentToGameObject(GameObject* object, std::string compName, Json::Value* value)
{
	ComponentBase* _comp = nullptr;
	if (compName == "Transform")
	{
		Transform* _thisComp = new Transform();
		_comp = _thisComp;
		object->m_Transform = _thisComp;
		object->AddComponent<Transform>(_thisComp);

	}
	else if (compName == "MeshRenderer")
	{
		MeshRenderer* _thisComp = new MeshRenderer();
		_comp = _thisComp;
		object->AddComponent<MeshRenderer>(_thisComp);
	}
	else if (compName == "MeshFilter")
	{
		MeshFilter* _thisComp = new MeshFilter();
		_comp = _thisComp;
		object->AddComponent<MeshFilter>(_thisComp);
	}
	else if (compName == "MaterialComponent")
	{
		MaterialComponent* _thisComp = new MaterialComponent();
		_comp = _thisComp;
		object->AddComponent<MaterialComponent>(_thisComp);
	}
	else if (compName == "Camera")
	{
		Camera* _thisComp = new Camera("");
		_comp = _thisComp;
		object->AddComponent<Camera>(_thisComp);
	}
	else if (compName == "Light")
	{
		Light* _thisComp = new Light();
		_comp = _thisComp;
		object->AddComponent<Light>(_thisComp);
	}
	else if (compName == "CameraController")
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

	else if (compName == "Enemy_AI")
	{
		Enemy_AI* _thisComp = new Enemy_AI();
		_comp = _thisComp;
		object->AddComponent<Enemy_AI>(_thisComp);
	}

	else if (compName == "EnemyMove")
	{
		EnemyMove* _thisComp = new EnemyMove();
		_comp = _thisComp;
		object->AddComponent<EnemyMove>(_thisComp);
	}


	if (_comp == nullptr)
	{
		return false;
	}
	else
	{
		m_Component_M.insert(std::make_pair((*value)[compName.c_str()]["m_ComponentId"].asUInt(), _comp));

		_comp->SetValue(value);
		_comp->Load();
		return true;
	}

}

