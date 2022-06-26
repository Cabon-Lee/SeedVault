#include "pch.h"
#include "EngineDLL.h"
#include "ResourceLoader.h"
#include "IRenderer.h"
#include "JsonLoader_GE.h"
#include "AudioEngineDLL.h"

static Engine* g_Engine = nullptr;

Timer* g_Timer = Timer::GetInstance();
Managers* g_Managers = Managers::GetInstance();


namespace DLLTime
{
	_DLL float DeltaTime()
	{
		return g_Timer->DeltaTime();
		//return g_Timer->_test;
	}

	_DLL float TotalTime()
	{
		return g_Timer->TotalTime();
	}

	_DLL void Reset()
	{
		g_Timer->Reset();
	}

	_DLL void Start()
	{
		g_Timer->Start();
	}

	_DLL void Stop()
	{
		g_Timer->Stop();
	}

	_DLL void Tick()
	{
		g_Timer->Tick();
	}

	_DLL void SetTick(double delta)
	{
		g_Timer->SetDeltaTime(delta);
	}

}

namespace DLLEngine
{
	_DLL void CreateEngine()
	{
		if (g_Engine == nullptr)
		{
			g_Engine = new Engine();
		}
	}

	_DLL void Initialize(int hWND, int width, int hight)
	{
		g_Engine->Initialize(hWND, width, hight);
	}

	_DLL void LoadResource2(bool* isLoadDone)
	{
		g_Engine->LoadResource(*isLoadDone);
	}

	_DLL void ShowLoadingScene(bool& isLoadDone)
	{
		g_Engine->ShowLoadingScene(isLoadDone);
	}

	_DLL void Loop()
	{
		g_Engine->Loop();
	}

	_DLL void AddScene(const char* name, SceneBase* pScene)
	{
		g_Engine->AddScene(std::string(name), pScene);
	}

	_DLL void SelectScene(const char* name)
	{
		g_Engine->SelectScene(std::string(name));
	}

	_DLL void PickScene(const char* name)
	{
		g_Engine->SetPickedScene(std::string(name));
	}

	_DLL void SceneChange()
	{
		if (g_Engine->GetPickedScene().size() > 0)
		{
			g_Engine->SelectScene(g_Engine->GetPickedScene());
			g_Engine->GetPickedScene().erase();
		}
	}

	_DLL void OnResize(int width, int height)
	{
		if (g_Engine != nullptr)
		{
			g_Engine->OnResize(width, height);
		}
	}

	_DLL GameObject* CreateObject(SceneBase* scene, bool isContinual)
	{
		/// <summary>
		/// Create하는 Scene의 Object_V에 넣어주기까지 한다.
		/// </summary>

		//여기서 오브젝트 매니저에 넣어준다
		GameObject* _object = scene->m_pGameObjectManager->CreateObject(isContinual);

		/// <summary>
		/// Transform컴포넌트를 기본으로 삽입하는 곳
		/// </summary>
		/// <returns></returns>
		Transform* _transformC = new Transform();
		_object->m_Transform = _transformC;
		_object->AddComponent<Transform>(_transformC);

		return _object;
	}

	_DLL GameObject* CreateObject_NoTransform(SceneBase* scene, bool isContinual)
	{
		/// <summary>
		/// Create하는 Scene의 Object_V에 넣어주기까지 한다.
		/// </summary>
		/// 
		//여기서 오브젝트 매니저에 넣어준다
		GameObject* _object = scene->m_pGameObjectManager->CreateObject(isContinual); //g_Managers->GetObjectManager()->CreateObject(isContinual);
		return _object;
	}

	_DLL void ResetAllObject(SceneBase* scene)
	{
		scene->m_pGameObjectManager->ResetAllObject();
	}

	_DLL void SetNowCamera(const char* name)
	{
		g_Managers->GetCameraManager()->SetNowCamera(std::string(name));
	}

	_DLL Engine* GetEngine()
	{
		return g_Engine;
	}
	_DLL Managers* GetManagers()
	{
		return g_Managers;
	}



	_DLL void StartComponent(SceneBase* pScene)
	{
		//pScene->StartComponent();
		g_Managers->GetSceneManager()->GetNowScene()->StartComponent();
	}

	_DLL void RemoveComponent(SceneBase* pScene)
	{
		pScene->ReleaseObject();
	}

