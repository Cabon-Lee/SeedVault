#pragma once
#include "DLLDefine.h"

#include "GameObject.h"
#include "ObjectManager.h"
#include "Transform.h"
#include "SceneBase.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Light.h"
#include "CameraManager.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "ReflectionProbeCamera.h"
#include "Input.h"
#include "AnimationSystem.h"
#include "Raycast.h"
#include "PhysicsActor.h"

#include "Singleton.h"
#include "Timer.h"
#include "Managers.h"
#include "Engine.h"
#include <string>
#include "ECameraController.h"

class ResourceObserver;
class JsonLoader_GE;

/// Client로 제공되는 DLL함수들 집합소

namespace DLLTime
{
	extern "C"
	{
		_DLL float DeltaTime();
		_DLL float TotalTime();
		_DLL void Reset();
		_DLL void Start();
		_DLL void Stop();
		_DLL void Tick();

		_DLL void SetTick(double delta);
	}
}

namespace DLLEngine
{
	extern "C"
	{
		_DLL void CreateEngine();
		_DLL void ActiveImgui(bool use);
		_DLL void Initialize(int hWND, int width, int hight);
		_DLL void LoadResource2(bool* isLoadDone);
		_DLL void ShowLoadingScene(bool& isLoadDone);
		_DLL void Loop();

		_DLL void QTRender();
		_DLL void QTInput(float dTime);
		_DLL void QTUpdate(float dTime);

		_DLL void StartComponent(SceneBase* pScene);
		_DLL void RemoveComponent(SceneBase* pScene);
		_DLL void DerigisterComponent(ComponentBase* component);
		_DLL void AddScene(const char* name, SceneBase* pScene);
		_DLL void SelectScene(const char* name);
		_DLL void SelectScene(const char* name);
		_DLL void PickScene(const char* name);
		_DLL void SceneChange();
		_DLL void OnResize(int width, int height);
		_DLL void WriteScene(SceneBase* pScene, const char* filepath);

		_DLL unsigned int GetClickedObjectId();
		_DLL GameObject* GetClickedObject();
		/// <summary>

		/// for QT
		/// </summary>
		/// <param name="scene"></param>
		/// <returns></returns>
		_DLL unsigned int GetSceneID(SceneBase* scene);
		_DLL SceneBase* GetNowScene();
		_DLL const char* GetNowSceneName();
		_DLL const char* GetSceneName(SceneBase* scene);
		_DLL void SetSceneName(SceneBase* scene, const char* name);
		_DLL ObjectManager* GetSceneObjectManager(SceneBase* scene);
		_DLL bool GetSceneIsPhysics(SceneBase* scene);
		_DLL void SetSceneIsPhysics(SceneBase* scene, bool isPhysics);



		_DLL SceneBase* GetSceneByIndex(unsigned int sceneIndex);
		_DLL Camera* GetCameraByIndex(unsigned int cameraIndex);
		_DLL const char* GetCameraName(Camera* camera);

		/// <summary>

		_DLL Scene* CreateScene(JsonLoader_GE* jsonLoader, const char* file, const char* sceneName);
		_DLL Scene* CreateNewScene(const char* sceneName);
		_DLL void SetParsingData(Scene* scene, std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);
		_DLL void SetValue(JsonSerializer* json, Json::Value* value);

		_DLL GameObject* CreateObject(SceneBase* scene, bool isContinual = false);
		_DLL GameObject* CreateObject_NoTransform(SceneBase* scene, bool isContinual = false);
		_DLL void SetTag(const char* newTag, GameObject* gameobject);
		_DLL const char* GetTagList(unsigned int index);
		_DLL GameObject* FindGameObjectByName(const char* objectName);
		_DLL GameObject* FindGameObjectById(const unsigned int& objectId);
		_DLL std::vector<GameObject*>& FindGameObjectByTag(const char* tag);
		_DLL GameObject* GetGameObject(SceneBase* scene, unsigned int idx);
		_DLL void ResetAllObject(SceneBase* scene);
		_DLL void SetNowCamera(const char* name);
		_DLL Camera* GetNowCamera();
		_DLL void RegisterComponent(ComponentBase* component);
		_DLL void RegisterObserver(ResourceObserver* pObserver);
		_DLL void SetupParentChild(SceneBase* scene);

		_DLL void AddPhysicsActorToPhysEngine(PhysicsActor* actor);      ///원래 AddPhysicsActor였지만 이름이 겹치는 관계로 좀더 정확한 명칭을 쓰기로 하였습니다.
		_DLL void SetPhysicsTransform(Transform* transform);
		_DLL void SetTransform(PhysicsActor* actor, const Transform& pos);
		_DLL void SetVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel);
		_DLL void SetAngularVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel);
		//_DLL GameObject* RaycastCheckGameObject(DirectX::SimpleMath::Vector3 rayOrigin,
		//                                 DirectX::SimpleMath::Vector3 rayDir,
		//                                 GeometryBase* geometry, Transform* transform,
		//                                 float maxDist, int hitFlag);
		_DLL GameObject* RaycastCheckGameObject(const DirectX::SimpleMath::Vector3& origin,
			const DirectX::SimpleMath::Vector3& dir, float dist);

		_DLL RaycastHit* CheckRaycastHit(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir, float dist);
		_DLL void DrawRay(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir, float dist, const DirectX::SimpleMath::Color& color = { 255.0f, 0.0f, 0.0f });    // YoKing : duration 까지 추가되면 좋겠다.

		_DLL void CursorUpdate();
		_DLL const DirectX::XMFLOAT4& GetCursorWorldPosition();
		_DLL const DirectX::XMFLOAT3& GetCursorWorldNormal();
		_DLL const unsigned int GetCursorObjectID();

		_DLL Engine* GetEngine();
		_DLL HRESULT ImGuiHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		_DLL void SetDebugRenderMode(const bool val);

		_DLL void EngineFinalize();


		_DLL void SetListener(const DirectX::SimpleMath::Matrix& veiewMatrix, const DirectX::SimpleMath::Vector3& velocity);
		_DLL void SetMasterVolume(float val);
		_DLL void SetAllBusVolume(float val);
		_DLL float GetBusVolume(const char* name);
		_DLL void SetBusVolume(const char* name, float val);
		_DLL bool GetBusPaused(const char* name);
		_DLL void SetBusPaused(const char* name, bool val);

	}
}

