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

/// ���״�� ����, Ŭ���� ��κ��� Update()�� ���⼭ UpdateAll() ����
/// ���� RenderAll()�� ����
/// DLL�� �����Ǵ� �Լ��鵵 ��κ� ���⼭ ����

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

	void ActiveImgui(bool use);	//������ �ٲ� ���� ����.
	void ActiveAudio(bool use); //������ �ٲ� ���� ����.

	void AddScene(std::string name, SceneBase* pScene);
	void SelectScene(std::string name);

	void Finalize();

	// ���÷��� ���κ� ���� �����Ѵ�
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

	// �Ŵ������� Managers���� �޾ƾ���
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

	bool m_isClicked;						// ��ŷ�� ���� ����
	unsigned int m_ClickedObjectID = 0;		// Ŭ������ �ʾ������� 0

	GameObject* m_SelectedObject;

	// �Ź� Single Instance ȣ���ؼ� �������� ������ �̸� �޾Ƶ�
	NavMeshManager* m_pNavMeshManager;

	unsigned int m_Frequency;

	std::string m_pickedSceneNmae;
};

