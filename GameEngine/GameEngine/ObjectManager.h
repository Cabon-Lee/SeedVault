#pragma once
#include <vector>
#include "GameObject.h"

//�ӽ� �Ŵ����� �����ڴ� ���� �˾Ƽ� �ϰ� �д�
//�ź��� �Ŵ����� �δ� �� ���� �̱����� ������ �����Ͱ���

class GameObject;
class Camera;

class ComponentSystem;

class ObjectManager
{
public:
	_DLL ObjectManager();
	_DLL ~ObjectManager();

	GameObject* CreateObject(bool isContinual);

	//void StartDisContinualObject();
	void UpdateAllObject(float dTime);
	void ReleaseAllObject();
	void ReleaseObject();
	void ReleaseObject(GameObject* object);
	void ResetAllObject();
	void SetupParentChild();
	void AddToManager(GameObject* object);

private:
	void AddCameraToManager(GameObject* cameraObject);
	
public:
	GameObject* FindGameObect(const std::string& objectName);
	GameObject* FindGameObect(const unsigned int& objectId);
	GameObject* FindGameObectBinary(const unsigned int& objectId);

	std::vector<GameObject*> m_pGameObject_V;
	static std::vector<GameObject*> m_pContinualGameObject_V;				//���������ʰ� ���� �Ǵ� Object
public:
	void SetNowCamera();
private:
	std::vector< Camera*> m_Camera_V;
	Camera* m_pNowCamera;

private:
	std::map<std::string, std::vector<GameObject*>> m_TagObject_M;
public:
	std::vector<GameObject*>& FindGameObjectVByTag(std::string tag);
	void AddToTagManager(const std::string& newTag, GameObject* object);

	
	friend class GameObject;
};

