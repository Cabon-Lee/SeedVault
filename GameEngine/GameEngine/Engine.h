#pragma once

#include <thread>

class SceneBase;

class SceneManager;
class ObjectManager;
class CameraManager;

class Timer;
class Camera;
class Input;

class GameObject;

__interface IRenderer;
__interface IResourceManager;
__interface IAudioSystem;
struct IRenderOption;

class InterfaceManager;
class ResourceLoader;
class ComponentSystem;

class MeshFilter;
class MaterialData;
class ImguiClass;

class PhysicsEngine;

/// 말그대로 엔진, 클래스 대부분의 Update()를 여기서 UpdateAll() 해줌
/// 이후 RenderAll()을 해줌
/// DLL로 제공되는 함수들도 대부분 여기서 관리

class Engine
{
public:
	Engine();
	~Engine();

	void Initialize();
	void Initialize(int hWND, int width, int height);

	void LoadResource(bool& isLoadDone);
	void ShowLoadingScene(bool& isLoadDone);

	void Loop();
	void PhysicsUpdateAll();
	void UpdateInput(float dTime);
	void UpdateAll(float dTime);
	void RenderAll();

	void ActiveImgui(bool use);	//실행중 바꿀 수는 없다.
	void ActiveAudio(bool use); //실행중 바꿀 수는 없다.

	void AddScene(std::string name, SceneBase* pScene);
	void SelectScene(std::string name);

	void Finalize();

	// 리플렉션 프로브 등을 리셋한다
	void ResetRenderer();

	Input* GetInput();
	void InputUpdate(float dTime);
	void DeferredPicking();

	void OnResize(int width, int height);

	LRESULT ImGuiHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND& GetWindowHandle() ;

    //void OnMouseDown(HWND hWnd, int x, int y);
	//void OnMouseUp();
	//void OnMouseMove(int btnState, int x, int y);

	std::shared_ptr<IRenderer> GetIRenderer() const;
	IRenderOption* GetRenderOption();
	ResourceLoader* GetResoruceLoader();
	struct DeferredInfo* GetDeferredInfoFromEngine();

	std::shared_ptr<IResourceManager> GetResourceManager() const;

	bool GetIsResize();
	
	std::string& GetPickedScene();
	void SetPickedScene(std::string sceneName);

private:
	//std::thread m_t1;

	InterfaceManager* m_pInterfaces;

	std::shared_ptr<IRenderer> m_pRenderer;
	IRenderOption* m_RenderOption;
	struct DeferredInfo* m_pDeferredInfo;

	std::shared_ptr<IResourceManager> m_pResourceManager;
	ResourceLoader* m_pResourceLoader;

	std::shared_ptr<IAudioSystem> m_pAudioSystem;

	HWND m_hWnd;

	// 매니저들은 Managers에서 받아쓰자
	ImguiClass* m_EngineImgui;
	ComponentSystem* m_pComponent;

	Input* m_pInput;
	POINT m_CurrMousePos;
	POINT m_PrevMousePos;

	int m_ClientWidth;
	int m_ClientHeight;

	bool m_isResize = false;

	bool m_bImguiActive;
	bool m_bAudioActive;

	bool m_isClicked;						// 피킹을 위한 변수
	unsigned int m_ClickedObjectID = 0;		// 클릭되지 않았을때는 0

	GameObject* m_SelectedObject;

	// 매번 Single Instance 호출해서 가져오면 느려서 미리 받아둠
	NavMeshManager* m_pNavMeshManager;

	unsigned int m_Frequency;

	std::string m_pickedSceneNmae;
};

