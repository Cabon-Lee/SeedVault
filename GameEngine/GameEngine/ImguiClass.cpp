#include "pch.h"
#include "EngineDLL.h"
#include "ImguiClass.h"
#include "IRenderer.h"
#include "Managers.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "SceneBase.h"
#include "Transform.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "SkinningAnimation.h"
#include "MaterialComponent.h"
#include "Light.h"
#include "Camera.h"
#include "ReflectionProbeCamera.h"
#include "NaviMeshController.h"
#include "ParticleSystem.h"

#include "Imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

ImguiClass::ImguiClass()
{
}

ImguiClass::~ImguiClass()
{
}

void ImguiClass::Initialize(HWND hWnd, IRenderer* renderer)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(hWnd);
	Void_ImGui_ImplDX11_Init(renderer->GetDeviceVoid(), renderer->GetDeviceContextVoid());

	ImGui::StyleColorsLight();
}

void ImguiClass::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//const ImVec4 _colorB = ImVec4(200.0f / 255.0f, 14.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
	//ImGui::PushStyleColor(ImGuiCol_Text, _colorB);
	//ImGui::PopStyleColor();
	/// <summary>
	Imgui_ObjectInfo();
	Imgui_SceneController();
	Imgui_RendererController();
	Imgui_RendererDebugInfo();

	/// </summary>

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImguiClass::Imgui_ObjectInfo()
{
	const ImVec4 _colorB = ImVec4(14.0f / 255.0f, 14.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
	const ImVec4 _colorW = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);
	ImGui::Begin("Object Information");

	int _index = 0;
	for (GameObject*& _go : Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->m_pGameObject_V)
	{
		std::string _gameobject = _go->GetObjectName() + "###gameobject" + std::to_string(_go->GetGameObjectId());
		std::string _enable = "Enable###enable" + std::to_string(_go->GetGameObjectId());
		bool _objcectEnable = _go->GetIsEnabled();
		//기본값이 Open인 CollapsingHeader 클릭하지 않아도 이미 열려있다.
		if (ImGui::CollapsingHeader(_gameobject.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			std::string _objectId = "Object Id : " + std::to_string(_go->GetGameObjectId());
			ImGui::Text("Component Count : ");
			ImGui::SameLine();

			ImGui::Text(std::to_string(_go->GetComponentCnt()).c_str());
			ImGui::TextColored(_objcectEnable ? _colorB : _colorW, _objectId.c_str());
			ImGui::SameLine(200.f);
			if (ImGui::Checkbox(_enable.c_str(), &_objcectEnable))
			{
				if (_objcectEnable)
				{
					_go->OnEnable();
				}
				else
				{
					_go->OnDisable();
				}
			}

			if (_go->GetIsEnabled() == false)
			{
				continue;
			}


			ImGui::Separator();


			ParticleSpawner* _thisParticleSpawer = _go->GetComponent<ParticleSpawner>();
			Imgui_ParticleSpawner(_thisParticleSpawer);


			/// <summary>
			/// ReflectionProbe
			/// </summary>
			NaviMeshController* _thisNaviMesh = _go->GetComponent<NaviMeshController>();
			Imgui_NaviMeshController(_thisNaviMesh);

			/// <summary>
			/// ReflectionProbe
			/// </summary>
			ReflectionProbeCamera* _thisReflection = _go->GetComponent<ReflectionProbeCamera>();
			Imgui_ReflectionProbeCamera(_thisReflection);

			/// <summary>
			/// Camera
			/// </summary>
			Camera* _thisCam = _go->GetComponent<Camera>();
			Imgui_Camera(_thisCam);

			/// <summary>
			/// Transform
			/// </summary>
			Transform* _thisTr = _go->GetComponent<Transform>();
			Imgui_Transform(_thisTr);

			/// <summary>
			/// MeshRenderer
			/// </summary>
			MeshRenderer* _thisMR = _go->GetComponent<MeshRenderer>();
			Imgui_MeshRenderer(_thisMR);

			/// <summary>
			/// MeshFilter
			/// </summary>
			MeshFilter* _thisMF = _go->GetComponent<MeshFilter>();
			Imgui_MeshFilter(_thisMF);

			/// <summary>
			/// SkinningAnimation
			/// </summary>
			SkinningAnimation* _thisSA = _go->GetComponent<SkinningAnimation>();
			Imgui_SkinningAnimation(_thisSA);

			/// <summary>
			/// Light
			/// </summary>
			Light* _thisLI = _go->GetComponent<Light>();
			Imgui_Light(_thisLI);

			/// <summary>
			/// Material
			/// </summary>
			MaterialComponent* _thisMA = _go->GetComponent<MaterialComponent>();
			Imgui_Material(_thisMA);

		}
	}

	for (GameObject*& _go : Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->m_pContinualGameObject_V)
	{
		std::string _gameobject = _go->GetObjectName() + "###gameobject" + std::to_string(_go->GetGameObjectId());
		std::string _enable = "Enable###enable" + std::to_string(_go->GetGameObjectId());
		bool _objcectEnable = _go->GetIsEnabled();
		//기본값이 Open인 CollapsingHeader 클릭하지 않아도 이미 열려있다.
		if (ImGui::CollapsingHeader(_gameobject.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			std::string _objectId = "Object Id : " + std::to_string(_go->GetGameObjectId());
			ImGui::Text("Component Count : ");
			ImGui::SameLine();

			ImGui::Text(std::to_string(_go->GetComponentCnt()).c_str());
			ImGui::TextColored(_objcectEnable ? _colorB : _colorW, _objectId.c_str());
			ImGui::SameLine(200.f);
			if (ImGui::Checkbox(_enable.c_str(), &_objcectEnable))
			{
				if (_objcectEnable)
				{
					_go->OnEnable();
				}
				else
				{
					_go->OnDisable();
				}
			}

			if (_go->GetIsEnabled() == false)
			{
				continue;
			}


			ImGui::Separator();


			/// <summary>
			/// ReflectionProbe
			/// </summary>
			ReflectionProbeCamera* _thisReflection = _go->GetComponent<ReflectionProbeCamera>();
			Imgui_ReflectionProbeCamera(_thisReflection);

			/// <summary>
			/// Camera
			/// </summary>
			Camera* _thisCam = _go->GetComponent<Camera>();
			Imgui_Camera(_thisCam);

			/// <summary>
			/// Transform
			/// </summary>
			Transform* _thisTr = _go->GetComponent<Transform>();
			Imgui_Transform(_thisTr);

			/// <summary>
			/// MeshRenderer
			/// </summary>
			MeshRenderer* _thisMR = _go->GetComponent<MeshRenderer>();
			Imgui_MeshRenderer(_thisMR);

			/// <summary>
			/// MeshFilter
			/// </summary>
			MeshFilter* _thisMF = _go->GetComponent<MeshFilter>();
			Imgui_MeshFilter(_thisMF);

			/// <summary>
			/// SkinningAnimation
			/// </summary>
			SkinningAnimation* _thisSA = _go->GetComponent<SkinningAnimation>();
			Imgui_SkinningAnimation(_thisSA);

			/// <summary>
			/// Light
			/// </summary>
			Light* _thisLI = _go->GetComponent<Light>();
			Imgui_Light(_thisLI);

			/// <summary>
			/// Material
			/// </summary>
			MaterialComponent* _thisMA = _go->GetComponent<MaterialComponent>();
			Imgui_Material(_thisMA);

		}
	}
	ImGui::End();
}

void ImguiClass::Imgui_SceneController()
{
	/// <summary>
	/// 신전환 조작
	/// </summary>
	ImGui::Begin("Scene Controller");
	SceneManager* _sceneManager = Managers::GetInstance()->GetSceneManager();


	//ImGui::BulletText(std::to_string(_sceneManager->GetNowScene()->GetSceneId()).c_str());
	for (unsigned int _scenecnt = 0; _scenecnt < _sceneManager->GetSceneCnt(); _scenecnt++)
	{
		std::string _scenename;
		//Scene이름이 비었으면 SceneId로 대체한다.
		if (_sceneManager->GetSceneByIndex(_scenecnt)->GetSceneName() == "")
		{
			_scenename = std::to_string(_sceneManager->GetSceneByIndex(_scenecnt)->GetSceneId());
		}
		else
		{
			//_scenename = _sceneManager->GetSceneByIndex(_scenecnt)->GetSceneName();
			_scenename = _sceneManager->GetSceneUMKey(_scenecnt);
		}

		"###SC" + std::to_string(_sceneManager->GetSceneByIndex(_scenecnt)->GetSceneId());
		bool _isNowScene = _sceneManager->GetSceneByIndex(_scenecnt)
			== _sceneManager->GetNowScene();

		if (ImGui::RadioButton(_scenename.c_str(), _isNowScene))
		{
			_sceneManager->SetNowScene(_scenecnt);
			m_SelectedObject = nullptr;
		}
	}

	ImGui::Separator();

	//ImGui::Text("...Comming Soon...");
	//if (ImGui::Button("Reset"))
	//{
	//	Managers::GetInstance()->GetObjectManager()->ResetAllObject();
	//	Managers::GetInstance()->GetSceneManager()->GetNowScene()->Reset();
	//}
	//ImGui::SameLine();

	if (ImGui::Button("Save"))
	{
#if DEBUG | _DEBUG
		Managers::GetInstance()->GetSceneManager()->GetNowScene()->WriteSceneData("../../SeedVault/Data/Scene");
#else
		Managers::GetInstance()->GetSceneManager()->GetNowScene()->WriteSceneData("../Data/Scene");
#endif // 0


	}

	if (Managers::GetInstance()->GetComponentSystem()->GetIsPaused())
	{
		if (ImGui::Button("Play"))
		{
			Managers::GetInstance()->GetComponentSystem()->Update_Restrat();
		}
	}
	else
	{
		if (ImGui::Button("Pause"))
		{
			Managers::GetInstance()->GetComponentSystem()->Update_Pause();
		}
	}
	static int num = 0;
	if (ImGui::Button("Add Object"))
	{
		GameObject* _GE = DLLEngine::CreateObject(Managers::GetInstance()->GetSceneManager()->GetNowScene(), false);
		_GE->SetObjectName(std::to_string(num++));
		_GE->AddComponent<MeshFilter>(new MeshFilter());
		_GE->AddComponent<MeshRenderer>(new MeshRenderer());
	}

	if (m_SelectedObject == nullptr)
	{
		m_SelectedObject = Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->m_pGameObject_V[0];
	}

	if (ImGui::BeginCombo("###ObejectDelete", m_SelectedObject->GetObjectName().c_str()))
	{
		for (int _objIndex = 0; _objIndex < Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->m_pGameObject_V.size(); _objIndex++)
		{
			const bool _isSelected = m_SelectedObject == Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->m_pGameObject_V[_objIndex];

			//선택한 매쉬로 매쉬필터를 바꿔준다.
			if (ImGui::Selectable(Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->m_pGameObject_V[_objIndex]->GetObjectName().c_str()))
			{
				m_SelectedObject = Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->m_pGameObject_V[_objIndex];
			}

			if (_isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		Managers::GetInstance()->GetSceneManager()->GetNowScene()->m_pGameObjectManager->ReleaseObject(m_SelectedObject);
		m_SelectedObject = nullptr;
	}

	ImGui::Separator();

	/// <summary>
	/// 카메라 변경
	/// </summary>
	ImGui::Text("Camera List");
	std::string _nowCam = Managers::GetInstance()->GetCameraManager()->GetNowCamera() == nullptr ?
		"No Camera" : Managers::GetInstance()->GetCameraManager()->GetNowCamera()->GetName().c_str();
	if (ImGui::BeginCombo("###camMananger", _nowCam.c_str()))
	{
		for (int _cameraIndex = 0; _cameraIndex < Managers::GetInstance()->GetCameraManager()->GetCameraCount(); _cameraIndex++)
		{
			const bool _isSelected = Managers::GetInstance()->GetCameraManager()->GetNowCamera() == Managers::GetInstance()->GetCameraManager()->GetCamera(_cameraIndex);

			//선택한 매쉬로 매쉬필터를 바꿔준다.
			if (ImGui::Selectable(Managers::GetInstance()->GetCameraManager()->GetCamera(_cameraIndex)->GetName().c_str()))
			{
				Managers::GetInstance()->GetCameraManager()->SetNowCamera(_cameraIndex);
			}

			if (_isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::End();
}

// 디버그, 포스트 프로세싱 등을 컨트롤하기 위한 함수
void ImguiClass::Imgui_RendererController()
{
	ImGui::Begin("Renderer Controller");
	std::shared_ptr<IRenderer> _nowRenderer = DLLEngine::GetEngine()->GetIRenderer();

	IRenderOption* _nowOption;
	_nowOption = DLLEngine::GetEngine()->GetRenderOption();

	// HDR 관련
	{
		std::string _deferred = "DEFERRED###deferredDebugCheck";
		ImGui::Checkbox(_deferred.c_str(), &_nowOption->isDeferredDebug);

		std::string _hdr = "HDR###HDRCheck";
		ImGui::Checkbox(_hdr.c_str(), &_nowOption->isHDRRender);

		std::string _adaptation = "ADAPTATION###adapatationCheck";
		ImGui::Checkbox(_adaptation.c_str(), &_nowOption->isAdaptation);

		std::string _TexCombo[3];
		_TexCombo[0] = "HDR_WIHTE###withe";
		_TexCombo[1] = "HDR_MIDDLE_GREY###middleGrey";
		_TexCombo[2] = "ADAPTATION###adaptation";

		ImGui::SliderFloat(_TexCombo[0].c_str(), &_nowOption->hdrWhite, 0.0f, 10.0f, "%.1f");
		ImGui::SliderFloat(_TexCombo[1].c_str(), &_nowOption->hdrMiddleGrey, 0.0f, 20.0f, "%.1f");
		ImGui::SliderFloat(_TexCombo[2].c_str(), &_nowOption->adaptation, 0.0f, 10.0f, "%.1f");
	}
	ImGui::Separator();

	// 블룸
	{
		std::string _bloom = "BLOOM###bloomCheck";
		ImGui::Checkbox(_bloom.c_str(), &_nowOption->isBloom);

		std::string _bloomFilter = "BLOOM_FILTER###bloomFilterCheck";
		ImGui::Checkbox(_bloomFilter.c_str(), &_nowOption->isBoomFilter);

		std::string _colorAdjust = "COLOR_ADJUST###colorAdjustCheck";
		ImGui::Checkbox(_colorAdjust.c_str(), &_nowOption->isColorAdjust);

		std::string _TexCombo[4];
		_TexCombo[0] = "BLOOM_SCALE###bloomScale";
		_TexCombo[1] = "THRESHOLD###bloomThreshold";
		_TexCombo[2] = "INCREASE###bloomIncrease";
		_TexCombo[3] = "ADJUSTFACTOR###adjustFactor";

		ImGui::SliderInt(_TexCombo[0].c_str(), &_nowOption->bloomScale, 0.0f, 12.0f, "%1.0f");
		ImGui::SliderFloat(_TexCombo[1].c_str(), &_nowOption->bloomThreshold, 0.0f, 20.0f, "%.1f");
		ImGui::SliderFloat(_TexCombo[2].c_str(), &_nowOption->intensity, 0.0f, 10.0f, "%.1f");
		ImGui::SliderFloat(_TexCombo[3].c_str(), &_nowOption->adjustFactor, 0.0f, 1.0f, "%.05f");

	}
	ImGui::Separator();

	// 블러
	{
		std::string _blur = "BLUR###blurCheck";
		ImGui::Checkbox(_blur.c_str(), &_nowOption->isBlur);

		std::string _TexCombo[1];
		_TexCombo[0] = "BLUR_COUNT###blurCount";

		ImGui::SliderInt(_TexCombo[0].c_str(), &_nowOption->blurCount, 1.0f, 12.0f, "%1.0f");
	}
	ImGui::Separator();

	// 샘플링
	{
		std::string _downSample = "DOWNSAMPLE###downsamplingCheck";
		ImGui::Checkbox(_downSample.c_str(), &_nowOption->isDownSampling);

		std::string _TexCombo[1];
		_TexCombo[0] = "SAMPLE###sample";

		ImGui::SliderInt(_TexCombo[0].c_str(), &_nowOption->sampleScale, 1.0f, 6.0f, "%1.0f");
	}


	ImGui::End();
}

void ImguiClass::Imgui_RendererDebugInfo()
{
	ImGui::Begin("Renderer Debug Infomation");
	std::shared_ptr<IRenderer> _nowRenderer = DLLEngine::GetEngine()->GetIRenderer();

	IRendererDebugInfo _nowDebugInfo;
	_nowDebugInfo = DLLEngine::GetEngine()->GetIRenderer()->GetRenderDebugInfo();

	{
		ImGui::Text("Total Obejct : %d", _nowDebugInfo.renderedObject + _nowDebugInfo.culledObject);

		ImGui::Separator();
		ImGui::Text("Rendered Obejct : %d", _nowDebugInfo.renderedObject);
		ImGui::Text("Opaque : %d", _nowDebugInfo.opaqueObejct);
		ImGui::Text("Transparent : %d", _nowDebugInfo.transparentObejct);

		ImGui::Separator();
		ImGui::Text("Culled Obejct : %d", _nowDebugInfo.culledObject);
	}

	ImGui::Separator();
	ImGui::End();
}

void ImguiClass::Imgui_Transform(Transform* transform)
{
	if (transform != nullptr)
	{
		std::string _id = "###tr" + std::to_string(transform->GetComponetId());
		std::string _Transform = "Transfrom" + _id;
		int _index = 0;

		if (ImGui::TreeNode(_Transform.c_str()))
		{
			std::string _transformPx = "10###" + _id + std::to_string(_index++);
			std::string _transformPy = "10###" + _id + std::to_string(_index++);
			std::string _transformPz = "10###" + _id + std::to_string(_index++);

			ImGui::BulletText("Position");
			ImGui::Text("X :");
			ImGui::SameLine();
			if (ImGui::InputFloat(_transformPx.c_str(), &transform->m_Position.x, 1))
			{
				Vector3 _pos = { transform->m_Position.x, transform->m_Position.y, transform->m_Position.z };
				transform->SetPosition(_pos);
			}

			ImGui::Text("Y :");
			ImGui::SameLine();
			if (ImGui::InputFloat(_transformPy.c_str(), &transform->m_Position.y, 1))
			{
				Vector3 _pos = { transform->m_Position.x, transform->m_Position.y, transform->m_Position.z };
				transform->SetPosition(_pos);
			}

			ImGui::Text("Z :");
			ImGui::SameLine();
			if (ImGui::InputFloat(_transformPz.c_str(), &transform->m_Position.z, 1))
			{
				Vector3 _pos = { transform->m_Position.x, transform->m_Position.y, transform->m_Position.z };
				transform->SetPosition(_pos);
			}

			std::string _transformRx = "10###" + _id + std::to_string(_index++);
			std::string _transformRy = "10###" + _id + std::to_string(_index++);
			std::string _transformRz = "10###" + _id + std::to_string(_index++);
			ImGui::BulletText("Rotation");
			ImGui::Text("X :");
			ImGui::SameLine();
			if (ImGui::InputFloat(_transformRx.c_str(), &transform->m_EulerAngles.x, 1))
			{
				transform->SetRotationFromVec({ transform->m_EulerAngles.x, transform->m_EulerAngles.y, transform->m_EulerAngles.z });
			}
			ImGui::Text("Y :");
			ImGui::SameLine();
			if (ImGui::InputFloat(_transformRy.c_str(), &transform->m_EulerAngles.y, 1))
			{
				transform->SetRotationFromVec({ transform->m_EulerAngles.x, transform->m_EulerAngles.y, transform->m_EulerAngles.z });
			}
			ImGui::Text("Z :");
			ImGui::SameLine();
			if (ImGui::InputFloat(_transformRz.c_str(), &transform->m_EulerAngles.z, 1))
			{
				transform->SetRotationFromVec({ transform->m_EulerAngles.x, transform->m_EulerAngles.y, transform->m_EulerAngles.z });
			}
			std::string _transformSx = "###" + _id + std::to_string(_index++);
			std::string _transformSy = "###" + _id + std::to_string(_index++);
			std::string _transformSz = "###" + _id + std::to_string(_index++);
			ImGui::BulletText("Scale");
			ImGui::Text("X :");
			ImGui::SameLine();
			ImGui::InputFloat(_transformSx.c_str(), &transform->m_Scale.x);
			//ImGui::SameLine(325.f);
			//if (ImGui::Button("/", ImVec2(20, 20)))
			//{
			//	transform->m_Scale.x *= 0.5f;
			//}
			//ImGui::SameLine(350.f);
			//if (ImGui::Button("*", ImVec2(20, 20)))
			//{
			//	transform->m_Scale.x *= 2.0f;
			//}
			//ImGui::SameLine(375.f);
			//ImGui::Text("2");
			ImGui::Text("Y :");
			ImGui::SameLine();
			ImGui::InputFloat(_transformSy.c_str(), &transform->m_Scale.y);
			//ImGui::SameLine(325.f);
			//if (ImGui::Button("/", ImVec2(20, 20)))
			//{
			//	transform->m_Scale.y *= 0.5f;
			//}
			//ImGui::SameLine(350.f);
			//if (ImGui::Button("*", ImVec2(20, 20)))
			//{
			//	transform->m_Scale.y *= 2.0f;
			//}
			//ImGui::SameLine(375.f);
			//ImGui::Text("2");
			ImGui::Text("Z :");
			ImGui::SameLine();
			ImGui::InputFloat(_transformSz.c_str(), &transform->m_Scale.z);
			//ImGui::SameLine(325.f);
			//if (ImGui::Button("/", ImVec2(20, 20)))
			//{
			//	transform->m_Scale.z *= 0.5f;
			//}
			//ImGui::SameLine(350.f);
			//if (ImGui::Button("*", ImVec2(20, 20)))
			//{
			//	transform->m_Scale.z *= 2.0f;
			//}
			//ImGui::SameLine(375.f);
			//ImGui::Text("2");
			ImGui::Text("Scale Button : ");
			ImGui::SameLine();
			if (ImGui::Button("/2"))
			{
				transform->m_Scale.x *= 0.5f;
				transform->m_Scale.y *= 0.5f;
				transform->m_Scale.z *= 0.5f;
			}
			ImGui::SameLine();
			if (ImGui::Button("*2"))
			{
				transform->m_Scale.x *= 2.0f;
				transform->m_Scale.y *= 2.0f;
				transform->m_Scale.z *= 2.0f;
			}
			ImGui::SameLine();
			if (ImGui::Button("/10"))
			{
				transform->m_Scale.x *= 0.1f;
				transform->m_Scale.y *= 0.1f;
				transform->m_Scale.z *= 0.1f;
			}
			ImGui::SameLine();
			if (ImGui::Button("*10"))
			{
				transform->m_Scale.x *= 10.0f;
				transform->m_Scale.y *= 10.0f;
				transform->m_Scale.z *= 10.0f;
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}

}

void ImguiClass::Imgui_MeshRenderer(MeshRenderer* meshrenderer)
{
	if (meshrenderer != nullptr)
	{
		std::string _id = "###mr" + std::to_string(meshrenderer->GetComponetId());
		std::string _MeshRenderer = "MeshRenderer" + _id;
		if (ImGui::TreeNode(_MeshRenderer.c_str()))
		{
			//기즈모 On/Off
			bool _gizmob = meshrenderer->GetIsGizmoOn();
			std::string _gizmo = "Gizmo###giz" + std::to_string(meshrenderer->GetComponetId());
			if (ImGui::Checkbox(_gizmo.c_str(), &_gizmob))
			{
				meshrenderer->SetGizmo(_gizmob);
			}

			//와이어프레임 On/Off
			const bool _isWireframe = meshrenderer->GetIsWireframeMode();
			std::string _sol = "Solid###sol" + std::to_string(meshrenderer->GetComponetId());
			std::string _wf = "Wireframe###wf" + std::to_string(meshrenderer->GetComponetId());
			if (ImGui::RadioButton(_sol.c_str(), !_isWireframe))
			{
				meshrenderer->SetRasterizerState(eRasterizerState::SOLID);
			}
			ImGui::SameLine(200.f);
			if (ImGui::RadioButton(_wf.c_str(), _isWireframe))
			{
				meshrenderer->SetRasterizerState(eRasterizerState::WIREFRAME);
			}

			bool _isReflection = false;
			bool _isCastShadow = false;
			unsigned int _flag = meshrenderer->GetRenderFlag();
			_isReflection = (_flag & REFLECTION_FLAG);
			_isCastShadow = (_flag & CASTSHADOW_FLAG);
			
			std::string _reflection = "Reflection###reflection" + std::to_string(meshrenderer->GetComponetId());
			if (ImGui::Checkbox(_reflection.c_str(), &_isReflection))
			{
				meshrenderer->SetReflection(_isReflection);
			}

			std::string _castShadow = "CastShadow###castShadow" + std::to_string(meshrenderer->GetComponetId());
			if (ImGui::Checkbox(_castShadow.c_str(), &_isCastShadow))
			{
				meshrenderer->SetCastShadow(_isCastShadow);
			}


			bool _isOff = meshrenderer->GetIsEnabled();
			std::string _isEnable = "Enable###isEnable" + std::to_string(meshrenderer->GetComponetId());
			if (ImGui::Checkbox(_isEnable.c_str(), &_isOff))
			{
				meshrenderer->SetEnable(_isOff);
			}


			ImGui::TreePop();
		}

		ImGui::Separator();
	}
}

void ImguiClass::Imgui_MeshFilter(MeshFilter* meshfilter)
{
	if (meshfilter != nullptr)
	{
		std::string _id = "###mf" + std::to_string(meshfilter->GetComponetId());
		std::string _MeshFilter = "MeshFilter" + _id;

		if (ImGui::TreeNode(_MeshFilter.c_str()))
		{
			ImGui::Text(meshfilter->GetName().c_str());
			ImGui::SameLine(150.f);

			if (ImGui::BeginCombo(_id.c_str(), meshfilter->GetName().c_str()))
			{
				for (int _meshIndex = 0; _meshIndex < meshfilter->GetMeshCount(); _meshIndex++)
				{
					const bool _isSelected = meshfilter->GetMeshName(_meshIndex) == meshfilter->GetName();
					std::string _meshlist = meshfilter->GetMeshName(_meshIndex) +
						"###mesh" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_meshIndex);

					//선택한 매쉬로 매쉬필터를 바꿔준다.
					if (ImGui::Selectable(_meshlist.c_str()))
					{
						meshfilter->SetIndex(_meshIndex);
					}

					if (_isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			for (int _materialIndex = 0; _materialIndex < meshfilter->GetGraphicMaterialCount(); _materialIndex++)
			{
				std::string _material = "material [" + std::to_string(_materialIndex) + "] " + meshfilter->GetGraphicMaterialName(_materialIndex) + " ###mat" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex);

				if (ImGui::TreeNode(_material.c_str()))
				{
					std::string _TexCombo[11];

					_TexCombo[ALBEDO] = "Albedo###texAlbedo" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex) + std::to_string(ALBEDO);
					_TexCombo[NORMAL] = "Normal###texNormal" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex) + std::to_string(NORMAL);
					_TexCombo[METALROUGH] = "Metal Rough###texMR" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex) + std::to_string(METALROUGH);
					_TexCombo[EMISSIVE] = "Emissive###texEmissive" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex) + std::to_string(EMISSIVE);

					_TexCombo[METALLIC + 4] = "Metallic###texMetallic" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex) + std::to_string(METALLIC);
					_TexCombo[SHINENESS + 4] = "Shineness###texShineness" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex) + std::to_string(SHINENESS);
					_TexCombo[NORMALFACFOR + 4] = "NormalFactor###normalfactor" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex) + std::to_string(NORMALFACFOR);
					_TexCombo[EMISSIVEFACTOR + 4] = "EmissiveFactor###emissviefactor" + std::to_string(meshfilter->GetComponetId()) + std::to_string(_materialIndex) + std::to_string(EMISSIVEFACTOR);

					for (int i = 0; i < EMISSIVE + 1; i++)
					{
						if (ImGui::BeginCombo(_TexCombo[i].c_str(), meshfilter->GetGraphicMaterialTextureName(_materialIndex, i).c_str()))
						{
							std::vector<std::pair<std::string, unsigned int>> _textures = meshfilter->GetTextureNames();

							for (int texIndex = 0; texIndex < _textures.size(); texIndex++)
							{
								const bool _isSelected = meshfilter->GetGraphicMaterialTextureName(_materialIndex, i)
									== _textures[texIndex].first;

								if (ImGui::Selectable(_textures[texIndex].first.c_str()))
								{
									meshfilter->SetGraphicMaterialTextureName(_materialIndex, i, _textures[texIndex].first);
								}

								if (_isSelected)
								{
									ImGui::SetItemDefaultFocus();
								}
							}

							ImGui::EndCombo();
						}
					}

					ImGui::SliderFloat(_TexCombo[METALLIC + 4].c_str(), &meshfilter->GetMaterialFactor(_materialIndex, METALLIC), 0.0f, 1.0f, "%.05f");
					ImGui::SliderFloat(_TexCombo[SHINENESS + 4].c_str(), &meshfilter->GetMaterialFactor(_materialIndex, SHINENESS), 0.0f, 1.0f, "%.05f");
					ImGui::SliderFloat(_TexCombo[NORMALFACFOR + 4].c_str(), &meshfilter->GetMaterialFactor(_materialIndex, NORMALFACFOR), 0.0f, 2.0f, "%.05f");
					ImGui::SliderFloat(_TexCombo[EMISSIVEFACTOR + 4].c_str(), &meshfilter->GetMaterialFactor(_materialIndex, EMISSIVEFACTOR), 0.0f, 1.0f, "%.05f");

					ImGui::Checkbox("Transparency", &meshfilter->GetMaterialIsTransparent(_materialIndex));


					if (ImGui::Button(".mat save"))
					{
						meshfilter->SaveMaterial(_materialIndex);
					}
					ImGui::SameLine();
					if (ImGui::Button(".mat load"))
					{
						meshfilter->LoadMaterial(_materialIndex);
					}

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}
}

void ImguiClass::Imgui_SkinningAnimation(SkinningAnimation* skinningAni)
{
	if (skinningAni != nullptr)
	{

		std::string _id = "###sa" + std::to_string(skinningAni->GetComponetId());
		std::string _SkinningAni = "SkinningAnimation" + _id;

		if (ImGui::TreeNode(_SkinningAni.c_str()))
		{
			if (skinningAni->GetMeshAnimationCnt() == 0)
			{
				ImGui::Text("no animation ");
			}
			else
			{

				ImGui::Text(skinningAni->GetNowAnimationName().c_str());
				ImGui::SameLine(150.f);

				if (ImGui::BeginCombo(_id.c_str(), skinningAni->GetNowAnimationName().c_str()))
				{
					for (int _aniIndex = 0; _aniIndex < skinningAni->GetMeshAnimationCnt(); _aniIndex++)
					{
						const bool _isSelected = skinningAni->GetMeshAnimationName(_aniIndex) == skinningAni->GetNowAnimationName();
						std::string _anilist = skinningAni->GetMeshAnimationName(_aniIndex) +
							"###" + std::to_string(skinningAni->GetComponetId()) + std::to_string(_aniIndex);

						//선택한 애니메이션으로 바꿔준다.
						if (ImGui::Selectable(_anilist.c_str()))
						{
							skinningAni->SetMeshAnimationIndex(_aniIndex);
						}

						if (_isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}

					}
					ImGui::EndCombo();
				}
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}
}

void ImguiClass::Imgui_Light(Light* light)
{
	if (light != nullptr)
	{
		std::string _id = "###li" + std::to_string(light->GetComponetId());
		std::string _Light = "Light" + _id;

		if (ImGui::TreeNode(_Light.c_str()))
		{
			const char* _lightType[] = { "Directional" , "Point", "Spot" };
			int _lightIndex = 0;
			switch (light->GetLightType())
			{
			case eLIGHT_TYPE::DIRECTIONAL:
			{
				_lightIndex = 0;
				break;
			}
			case eLIGHT_TYPE::POINT:
			{
				_lightIndex = 1;
				break;
			}
			case eLIGHT_TYPE::SPOT:
			{
				_lightIndex = 2;
				break;
			}
			};

			ImGui::Combo(_id.c_str(), &_lightIndex, _lightType, IM_ARRAYSIZE(_lightType), IM_ARRAYSIZE(_lightType));
			switch (_lightIndex)
			{
			case 0:
			{
				light->m_eLightType = eLIGHT_TYPE::DIRECTIONAL;
				break;
			}
			case 1:
			{
				light->m_eLightType = eLIGHT_TYPE::POINT;
				break;
			}
			case 2:
			{
				light->m_eLightType = eLIGHT_TYPE::SPOT;
				break;
			}
			}

			ImVec4 _color = ImVec4(light->GetColor().x, light->GetColor().y, light->GetColor().z, 255.0f / 255.0f);
			std::string _colorEdit = "Light Color###color" + std::to_string(light->GetComponetId());
			if (ImGui::ColorEdit3(_colorEdit.c_str(), (float*)&_color))
			{
				light->SetColor(_color.x, _color.y, _color.z);
			}
			std::string _lightIntensity = "Intensity" + _id + "in";
			float _intensity = light->GetIntensity();
			if (ImGui::SliderFloat(_lightIntensity.c_str(), &_intensity, 0.0f, 1.0f))
			{
				light->SetIntensity(_intensity);
			}
			std::string _lightInput0 = "###FalloffStart" + std::to_string(light->GetComponetId());
			std::string _lightInput1 = "###FalloffEnd" + std::to_string(light->GetComponetId());
			std::string _lightInput2 = "###SpotPower" + std::to_string(light->GetComponetId());
			ImGui::Text("FalloffStart");
			ImGui::SameLine(120.f);
			ImGui::InputFloat(_lightInput0.c_str(), &(light->m_FalloffStart), 0.1f);
			ImGui::Text("FalloffEnd");
			ImGui::SameLine(120.f);
			ImGui::InputFloat(_lightInput1.c_str(), &(light->m_FalloffEnd), 0.1f);
			ImGui::Text("SpotPower");
			ImGui::SameLine(120.f);
			ImGui::InputFloat(_lightInput2.c_str(), &(light->m_SpotPower), 0.1f);

			std::string _shadow = "###CastShadow" + _id + std::to_string(light->GetComponetId());
			ImGui::Text("CastShadow");
			ImGui::SameLine(120.f);
			bool _isCastShadow = light->m_CastShadow;
			if (ImGui::Checkbox(_shadow.c_str(), &_isCastShadow))
			{
				light->m_CastShadow = _isCastShadow;
			}

			ImGui::TreePop();
		}
		ImGui::Separator();
	}
}

void ImguiClass::Imgui_Material(MaterialComponent* material)
{
	if (material != nullptr)
	{
		std::string _id = "###ma" + std::to_string(material->GetComponetId());
		std::string _Material = "Material" + _id;

		if (ImGui::TreeNode(_Material.c_str()))
		{
			int _index = 0;

			std::string _albedo = "AlbedoName" + _id + std::to_string(_index++);

			if (ImGui::BeginCombo(_albedo.c_str(), material->m_AlbedoMapName.c_str()))
			{
				for (int _texIndex = 0; _texIndex < material->m_TextureName_V.size(); _texIndex++)
				{
					const bool _isSelected = material->m_TextureName_V[_texIndex].first == material->GetNowAlbedoName();
					std::string _meshlist = material->m_TextureName_V[_texIndex].first +
						"###" + std::to_string(material->GetComponetId()) + std::to_string(_texIndex);

					//선택한 매쉬로 매쉬필터를 바꿔준다.
					if (ImGui::Selectable(_meshlist.c_str()))
					{
						material->m_AlbedoMapIndex = _texIndex;
						material->m_AlbedoMapName = material->m_TextureName_V[_texIndex].first;
						material->UpdateMaterial();
					}

					if (_isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}

				}
				ImGui::EndCombo();
			}

			std::string _normal = "NormalMap" + _id + std::to_string(_index++);

			if (ImGui::BeginCombo(_normal.c_str(), material->m_NormalMapName.c_str()))
			{
				for (int _texIndex = 0; _texIndex < material->m_TextureName_V.size(); _texIndex++)
				{
					const bool _isSelected = material->m_TextureName_V[_texIndex].first == material->GetNowNormalName();
					std::string _meshlist = material->m_TextureName_V[_texIndex].first +
						"###" + std::to_string(material->GetComponetId()) + std::to_string(_texIndex);

					//선택한 매쉬로 매쉬필터를 바꿔준다.
					if (ImGui::Selectable(_meshlist.c_str()))
					{
						material->m_NormalMapIndex = _texIndex;
						material->m_NormalMapName = material->m_TextureName_V[_texIndex].first;
						material->UpdateMaterial();
					}

					if (_isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			std::string _emissive = "Emissive" + _id + std::to_string(_index++);

			if (ImGui::BeginCombo(_emissive.c_str(), material->m_EmissiveMapName.c_str()))
			{
				for (int _texIndex = 0; _texIndex < material->m_TextureName_V.size(); _texIndex++)
				{
					const bool _isSelected = material->m_TextureName_V[_texIndex].first == material->GetNowEmissiveName();
					std::string _meshlist = material->m_TextureName_V[_texIndex].first +
						"###" + std::to_string(material->GetComponetId()) + std::to_string(_texIndex);

					//선택한 매쉬로 매쉬필터를 바꿔준다.
					if (ImGui::Selectable(_meshlist.c_str()))
					{
						material->m_EmissiveMapIndex = _texIndex;
						material->m_EmissiveMapName = material->m_TextureName_V[_texIndex].first;
						material->UpdateMaterial();
					}

					if (_isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}


			std::string _metalRough = "MetalRoughMap" + _id + std::to_string(_index++);

			if (ImGui::BeginCombo(_metalRough.c_str(), material->m_MetalRoughMapName.c_str()))
			{
				for (int _texIndex = 0; _texIndex < material->m_TextureName_V.size(); _texIndex++)
				{
					const bool _isSelected = material->m_TextureName_V[_texIndex].first == material->GetNowMetalRoughName();
					std::string _meshlist = material->m_TextureName_V[_texIndex].first +
						"###" + std::to_string(material->GetComponetId()) + std::to_string(_texIndex);

					//선택한 매쉬로 매쉬필터를 바꿔준다.
					if (ImGui::Selectable(_meshlist.c_str()))
					{
						material->m_MetalRoughMapIndex = _texIndex;
						material->m_MetalRoughMapName = material->m_TextureName_V[_texIndex].first;
						material->UpdateMaterial();
					}

					if (_isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			std::string _metallic = "10###" + _id + std::to_string(_index++);
			std::string _roughtness = "10###" + _id + std::to_string(_index++);

			ImGui::BulletText("Metal-Roughness");
			ImGui::Text("Metallic :");
			ImGui::SameLine();
			if (ImGui::InputFloat(_metallic.c_str(), &material->m_Metallic, 0.01))
			{
				material->m_Metallic;
				material->UpdateMaterial();
			}

			ImGui::Text("Roughness :");
			ImGui::SameLine();
			if (ImGui::InputFloat(_roughtness.c_str(), &material->m_Shineness, 0.01))
			{
				material->m_Shineness;
				material->UpdateMaterial();
			}


			ImGui::TreePop();
		}

		/*
		for (int _matIndex = 0; _matIndex < material->m_Material_V.size(); _matIndex++)
		{
			std::string _matV = std::to_string(_matIndex) + "###materialV" + _id + std::to_string(_matIndex);
			//if (ImGui::BeginCombo(_matV.c_str(), material->m_Material->m_MaterialName.c_str()))
			{


			}
			ImGui::EndCombo();
		}
		*/
	}

	ImGui::Separator();

}

void ImguiClass::Imgui_Camera(Camera* camera)
{
	if (camera != nullptr)
	{
		std::string _id = "###cam" + std::to_string(camera->GetComponetId());
		std::string _Camera = "Camera" + _id;
		if (ImGui::TreeNode(_Camera.c_str()))
		{
			ImGui::Text(camera->GetName().c_str());
			ImGui::Text("Look at X : %f Y : %f Z : %f", camera->GetLook().x, camera->GetLook().y, camera->GetLook().z);

			ImGui::TreePop();
		}

		ImGui::Separator();
	}
}

void ImguiClass::Imgui_ReflectionProbeCamera(ReflectionProbeCamera* reflectionProbeCam)
{
	if (reflectionProbeCam != nullptr)
	{
		std::string _id = "###reflectionProbeCam" + std::to_string(reflectionProbeCam->GetComponetId());
		std::string _ReflectionProbe = "ReflectionProbeCamera" + _id;
		if (ImGui::TreeNode(_ReflectionProbe.c_str()))
		{
			if (ImGui::Button("Bake"))
			{
				reflectionProbeCam->BakeReflectionProbe();
			}

			bool _isRender = reflectionProbeCam->GetIsRender();
			std::string _nowRender = "Render###isRender" + std::to_string(reflectionProbeCam->GetComponetId());
			if (ImGui::Checkbox(_nowRender.c_str(), &_isRender))
			{
				reflectionProbeCam->SetIsRender(_isRender);
			}

			ImGui::TreePop();
		}

		ImGui::Separator();
	}
}

void ImguiClass::Imgui_NaviMeshController(NaviMeshController* naviMeshController)
{
	if (naviMeshController != nullptr)
	{
		std::string _id = "###naviMeshCon" + std::to_string(naviMeshController->GetComponetId());
		std::string _naviMesh = "NaviMeshController" + _id;
		if (ImGui::TreeNode(_naviMesh.c_str()))
		{
			if (ImGui::Button("Make"))
			{
				naviMeshController->MakeNaviMesh();
			}

			ImGui::Separator();

			//기즈모 On/Off
			bool _debugNavi = naviMeshController->GetDebugMode();
			std::string _debug = "Debug###navi" + std::to_string(naviMeshController->GetComponetId());
			if (ImGui::Checkbox(_debug.c_str(), &_debugNavi))
			{
				naviMeshController->SetDebugMode(_debugNavi);
			}

			std::string _TexCombo[1];
			_TexCombo[0] = "DEBUG_OFFSET###offsetScale";

			float _nowOffset = naviMeshController->GetOffset();
			ImGui::SliderFloat(_TexCombo[0].c_str(), &_nowOffset, 0.0f, 10.0f, "%0.1f");

			naviMeshController->SetOffset(_nowOffset);

			ImGui::TreePop();
		}

		ImGui::Separator();
	}
}

void ImguiClass::Imgui_ParticleSpawner(class ParticleSpawner* pParticleSpawner)
{
	if (pParticleSpawner != nullptr)
	{
		std::string _id = "###ParticleSpawner" + std::to_string(pParticleSpawner->GetComponetId());
		std::string _particle = "ParticleSpawner" + _id;

		if (ImGui::TreeNode(_particle.c_str()))
		{
			int _index = 0;

			{
				std::string _dirPx = "X###" + _id + std::to_string(_index++);
				std::string _dirPy = "Y###" + _id + std::to_string(_index++);
				std::string _dirPz = "Z###" + _id + std::to_string(_index++);

				float _nowDir[3] = {
					pParticleSpawner->GetDirection().x,
					pParticleSpawner->GetDirection().y,
					pParticleSpawner->GetDirection().z, };

				bool _dirChange = false;

				ImGui::BulletText("Direction");
				ImGui::Text("X :");
				ImGui::SameLine();
				if (ImGui::InputFloat(_dirPx.c_str(), &_nowDir[0], 0.1f))
					_dirChange = true;

				ImGui::Text("Y :");
				ImGui::SameLine();
				if (ImGui::InputFloat(_dirPy.c_str(), &_nowDir[1], 0.1f))
					_dirChange = true;

				ImGui::Text("Z :");
				ImGui::SameLine();
				if (ImGui::InputFloat(_dirPz.c_str(), &_nowDir[2], 0.1f))
					_dirChange = true;

				if (_dirChange == true)
					pParticleSpawner->SetDirection(_nowDir);
			}

			{
				std::string _velocity = "Vel###" + _id + std::to_string(_index++);
				float _nowVel = pParticleSpawner->GetVelocity();
				ImGui::Text("Velocity :");
				if (ImGui::SliderFloat(_velocity.c_str(), &_nowVel, 0.0f, 100.0f, "%0.1f"))
				{
					pParticleSpawner->SetVelocity(_nowVel);
				}
			}

			ImGui::Separator();

			{
				std::string _sizePx = "X###" + _id + std::to_string(_index++);
				std::string _sizePy = "Y###" + _id + std::to_string(_index++);

				float _nowSize[2] = {
					pParticleSpawner->GetSize().x,
					pParticleSpawner->GetSize().y, };

				bool _sizeChange = false;

				ImGui::BulletText("Size");
				ImGui::Text("X :");
				ImGui::SameLine();
				if (ImGui::InputFloat(_sizePx.c_str(), &_nowSize[0], 0.1f))
					_sizeChange = true;

				ImGui::Text("Y :");
				ImGui::SameLine();
				if (ImGui::InputFloat(_sizePy.c_str(), &_nowSize[1], 0.1f))
					_sizeChange = true;

				if (_sizeChange == true)
				{
					pParticleSpawner->SetSize(_nowSize);
				}
			}

			ImGui::Separator();

			{
				std::string _randomWeight = "Random###Weight" + std::to_string(pParticleSpawner->GetComponetId());
				float _nowRandomWeight = pParticleSpawner->GetRandomDirWeight();
				if (ImGui::SliderFloat(_randomWeight.c_str(), &_nowRandomWeight, 0.0f, 1.0f, "%0.05f"))
				{
					pParticleSpawner->SetRandomDirWeight(_nowRandomWeight);
				}

			}

			ImGui::Separator();

			{
				std::string _TexCombo[2];
				_TexCombo[0] = "Const_Force###constForce";
				_TexCombo[1] = "Const_ZRot###constZrot";

				float _force = pParticleSpawner->GetForce();
				if (ImGui::SliderFloat(_TexCombo[0].c_str(), &_force, -2.0f, 2.0f, "%0.1f"))
					pParticleSpawner->SetForce(_force);

				float _Zrot = pParticleSpawner->GetRotation();
				if (ImGui::SliderFloat(_TexCombo[1].c_str(), &_Zrot, 0.0f, 360.0f, "%1.0f"))
					pParticleSpawner->SetRotation(_Zrot);
			}

			ImGui::Separator();

			{
				std::string _TexCombo[2];
				_TexCombo[0] = "Delta_Alpha###deltaAlpha";
				_TexCombo[1] = "Delta_Velocity###deltaVelocity";

				float _alpha = pParticleSpawner->GetDeltaAlpha();
				if (ImGui::SliderFloat(_TexCombo[0].c_str(), &_alpha, 0.0f, 1.0f, "%0.05f"))
					pParticleSpawner->SetDeltaAlpha(_alpha);
				

				float _velocity = pParticleSpawner->GetDeltaVelocity();
				if (ImGui::SliderFloat(_TexCombo[1].c_str(), &_velocity, 0.0f, 2.0f, "%0.1f"))
					pParticleSpawner->SetDeltaVelocity(_velocity);

			}

			ImGui::Separator();

			{
				std::string _TexCombo[2];
				_TexCombo[0] = "Delta_Size###deltaSize";
				_TexCombo[1] = "Size_Weight###deltaSizeWeight";

				float _size = pParticleSpawner->GetDeltaSize();
				if (ImGui::SliderFloat(_TexCombo[0].c_str(), &_size, -1.0f, 1.0f, "%0.1f"))
					pParticleSpawner->SetDeltaSize(_size);

				float _sizeWeight = pParticleSpawner->GetRandomSizeWeight();
				if (ImGui::SliderFloat(_TexCombo[1].c_str(), &_sizeWeight, 0.0f, 1.0f, "%0.05f"))
					pParticleSpawner->SetRandomSizeWeight(_sizeWeight);
			}


			ImGui::Separator();

			{
				std::string _TexCombo[2];
				_TexCombo[0] = "Delta_Rotation###deltaRotation";
				_TexCombo[1] = "Rotation_Weight###RotationWeight";

				float _deltaRot = pParticleSpawner->GetDeltaRotation();
				if (ImGui::SliderFloat(_TexCombo[0].c_str(), &_deltaRot, -1.0f, 1.0f, "%0.05f"))
					pParticleSpawner->SetDeltaRotation(_deltaRot);

				float _deltaWeight = pParticleSpawner->GetRandowmRotWeight();
				if (ImGui::SliderFloat(_TexCombo[1].c_str(), &_deltaWeight, 0.0f, 1.0f, "%0.05f"))
					pParticleSpawner->SetRandowmRotWeight(_deltaWeight);
			}


			ImGui::Separator();

			{
				std::string _birth = "birth###birthTime" + std::to_string(pParticleSpawner->GetComponetId());
				float _birthTime = pParticleSpawner->GetBirthTime();
				if (ImGui::SliderFloat(_birth.c_str(), &_birthTime, 0.0f, 10.0f, "%0.05f"))
				{
					pParticleSpawner->SetBirthTime(_birthTime);
				}
			}

			{
				std::string _dead = "dead###deadTime" + std::to_string(pParticleSpawner->GetComponetId());
				float _deadTime = pParticleSpawner->GetDeadTime();
				if (ImGui::SliderFloat(_dead.c_str(), &_deadTime, 0.0f, 10.0f, "%0.05f"))
				{
					pParticleSpawner->SetDeadTime(_deadTime);
				}
				
			}

			{
				if (ImGui::Button("Spawn"))
				{
					pParticleSpawner->SetActive(true);
				}
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

	}
}
