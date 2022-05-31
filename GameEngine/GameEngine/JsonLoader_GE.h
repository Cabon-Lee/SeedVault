#pragma once

/// <summary>
/// Client에서 상속받아야 AddComponent를 채워야한다.
/// </summary>
namespace Json
{
	class StreamWriterBuilder;
	class StreamWriter;
	class CharReaderBuilder;
	class CharReader;
	class Value;
}

class Scene;
class GameObject;
class ComponentBase;

class JsonLoader_GE
{
public:
	_DLL JsonLoader_GE();
	_DLL virtual ~JsonLoader_GE();

private:
	Json::CharReaderBuilder*					m_ReaderBuilder;
	const std::unique_ptr<Json::CharReader>		m_Reader;

public:
	Scene* SceneLoad(const std::string& filePath, const std::string& fileName);
	void		SceneLoad(Scene* scene, const std::string& fullFilePath);
	void		SceneLoad(Scene* scene, const std::string& filePath, const std::string& fileName);
	GameObject* GameObjectPrefabLoad(const std::string& filePath, const std::string& fileName);

	 std::map<unsigned int, GameObject*>*	GetGameObject_Map();
	 std::map<unsigned int, ComponentBase*>* GetComponent_Map();
private:
	void SceneLoad(float version, const std::string& fileName, Json::Value* value);

	Scene* m_LoadingScene;

protected:
	 _DLL virtual bool AddComponentToGameObject(GameObject* object, const char* compName, Json::Value* value);

	std::map<unsigned int, GameObject*>		m_GameObject_M;		//파싱 데이터 보관
	std::map<unsigned int, ComponentBase*>	m_Component_M;		//파싱 데이터 보관

};

