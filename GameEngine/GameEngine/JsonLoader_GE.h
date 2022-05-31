#pragma once

/// <summary>
/// Client���� ��ӹ޾ƾ� AddComponent�� ä�����Ѵ�.
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

	std::map<unsigned int, GameObject*>		m_GameObject_M;		//�Ľ� ������ ����
	std::map<unsigned int, ComponentBase*>	m_Component_M;		//�Ľ� ������ ����

};

