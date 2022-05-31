#pragma once
//#include "IScene.h"

#include "DLLDefine.h"
#include "JsonSerializer.h"
#include <memory>

class GameObject;
class Camera;
class ObjectManager;
class Value;

#define IDSCENE 1

struct Scene_Save;


class SceneBase : public JsonSerializer //: public IScene 
{
	//private:
	//	SceneBase(); // 상속해서 쓰긴해야하는데
public:
	_DLL SceneBase();
	_DLL virtual ~SceneBase();

public:
	void Initialize_();
	virtual void Initialize()abstract;
	virtual void Update(float dTime) abstract;
	virtual void Reset() abstract;


	void SetName(std::string& name);
	_DLL std::string& GetSceneName();
	const unsigned int& GetSceneId();
	bool IsPhysicsScene() const;
	void SetPhysicsScene(bool val);

	_DLL const unsigned int GetSceneIndex();
	void SetSceneIndex(const unsigned int idx);

protected:
	const unsigned int m_SceneId;
	unsigned int m_SceneIndex;

	std::string m_SceneName;
	bool m_bPhysicsScene;

	std::string m_CurrentCamreaName;
public:
	std::shared_ptr<ObjectManager> m_pGameObjectManager;

public:
	void StartComponent();
	void SetParentChild();
	void AddObject(GameObject* gameobject);
	void ReleaseObject();
	void ReleaseAllObject();


	/// <summary>
	/// 수제 Json 저장 함수 정의
	/// </summary>
public:
	_DLL void Save();
	_DLL virtual void Load();

	void WriteSceneData(const std::string& filepath);

protected:
	std::shared_ptr<Scene_Save> m_SaveData;
};

struct Scene_Save
{
	unsigned int m_SceneId;
	std::string m_SceneName;
	bool m_bPhysicsScene;
	std::string m_CurrentCamreaName;
};