namespace DLLInput
{
	extern "C"
	{
		_DLL bool InputKey(int vk);
		_DLL bool InputKeyDown(int vk);
		_DLL bool InputKeyUp(int vk);
		_DLL POINT GetMouseClientPos();
		_DLL POINT GetPrevMouseClientPos();

	}
}

namespace DLLWindow
{
	extern "C"
	{
		_DLL HWND& GetWindowHandle();
		_DLL unsigned int GetScreenWidth();
		_DLL unsigned int GetScreenHeight();

		_DLL void MoveCursorToCenter();
	}
}

namespace DLLGameObject
{
	extern "C"
	{
		_DLL void DeleteGameObject(SceneBase* scene, GameObject* pObj);

		_DLL bool AddComponentBase(GameObject* pObj, ComponentBase* comp, const type_info& typeinfo);

		_DLL unsigned int GetComponentIDByJsonValue(Json::Value& value, const char* name);

		_DLL void JsonLoaderInsertComponentMap(JsonLoader_GE* loader, unsigned int id, ComponentBase* comp);

		_DLL const char* GetName(GameObject* pObj);
		_DLL void SetName(GameObject* pObj, const char* name);
		_DLL const char* GetTag(GameObject* pObj);

		_DLL MeshFilter* GetMeshFilter(GameObject* pObj);
		_DLL bool AddMeshFilter(GameObject* pObj);

		_DLL MeshRenderer* GetMeshRenderer(GameObject* pObj);
		_DLL bool AddMeshRenderer(GameObject* pObj);

		_DLL PhysicsActor* GetPhysicsActor(GameObject* pObj);
		_DLL bool AddPhysicsActor(GameObject* pObj);

		_DLL Camera* GetCamera(GameObject* pObj);
		_DLL bool AddCamera(GameObject* pObj, const char* name);

		_DLL Light* GetLight(GameObject* pObj);
		_DLL bool AddLight(GameObject* pObj);

		_DLL ReflectionProbeCamera* GetReflectionProbeCam(GameObject* pObj);
		_DLL bool AddReflectionProbeCam(GameObject* pObj);

		_DLL ECameraController* GetCamraController(GameObject* pObj);
		_DLL bool AddCamraController(GameObject* pObj);
	}

}

namespace DLLNavMesh
{
	extern "C"
	{
		_DLL bool AddNavMeshByResourceName(std::string name, std::string resourceName);

		_DLL const char* GetCurrentnavMeshName();

		_DLL bool SetCurrentNavMeshByName(std::string name);
		_DLL bool SetCurrentNavMeshByIndex(unsigned int index);

		_DLL bool GetDebugMode();
		_DLL void SetDebugMode(const bool mode);
		_DLL void ToggleDebugMode();
	}
}