	_DLL void SetupParentChild(SceneBase* scene)
	{
		//g_Managers->GetObjectManager()->SetupParentChild();
		scene->SetParentChild();
	}

	///원래 AddPhysicsActor였지만 이름이 겹치는 관계로 좀더 정확한 명칭을 쓰기로 하였습니다.
	_DLL void AddPhysicsActorToPhysEngine(PhysicsActor* actor)	
	{
		g_Managers->GetPhysicsEngine()->AddPhysicsActor(actor);
	}

	_DLL void SetPhysicsTransform(Transform* transform)
	{
		g_Managers->GetPhysicsEngine()->SetPhysicsTransform(transform);
	}

	_DLL void SetTransform(PhysicsActor* actor, const Transform& pos)
	{
		g_Managers->GetPhysicsEngine()->SetTransform(actor, pos);
	}

	_DLL void SetVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel)
	{
		g_Managers->GetPhysicsEngine()->SetVelocity(actor, vel);
	}

	_DLL void SetAngularVelocity(PhysicsActor* actor, const DirectX::SimpleMath::Vector3& vel)
	{
		g_Managers->GetPhysicsEngine()->SetAngularVelocity(actor, vel);
	}

	//_DLL GameObject* RaycastCheckGameObject(DirectX::SimpleMath::Vector3 rayOrigin,
			 //											 DirectX::SimpleMath::Vector3 rayDir, 
			 //											 GeometryBase* geometry, Transform* transform, 
			 //											 float maxDist, int hitFlag)
	//{
	//   return g_Managers->GetPhysicsSystem()->RaycastCheckGameObject(rayOrigin, rayDir, geometry, transform, maxDist, hitFlag);
	//}

	_DLL GameObject* RaycastCheckGameObject(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir, float dist)
	{
		return g_Managers->GetPhysicsEngine()->RaycastCheckGameObject(origin, dir, dist);
	}

	_DLL RaycastHit* CheckRaycastHit(
		const DirectX::SimpleMath::Vector3& origin, 
		const DirectX::SimpleMath::Vector3& dir, float dist)
	{
		return &g_Managers->GetPhysicsEngine()->CheckRaycastHit(origin, dir, dist);
	}

	_DLL void DrawRay(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir, float dist, const DirectX::SimpleMath::Color& color)
	{
		g_Engine->GetIRenderer()->DrawDebugging(origin, dir, dist, color);
	}

	_DLL HRESULT ImGuiHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return g_Engine->ImGuiHandler(hWnd, msg, wParam, lParam);
	}

	_DLL void SetDebugRenderMode(const bool val)
	{
		Managers::GetInstance()->GetComponentSystem()->SetDebugRenderMode(val);
	}

	_DLL void WriteScene(SceneBase* pScene, const char* filepath)
	{
		pScene->WriteSceneData(std::string(filepath));
	}

	//현재 신을 기준으로한다.
	_DLL GameObject* FindGameObjectByName(const char* objectName)
	{
		return Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->FindGameObect(std::string(objectName));
	}

	//현재 신을 기준으로한다.
	_DLL GameObject* FindGameObjectById(const unsigned int& objectId)
	{
		return Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->FindGameObect(objectId);
	}

	//현재 신을 기준으로한다.
	_DLL std::vector<GameObject*>& FindGameObjectByTag(const char* tag)
	{
		return Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->FindGameObjectVByTag(std::string(tag));
	}

	_DLL Scene* CreateScene(JsonLoader_GE* jsonLoader, const char* file, const char* sceneName)
	{
		Scene* tempScene = new Scene(std::string(sceneName));
		tempScene->SetJsonLoader(jsonLoader, std::string(file));

		return tempScene;
	}

	_DLL Scene* CreateNewScene(const char* sceneName)
	{
		return new Scene(sceneName);
	}

	_DLL void SetParsingData(Scene* scene, std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
	{
		scene->SetParsingData(gameobject, component);
	}

	_DLL void SetValue(JsonSerializer* json, Json::Value* value)
	{
		json->SetValue(value);
	}

	_DLL void RegisterComponent(ComponentBase* component)
	{
		Managers::GetInstance()->GetComponentSystem()->RegisterComponent(component->GetComponentType(), component);
	}

	_DLL void RegisterObserver(ResourceObserver* pObserver)
	{
		Managers::GetInstance()->GetComponentSystem()->RegisterObserver(pObserver);
	}

	_DLL void QTRender()
	{
		g_Engine->RenderAll();
	}

	_DLL void QTInput(float dTime)
	{
		g_Engine->InputUpdate(dTime);
	}

	_DLL void QTUpdate(float dTime)
	{
		if (Managers::GetInstance()->GetSceneManager()->GetNowScene()->IsPhysicsScene() == true)
		{
			Managers::GetInstance()->GetPhysicsEngine()->PhysicsLoop();
		}

		g_Engine->PhysicsUpdateAll();
		g_Engine->UpdateAll(dTime);
	}

	_DLL void SetTag(const char* newTag, GameObject* gameobject)
	{
		Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->AddToTagManager(std::string(newTag), gameobject);
	}

	_DLL const char* GetTagList(unsigned int index)
	{
		return Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->GetTagList(index).c_str();
	}

	_DLL unsigned int GetSceneID(SceneBase* scene)
	{
		return scene->GetSceneId();
	}

	_DLL  const char* GetSceneName(SceneBase* scene)
	{
		return scene->GetSceneName().c_str();
	}

	_DLL void SetSceneName(SceneBase* scene, const char* name)
	{
		scene->SetName(std::string(name));
	}

	_DLL bool GetSceneIsPhysics(SceneBase* scene)
	{
		return scene->IsPhysicsScene();
	}

	_DLL void SetSceneIsPhysics(SceneBase* scene, bool isPhysics)
	{
		scene->SetPhysicsScene(isPhysics);
	}

	_DLL SceneBase* GetSceneByIndex(unsigned int sceneIndex)
	{
		return Managers::GetInstance()->GetSceneManager()->GetSceneByIndex(sceneIndex);
	}

	_DLL SceneBase* GetNowScene()
	{
		return Managers::GetInstance()->GetSceneManager()->GetNowScene();
	}

	_DLL void ActiveImgui(bool use)
	{
		g_Engine->ActiveImgui(use);
	}

	_DLL Camera* GetCameraByIndex(unsigned int cameraIndex)
	{
		return Managers::GetInstance()->GetCameraManager()->GetCamera(cameraIndex);
	}

	_DLL const char* GetCameraName(Camera* camera)
	{
		if (camera==nullptr)
		{
			Trace("Camera : Camera Set Fail - NULL");
		}
		return camera->GetName().c_str();
	}

	_DLL Camera* GetNowCamera()
	{
		return Managers::GetInstance()->GetCameraManager()->GetNowCamera();
	}

	_DLL ObjectManager* GetSceneObjectManager(SceneBase* scene)
	{
		return scene->m_pGameObjectManager.get();
	}

	_DLL void DerigisterComponent(ComponentBase* component)
	{
		ComponentSystem::GetInstance()->DeregisterComponent(component->GetComponentType(), component);	//컴포넌트 시스템에서 제외하고

		//오브젝트 내에서 삭제한다.
		std::vector<ComponentBase*>::iterator _compIter = component->GetMyObject()->m_Component_V.begin();
		for (; _compIter != component->GetMyObject()->m_Component_V.end(); _compIter++)
		{
			if (*_compIter == component)
			{
				component->Remove();
				component->GetMyObject()->m_Component_V.erase(_compIter);
				delete(component);
				return;
			}
		}
	}

	_DLL GameObject* GetGameObject(SceneBase* scene, unsigned int idx)
	{
		ObjectManager* _pOM = DLLEngine::GetSceneObjectManager(scene);

		if (_pOM->m_pGameObject_V.empty())
		{
			return nullptr;
		}
		else
		{
			//범위를 벗어나면
			if (idx >= _pOM->m_pGameObject_V.size())
			{
				return nullptr;
			}
			else
			{
				return _pOM->m_pGameObject_V[idx];
			}
		}
	}

	_DLL void EngineFinalize()
	{
		g_Engine->Finalize();
		g_Engine->~Engine();
	}

	_DLL unsigned int GetClickedObjectId()
	{
		//0이 나오는 경우 현재 선택된 오브젝트가 없다는 의미이다.
		return g_Engine->GetIRenderer()->GetClickedObjectId();
	}

	_DLL GameObject* GetClickedObject()
	{
		unsigned int _id = g_Engine->GetIRenderer()->GetClickedObjectId();

		if (_id != 0)
		{
			return DLLEngine::FindGameObjectById(_id);
		}
		else
		{
			return nullptr;
		}
	}
	
	_DLL const DirectX::XMFLOAT4& GetCursorWorldPosition()
	{
		return *g_Engine->GetDeferredInfoFromEngine()->postion;
	}

	_DLL const DirectX::XMFLOAT3& GetCursorWorldNormal()
	{
		return *g_Engine->GetDeferredInfoFromEngine()->normal;
	}

	_DLL const unsigned int GetCursorObjectID()
	{
		return *g_Engine->GetDeferredInfoFromEngine()->objectID;
	}

	_DLL void CursorUpdate()
	{
		g_Engine->DeferredPicking();
	}

	_DLL const char* GetNowSceneName()
	{
		return g_Managers->GetSceneManager()->GetNowScene()->GetSceneName().c_str();
	}

	_DLL void SetListener(const DirectX::SimpleMath::Matrix& veiewMatrix, const DirectX::SimpleMath::Vector3& velocity)
	{
		DLLAudio::SetListener(veiewMatrix, velocity);
	}

	_DLL void SetMasterVolume(float val)
	{
		DLLAudio::SetMasterVolume(val);
	}

	_DLL void SetAllBusVolume(float val)
	{
		DLLAudio::SetAllBusVolume(val);
	}

	_DLL float GetBusVolume(const char* name)
	{
		return DLLAudio::GetBusVolume(name);
	}

	_DLL void SetBusVolume(const char* name, float val)
	{
		DLLAudio::SetBusVolume(name, val);
	}

	_DLL bool GetBusPaused(const char* name)
	{
		return DLLAudio::GetBusPaused(name);
	}

	_DLL void SetBusPaused(const char* name, bool val)
	{
		DLLAudio::SetBusPaused(name, val);
	}
}

