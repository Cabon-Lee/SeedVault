#pragma once

#include "DLLDefine.h"
#include "SceneBase.h"

class GameObject;
class ComponentBase;
class JsonLoader_GE;

class Scene : public SceneBase
{
public:
	_DLL Scene();
	_DLL Scene(std::string sceneName);
	_DLL virtual ~Scene();

public:
	_DLL virtual void Initialize();
	virtual void Update(float dTime);
	virtual void Reset();

	void SetParsingData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);
	_DLL void SetJsonLoader(JsonLoader_GE* jsonLoader , const std::string& fullfilepath);

private:
	JsonLoader_GE*									m_JsonLoader;
	std::string										m_SaveFile;

	std::map<unsigned int, GameObject*>*			m_pParsedGameObject_M;			//파싱 데이터 보관
	std::map<unsigned int, ComponentBase*>*			m_pParsedComponent_M;			//파싱 데이터 보관

private:
	void SetCurrentCamera();
};

