#pragma once

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

class JsonLoader
{
public:
	JsonLoader();
	~JsonLoader();

private:
	Json::CharReaderBuilder*					m_ReaderBuilder;
	const std::unique_ptr<Json::CharReader>		m_Reader;

public:
	Scene*		SceneLoad 					  (const std::string& filePath, const std::string& fileName);
	void		SceneLoad		(Scene* scene, const std::string& filePath, const std::string& fileName);
	GameObject* GameObjectPrefabLoad(const std::string& filePath, const std::string& fileName);

private:
	void SceneLoad(float version, Json::Value* value);
	
	bool AddComponentToGameObject(GameObject* object, std::string compName, Json::Value* value);

	Scene* m_LoadingScene;

	std::map<unsigned int, GameObject*>		m_GameObject_M;		//파싱 데이터 보관
	std::map<unsigned int, ComponentBase*>	m_Component_M;		//파싱 데이터 보관

};