namespace DLLInput
{
	_DLL bool InputKey(int vk)
	{
		return g_Engine->GetInput()->GetKey(vk);
	}

	_DLL bool InputKeyDown(int vk)
	{
		return g_Engine->GetInput()->GetKeyDown(vk);
	}

	_DLL bool InputKeyUp(int vk)
	{
		return g_Engine->GetInput()->GetKeyUp(vk);
	}

	_DLL POINT GetMouseClientPos()
	{
		return g_Engine->GetInput()->GetMouseClientPos();
	}

	_DLL POINT GetPrevMouseClientPos()
	{
		return g_Engine->GetInput()->GetPrevMouseClientPos();
	}

}

namespace DLLWindow
{
	_DLL HWND& GetWindowHandle()
	{
		return g_Engine->GetWindowHandle();
	}
	_DLL unsigned int DLLWindow::GetScreenWidth()
	{
		return g_Engine->GetIRenderer()->GetScreenWidth();
	}

	_DLL unsigned int DLLWindow::GetScreenHeight()
	{
		return g_Engine->GetIRenderer()->GetScreenHeight();
	}
	_DLL void MoveCursorToCenter()
	{
		// 윈도우 기준 화면 중앙 좌표
		POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

		ClientToScreen(GetWindowHandle(), &_centerPos);
		SetCursorPos(_centerPos.x, _centerPos.y);	// 윈도우 내부 좌표가아니라 스크린(주 모니터)좌표계임...
	}
}


