#pragma once
#include <windows.h>

__interface IRenderer;
class Transform;
class MeshRenderer;
class MeshFilter;
class MaterialComponent;
class Camera;
class Light;
class SkinningAnimation;
class ReflectionProbeCamera;
class NaviMeshController;
class GameObject;

class ImguiClass
{
public:
	ImguiClass();
	~ImguiClass();

public:
	void Initialize(HWND hWnd , IRenderer* renderer);
	void Update();
	void Render();

private:
	void Imgui_ObjectInfo();
	void Imgui_SceneController();
	void Imgui_RendererController();
	void Imgui_RendererDebugInfo();
	GameObject* m_SelectedObject;

private:
	void Imgui_Transform(Transform* transform);
	void Imgui_MeshRenderer(MeshRenderer* meshRenderer);
	void Imgui_MeshFilter(MeshFilter* meshFilter);
	void Imgui_SkinningAnimation(SkinningAnimation* skinningAni);
	void Imgui_Light(Light* light);
	void Imgui_Material(MaterialComponent* material);
	void Imgui_Camera(Camera* camera);
	void Imgui_ReflectionProbeCamera(ReflectionProbeCamera* reflectionProbeCam);
	void Imgui_NaviMeshController(NaviMeshController* naviMeshController);
	void Imgui_ParticleSpawner(class ParticleSpawner* pParticleSpawner);
};