namespace DLLGameObject
{
	_DLL void DeleteGameObject(SceneBase* scene, GameObject* pObj)
	{
		scene->m_pGameObjectManager->ReleaseObject(pObj);
	}

	_DLL bool AddComponentBase(GameObject* pObj, ComponentBase* comp, const type_info& typeinfo) //, , typeid(T) 
	{
		return pObj->AddComponentBase(comp, typeinfo);
	}

	_DLL unsigned int GetComponentIDByJsonValue(Json::Value& value , const char* name)
	{
		return (value)[name]["m_ComponentId"].asUInt();
	}

	_DLL void JsonLoaderInsertComponentMap(JsonLoader_GE* loader, unsigned int id, ComponentBase* comp)
	{
		loader->JsonLoader_GE::GetComponent_Map()->insert(std::make_pair(id, comp));
	}

	_DLL const char* GetName(GameObject* pObj)
	{
		return pObj->GetObjectName_char();
	}

	_DLL void SetName(GameObject* pObj, const char* name)
	{
		pObj->SetObjectName_char(name);
	}

	_DLL MeshFilter* GetMeshFilter(GameObject* pObj)
	{
		return pObj->GetComponent<MeshFilter>();
	}

	//bool AddMeshFilter(GameObject* pObj, MeshFilter* meshfilter)
	//{
	//	return pObj->AddComponent<MeshFilter>(meshfilter);
	//}

	_DLL bool AddMeshFilter(GameObject* pObj)
	{
		return pObj->AddComponent<MeshFilter>(new MeshFilter());
	}

	_DLL MeshRenderer* GetMeshRenderer(GameObject* pObj)
	{
		return pObj->GetComponent<MeshRenderer>();
	}

	//bool AddMeshRenderer(GameObject* pObj, MeshRenderer* meshrenderer)
	//{
	//	return pObj->AddComponent<MeshRenderer>(meshrenderer);
	//}

	_DLL bool AddMeshRenderer(GameObject* pObj)
	{
		return pObj->AddComponent<MeshRenderer>(new MeshRenderer());
	}

	_DLL bool AddPhysicsActor(GameObject* pObj)
	{
		PhysicsActor* physA = new PhysicsActor({1,1,1}, RigidType::Dynamic);
		bool is_success = pObj->AddComponent<PhysicsActor>(physA);
		//g_Managers->GetPhysicsEngine()->AddPhysicsActor(physA);

		return is_success;
	}

	_DLL PhysicsActor* GetPhysicsActor(GameObject* pObj)
	{
		return pObj->GetComponent<PhysicsActor>();
	}

	_DLL bool AddCamera(GameObject* pObj, const char* name)
	{
		return pObj->AddComponent<Camera>(new Camera(std::string(name)));
	}

	_DLL Camera* GetCamera(GameObject* pObj)
	{
		return pObj->GetComponent<Camera>();
	}

	_DLL Light* GetLight(GameObject* pObj)
	{
		return pObj->GetComponent<Light>();
	}

	_DLL bool AddLight(GameObject* pObj)
	{
		return pObj->AddComponent<Light>(new Light());
	}

	_DLL ECameraController* GetCamraController(GameObject* pObj)
	{
		return pObj->GetComponent<ECameraController>();
	}

	_DLL bool AddCamraController(GameObject* pObj)
	{
		return pObj->AddComponent<ECameraController>(new ECameraController());
	}

	_DLL ReflectionProbeCamera* GetReflectionProbeCam(GameObject* pObj)
	{
		return pObj->GetComponent<ReflectionProbeCamera>();
	}

	_DLL bool AddReflectionProbeCam(GameObject* pObj)
	{
		return pObj->AddComponent<ReflectionProbeCamera>(new ReflectionProbeCamera());
	}

	_DLL const char* GetTag(GameObject* pObj)
	{
		return pObj->GetTag().c_str();
	}

}

namespace DLLNavMesh
{
	_DLL bool AddNavMeshByResourceName(std::string name, std::string resourceName)
	{
		if (Managers::GetInstance()->GetNavMeshManager()->AddNavMesh(name, resourceName))
		{
			return true;
		}

		return false;
	}

	_DLL const char* DLLNavMesh::GetCurrentnavMeshName()
	{
		return Managers::GetInstance()->GetNavMeshManager()->GetCurrentNavMeshName().c_str();
	}

	_DLL bool DLLNavMesh::SetCurrentNavMeshByName(std::string name)
	{
		return Managers::GetInstance()->GetNavMeshManager()->SetCurrentNavMesh(name);
	}

	_DLL bool DLLNavMesh::SetCurrentNavMeshByIndex(unsigned int index)
	{
		return  Managers::GetInstance()->GetNavMeshManager()->SetCurrentNavMesh(index);
	}
	_DLL bool GetDebugMode()
	{
		return Managers::GetInstance()->GetNavMeshManager()->GetDebugMode();
	}
	_DLL void SetDebugMode(const bool mode)
	{
		Managers::GetInstance()->GetNavMeshManager()->SetDebugMode(mode);
	}
	_DLL void ToggleDebugMode()
	{
		Managers::GetInstance()->GetNavMeshManager()->ToggleDebugMode();
	}
}